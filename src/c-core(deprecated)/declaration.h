//Written by Vito Devlin

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#include <dirent.h>
#include <sys/stat.h>


//warning:非终端时window_message定义在前
#ifndef OUTPUT_MESSAGE
	#define OUTPUT_MESSAGE(str,...) wprintf((str) __VA_OPT__(,) __VA_ARGS__)
#endif

//输出定点DBG信息
#ifndef DEBUG_CODE
#define DEBUG_CODE 0
#endif

#ifndef MAX_PATH
	#define MAX_PATH 260
#endif
#define ENOUGH_SPACES 128

#define CODEC_LENGTH_SETTING 4

#define DIR_ERROR 0x0010
#define FILE_ERROR 0x0020

//在词频作为输入时能使用来使高频词位置前置
//
//output_balance_bitree_node_to_file的工作方式指定
//NUMBER_ORDER 1 NUMBER_REVERSE_ORDER 0 使用正序号
//NUMBER_ORDER 0 NUMBER_REVERSE_ORDER 1 使用逆序号
//NUMBER_ORDER 0 NUMBER_REVERSE_ORDER 0 不使用序号,保留原数
//NUMBER_ORDER 1 NUMBER_REVERSE_ORDER 1 未定义行为，但优先正序号
#define NUMBER_ORDER 1
#define NUMBER_REVERSE_ORDER 0


//基本元素
struct element{
	wchar_t pa[ENOUGH_SPACES];//编码
	wchar_t pb[ENOUGH_SPACES];//序号
	wchar_t pc[ENOUGH_SPACES];//字词
};


//树链表的链
struct sub_list{
	struct element e;

	struct sub_list *next;
};

//带链的树，故tree list ...
struct tree_list{
	struct element e;

	struct sub_list *next;//候选表

	int height;//树高
	int balance_factor;//平衡因子

	struct tree_list *left;
	struct tree_list *right;
};

//储行链表
struct line_list{
	wchar_t line[ENOUGH_SPACES];
	struct line_list *previos_line;
	struct line_list *latter_line;
};

//树节点链表
struct tree_node{
	struct tree_list *node;

	struct tree_node *next;
};

//层序用的队列
struct queue_for_tree_node{

	struct tree_node *list;

	struct tree_node *head;
	struct tree_node *tail;
};


FILE *generate_object_file(wchar_t *access_path,wchar_t *main_work_path,wchar_t *sub_work_path,wchar_t *file_name_add);//创建文件,成1失败0
int generate_object_directory(wchar_t *access_path,wchar_t *main_work_path,wchar_t *sub_work_path);//创建目录,成1失败0
//e.g. 
//create D:/main_work_path/sub_work_path 
//create D:/main_work_path/sub_work_path/file.txt
//
//	if (generate_object_directory(L"D:",NULL,L"/main_work_path") == 1)return DIR_ERROR;
//	if (generate_object_directory(L"D:",L"/main_work_path",L"/sub_work_path") == 1)return DIR_ERROR;
//	FILE *fp = generate_object_file(L"D:",L"/main_work_path",L"/sub_work_path",L"/file.txt");

int read_line(FILE *fp_in,wchar_t *line_buffer);//fp_in当前位置读入行到line_buffer，EOF时返回-1，否则0

//将a,b,c读入树，其中a编码,b序号,c字词
void read_bitree_a(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);//a串决定左右子树
void read_bitree_b(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);//b决定
void read_bitree_c(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);//c串决定

void read_bitree_a_no_same(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c,int max_one_min_zero);//a串决定，相同字词时相同编码时,max_one_min_zero决定保大保小
void read_bitree_a_no_replace(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);//a串决定,编码序号相同时不替换
void read_bitree_c_get_full_codec(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);//c串决定,为提取最长a

void free_bitree(struct tree_list **bitree);//释放bitree

//先左子后右子类
//LR
void pre_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp);//先序到文件
void in_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp);//中序到文件
void post_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp);//后序到文件
void sequence_output_bitree_LR(struct tree_list **tree_root,FILE *fp);//层序到文件

//先右子后左子类
//RL
void pre_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp);//先序到文件
void in_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp);//中序到文件
void post_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp);//后序到文件
void sequence_output_bitree_RL(struct tree_list **tree_root,FILE *fp);//层序到文件

//a编码,b序号,c字词
int search_bitree_ab(struct tree_list **bi,wchar_t *a,wchar_t *b,const wchar_t *c);//使用c查找返回在a,b,找到1，否则0
int search_bitree_c(struct tree_list **bi,const wchar_t *a,const wchar_t *b,wchar_t *c);//使用a,b查找返回在c，找到1，否则0

int bitree_delete_node(struct tree_list **bi,wchar_t *a,wchar_t *b,wchar_t *c);//在bitree中删除a,b,c完全相同的节点,成功1,失败0

//生成c的编码于a,注：single_words为4码对应方案"全码的单字"树
//a1,a2,a3,a4,分别是1长编码，2长编码……
//注：像86中"我、有"这种有两种编码的在树中不应保留两份编码
void generate_codec(struct tree_list *single_words,wchar_t *c,wchar_t *a1,wchar_t *a2,wchar_t *a3,wchar_t *a4);


//码表转换格式
//in,out 输入输出文件
//in_line_format,out_line_format,可用swscanf的格式参数
//line_filter,行过滤
//order,可以为L"abc",L"acb",L"bca"...来指定a,b,c对应输入的哪部分
void table_format_convert(FILE *in,wchar_t *in_line_format,wchar_t *order,int (*line_filter)(wchar_t *input_line),FILE *out,wchar_t *out_line_format);
int line_filter_default(wchar_t *input_line);//默认可用的行过滤器
//e.g.
//table_format_convert(filep_in,L"%l[^,],%l[^=]=%l[^\n]\n",L"acb",&line_filter_default,filep_out,L"%ls\t%ls%ls\n");

void file_reverse(FILE *fp_in,FILE *fp_out);//文件行倒置,将fp_in倒置放入fp_out
void free_line_list(struct line_list **fll);//行空间回堆释放

void in_order_output_test_theory(struct tree_list **bitree,FILE *test_theory);//理论码长测试表生成,请read_bitree_c读入树传入

void output_bitree_to_files(struct tree_list **bitree,FILE *no_child,FILE *with_child,FILE *f_merge,FILE **f_list);

void phrases_generate(FILE *sougou_phrases,FILE *f1,FILE *f2,FILE *f3,FILE *f4);

void write_into_line_list(wchar_t *buf,struct line_list **fll,struct line_list **prelp);

void queue_push(struct queue_for_tree_node **queue,struct tree_list *node);
int queue_pop(struct queue_for_tree_node **queue);



