//Written by Vito Devlin


//#########################EXPOSE#######################
#include "declaration.h"
//#########################EXPOSE END#######################

static void init_bitree(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);
static void init_next(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);
static void init_next_no_replace(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c);

static void left_rotate(struct tree_list **bitree);
static void right_rotate(struct tree_list **bitree);
static void calculate_hight_and_factor(struct tree_list **bitree);
static void update_and_checkout_height_and_balance_factor(struct tree_list **bitree);

static void output_bitree_node_to_files(struct tree_list **bitree,FILE *no_child,FILE *with_child,FILE *f_merge,FILE **f_list);

static void free_bitree_node(struct tree_list **bitree);

static void specially_copy(FILE *f_input,FILE *f_output);

static void output_balance_bitree_node_to_file_default(struct tree_list **bitree,FILE *fp);
static void output_balance_bitree_node_to_file_number_order(struct tree_list **bitree,FILE *fp);
static void output_balance_bitree_node_to_file_number_reverse_order(struct tree_list **bitree,FILE *fp);
static void output_balance_bitree_node_to_file(struct tree_list **bitree,FILE *fp,int way);

static void output_bitree_node_to_test_theory(struct tree_list **bitree,FILE *test_theory);

static void reverse_output_line_list(struct line_list **fll,FILE *f_output);

static int search_bitree_delete_node(struct tree_list **bi,wchar_t *a,wchar_t *b,wchar_t *c,struct tree_list **obj_node,struct sub_list **obj_next);
static void copy_bitree_node_for_delete(struct tree_list **node_a,const struct tree_list **node_b);
static void delete_bitree_node_operation(struct tree_list **obj_node);

static void full_codec_disperate(wchar_t *buffer,wchar_t *a1,wchar_t *a2,wchar_t *a3,wchar_t *a4);
static int use_position_to_get_word(struct tree_list *single_words,wchar_t *c,wchar_t *buffer_c,wchar_t *buffer_temp,int bufc_position);
static void set_b_for_wcscat(wchar_t *a,wchar_t *b,int b_pos);

int generate_object_directory(wchar_t *access_path,wchar_t *main_work_path,wchar_t *sub_work_path){

	wchar_t *buffer_path = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);
	wchar_t *main_path = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);
	wchar_t *sub_path_name = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);

	buffer_path[0]=L'\0';
	main_path[0]=L'\0';
	sub_path_name[0]=L'\0';

	if (access_path != NULL)wcscpy( buffer_path,access_path);
	if (main_work_path != NULL)wcscpy( main_path,main_work_path);
	if (sub_work_path != NULL)wcscpy( sub_path_name,sub_work_path);

	struct _stat checkout_directory;
	memset( &checkout_directory,0,sizeof (struct _stat));

	wcscat( buffer_path,main_path);
	wcscat( buffer_path,sub_path_name);

	_wstat(buffer_path,&checkout_directory);

	if (S_ISDIR(checkout_directory.st_mode) != 1){
		if (_wmkdir(buffer_path) == 0){
			OUTPUT_MESSAGE(L"create:%ls\n",buffer_path);
		}else{
			OUTPUT_MESSAGE(L"Error directory");
			return 1;
		}
	}else{
		OUTPUT_MESSAGE(L"%ls,已存在,可能将覆盖\n",buffer_path);
	}

	free(buffer_path);
	free(main_path);
	free(sub_path_name);
	return 0;
}

FILE *generate_object_file(wchar_t *access_path,wchar_t *main_work_path,wchar_t *sub_work_path,wchar_t *file_name_add){

	wchar_t *buffer_path = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);
	wchar_t *main_path = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);
	wchar_t *sub_path_name = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);
	wchar_t *file_name = (wchar_t *)malloc(sizeof (wchar_t) * MAX_PATH);


	buffer_path[0]=L'\0';
	main_path[0]=L'\0';
	sub_path_name[0]=L'\0';
	file_name[0]=L'\0';

	if (access_path != NULL)wcscpy( buffer_path,access_path);
	if (main_work_path != NULL)wcscpy( main_path,main_work_path);
	if (sub_work_path != NULL)wcscpy( sub_path_name,sub_work_path);
	if (file_name_add != NULL)wcscpy( file_name,file_name_add);


	FILE *object_file_p = NULL;

	wcscat( buffer_path , main_path);
	wcscat( buffer_path , sub_path_name);
	wcscat( buffer_path , file_name);
	object_file_p = _wfopen(buffer_path ,L"w+,ccs=UTF-16LE");

	if (object_file_p == NULL){
		OUTPUT_MESSAGE(L"error create");
		return NULL;
	}else{
		OUTPUT_MESSAGE(L"create:%ls\n",buffer_path);
		return object_file_p;
	}

	free(buffer_path);
	free(main_path);
	free(sub_path_name);
	free(file_name_add);
}


int read_line(FILE *fp_in,wchar_t *line_buffer){

	if (line_buffer == NULL)return EOF;

	int line_count;
	wchar_t cur = !(wchar_t)EOF;

	line_count = 0;
	while ( (cur = fgetwc(fp_in)) != L'\n' && cur != (wchar_t)EOF){

		if (line_count < ENOUGH_SPACES - 2 - 1){
			line_buffer[line_count] = cur;
			line_count ++;
		}

	}

	line_buffer[line_count] = L'\n';
	line_count ++;
	line_buffer[line_count] = L'\0';

	if (cur == (wchar_t)EOF)return EOF;
	else return 0;
}


void read_bitree_a(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{
		int var;

		var = wcscmp(a , (**bitree).e.pa);

		if (var < 0){//left

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_a(&((**bitree).left),a,b,c);
			}

		}else if(var > 0){//right
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_a(&((**bitree).right),a,b,c);
			}

		}else{//equal
			
			init_next(bitree,a,b,c);
		}
	}

	//balance
	update_and_checkout_height_and_balance_factor(bitree);

}

void read_bitree_a_no_same(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c,int max_one_min_zero){

	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{
		int var;

		var = wcscmp(a , (**bitree).e.pa);

		if (var < 0){//left

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_a_no_same(&((**bitree).left),a,b,c,max_one_min_zero);
			}

		}else if(var > 0){//right
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_a_no_same(&((**bitree).right),a,b,c,max_one_min_zero);
			}

		}else{//equal
			
			int value_of_b = _wtoi(b);
			int value_of_bitree_e_pb;
			int is_search = 0;

			if (wcscmp(c , (**bitree).e.pc) == 0){
				//search
				is_search = 1;
				value_of_bitree_e_pb = _wtoi((**bitree).e.pb);

				if (max_one_min_zero == 1){//保大
					if (value_of_b > value_of_bitree_e_pb){
						wcscpy((**bitree).e.pb,b);
					}
				}else{//保小
					if (value_of_b < value_of_bitree_e_pb){
						wcscpy((**bitree).e.pb,b);
					}
				}

			}else if ((**bitree).next != NULL){
				
				struct sub_list *next_search = (**bitree).next;

				while(next_search != NULL){
					
					if (wcscmp(c,(*next_search).e.pc) == 0){

						value_of_bitree_e_pb = _wtoi((*next_search).e.pb);

						//search
						is_search = 1;
						if (max_one_min_zero == 1){//保大
							if (value_of_b > value_of_bitree_e_pb){
								wcscpy((*next_search).e.pb,b);
							}
						}else{//保小
							if (value_of_b < value_of_bitree_e_pb){
								wcscpy((*next_search).e.pb,b);
							}
						}

					}

					next_search = (*next_search).next;
				}
			}

			//unsearch
			if (is_search == 0){
				init_next(bitree,a,b,c);
			}
		}

	}

	//balance
	update_and_checkout_height_and_balance_factor(bitree);
}


void read_bitree_b(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{

		int var1,var2,var3;

		var1 = _wtoi(b) - _wtoi((**bitree).e.pb);

		//use b to decide
		if (var1 < 0){//l

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_b(&((**bitree).left),a,b,c);
			}

		}else if(var1 > 0){//r
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_b(&((**bitree).right),a,b,c);
			}

		}else{//use a to decide
			
			var2 = wcscmp(a,(**bitree).e.pa);

			if (var2 < 0){//l
				if ((**bitree).left == NULL){
					init_bitree(&((**bitree).left),a,b,c);
				}else{
					read_bitree_b(&((**bitree).left),a,b,c);
				}
			}else if(var2 > 0){//r
				if ((**bitree).right== NULL){
					init_bitree(&((**bitree).right),a,b,c);
				}else{
					read_bitree_b(&((**bitree).right),a,b,c);
				}

			}else{//use c to decide

				var3 = wcscmp(c,(**bitree).e.pc);

				if (var3 < 0){//l
					if ((**bitree).left == NULL){
						init_bitree(&((**bitree).left),a,b,c);
					}else{
						read_bitree_b(&((**bitree).left),a,b,c);
					}
				}else if(var3 > 0){//r

					if ((**bitree).right== NULL){
						init_bitree(&((**bitree).right),a,b,c);
					}else{
						read_bitree_b(&((**bitree).right),a,b,c);
					}
				}else{
					wcscpy((**bitree).e.pa,a);
					wcscpy((**bitree).e.pb,b);
					wcscpy((**bitree).e.pc,c);
				}

			}

		}

		update_and_checkout_height_and_balance_factor(bitree);
	}

}


void read_bitree_c(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{

		int var1,var2,var3;

		var1 = wcslen(c) - wcslen((**bitree).e.pc);
		if (var1 == 0){
			var1 = wcscmp(c,(**bitree).e.pc);
		}

		//use c to decide
		if (var1 < 0){//l

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_c(&((**bitree).left),a,b,c);
			}

		}else if(var1 > 0){//r
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_c(&((**bitree).right),a,b,c);
			}
		}else{//use a to decide
			
			var2 = wcscmp(a,(**bitree).e.pa);

			if (var2 > 0){//l
				if ((**bitree).left == NULL){
					init_bitree(&((**bitree).left),a,b,c);
				}else{
					read_bitree_c(&((**bitree).left),a,b,c);
				}
			}else if(var2 < 0){//r
				if ((**bitree).right== NULL){
					init_bitree(&((**bitree).right),a,b,c);
				}else{
					read_bitree_c(&((**bitree).right),a,b,c);
				}

			}else{//use b to decide

				var3 = _wtoi(b) - _wtoi((**bitree).e.pb);

				if (var3 > 0){//l
					if ((**bitree).left == NULL){
						init_bitree(&((**bitree).left),a,b,c);
					}else{
						read_bitree_c(&((**bitree).left),a,b,c);
					}
				}else if(var3 < 0){//r

					if ((**bitree).right== NULL){
						init_bitree(&((**bitree).right),a,b,c);
					}else{
						read_bitree_c(&((**bitree).right),a,b,c);
					}
				}else{
					wcscpy((**bitree).e.pa,a);
					wcscpy((**bitree).e.pb,b);
					wcscpy((**bitree).e.pc,c);
				}

			}

		}

		update_and_checkout_height_and_balance_factor(bitree);
	}
}

void read_bitree_a_no_replace(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){
	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{
		int var;

		var = wcscmp(a , (**bitree).e.pa);

		if (var < 0){//left

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_a_no_replace(&((**bitree).left),a,b,c);
			}

		}else if(var > 0){//right
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_a_no_replace(&((**bitree).right),a,b,c);
			}

		}else{//equal
			
			init_next_no_replace(bitree,a,b,c);
		}
	}

	//balance
	update_and_checkout_height_and_balance_factor(bitree);

}

void read_bitree_c_get_full_codec(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;

	if (*bitree == NULL){
		init_bitree(bitree,a,b,c);
	}else{

		int var1,var2;//,var3;

		var1 = wcscmp(c,(**bitree).e.pc);

		//use c to decide
		if (var1 < 0){//l

			if ((**bitree).left == NULL){
				init_bitree(&((**bitree).left),a,b,c);
			}else{
				read_bitree_c_get_full_codec(&((**bitree).left),a,b,c);
			}

		}else if(var1 > 0){//r
			
			if ((**bitree).right== NULL){
				init_bitree(&((**bitree).right),a,b,c);
			}else{
				read_bitree_c_get_full_codec(&((**bitree).right),a,b,c);
			}
		}else if(wcslen(a) > 0){//make a to maximum
			
			var2 = wcslen(a) - wcslen((**bitree).e.pa);

			if(var2 == 0)var2 = wcscmp(a,(**bitree).e.pa);

			if (var2 >= 0){
				#if DEBUG_CODE == 1
				OUTPUT_MESSAGE(L"replace %ls,%ls=%ls with %ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc,a,b,c);//####dbg
				#endif
				wcscpy((**bitree).e.pa,a);
				wcscpy((**bitree).e.pb,b);
				wcscpy((**bitree).e.pc,c);
			}

		}
	}

	update_and_checkout_height_and_balance_factor(bitree);
}


void free_bitree(struct tree_list **bitree){
	if (bitree == NULL)return;

	if (*bitree != NULL){

		free_bitree(&((**bitree).left));
		free_bitree(&((**bitree).right));

		free_bitree_node(bitree);

	}
}


void pre_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
		if (*bitree != NULL){

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif

			pre_order_output_balance_bitree_LR(&((**bitree).left),fp);
			pre_order_output_balance_bitree_LR(&((**bitree).right),fp);
		}
	}
}


void in_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
	
		if (*bitree != NULL){
			in_order_output_balance_bitree_LR(&((**bitree).left),fp);

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif

			in_order_output_balance_bitree_LR(&((**bitree).right),fp);
		}
	}

}


void post_order_output_balance_bitree_LR(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
	
		if (*bitree != NULL){
			post_order_output_balance_bitree_LR(&((**bitree).left),fp);
			post_order_output_balance_bitree_LR(&((**bitree).right),fp);

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif
		}
	}

}


void sequence_output_bitree_LR(struct tree_list **tree_root,FILE *fp){
	if (fp == NULL)return;
	if (tree_root == NULL)return;
	if (*tree_root == NULL)return;

	struct queue_for_tree_node *queue = NULL;
	queue_push(&queue,*tree_root);

	struct tree_list *temp_name = NULL;

	while((*queue).head != NULL){
		
		temp_name = (*((*queue).head)).node;

		#if NUMBER_ORDER == 1
			output_balance_bitree_node_to_file(&temp_name,fp,NUMBER_ORDER);
		#elif NUMBER_REVERSE_ORDER == 1
			output_balance_bitree_node_to_file(&temp_name,fp,NUMBER_REVERSE_ORDER);
		#else
			output_balance_bitree_node_to_file(&temp_name,fp,-1);
		#endif
		
		if ((*temp_name).left != NULL){
			queue_push(&queue,(*temp_name).left);
		}
	
		if ((*temp_name).right != NULL){
			queue_push(&queue,(*temp_name).right);
		}

		queue_pop(&queue);

	}

}


void pre_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
	
		if (*bitree != NULL){

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif

			pre_order_output_balance_bitree_RL(&((**bitree).right),fp);
			pre_order_output_balance_bitree_RL(&((**bitree).left),fp);
		}
	}

}


void in_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
	
		if (*bitree != NULL){
			in_order_output_balance_bitree_RL(&((**bitree).right),fp);

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif

			in_order_output_balance_bitree_RL(&((**bitree).left),fp);
		}
	}

}

void post_order_output_balance_bitree_RL(struct tree_list **bitree , FILE *fp){
	if (bitree != NULL){
		if (*bitree != NULL){
			post_order_output_balance_bitree_RL(&((**bitree).right),fp);
			post_order_output_balance_bitree_RL(&((**bitree).left),fp);

			#if NUMBER_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_ORDER);
			#elif NUMBER_REVERSE_ORDER == 1
				output_balance_bitree_node_to_file(bitree,fp,NUMBER_REVERSE_ORDER);
			#else
				output_balance_bitree_node_to_file(bitree,fp,-1);
			#endif
		}
	}
	

}


void sequence_output_bitree_RL(struct tree_list **tree_root,FILE *fp){
	if (fp == NULL)return;
	if (tree_root == NULL)return;
	if (*tree_root == NULL)return;

	struct queue_for_tree_node *queue = NULL;
	queue_push(&queue,*tree_root);

	struct tree_list *temp_name = NULL;

	while((*queue).head != NULL){
		
		temp_name = (*((*queue).head)).node;

		#if NUMBER_ORDER == 1
			output_balance_bitree_node_to_file(&temp_name,fp,NUMBER_ORDER);
		#elif NUMBER_REVERSE_ORDER == 1
			output_balance_bitree_node_to_file(&temp_name,fp,NUMBER_REVERSE_ORDER);
		#else
			output_balance_bitree_node_to_file(&temp_name,fp,-1);
		#endif
		
	
		if ((*temp_name).right != NULL){
			queue_push(&queue,(*temp_name).right);
		}

		if ((*temp_name).left != NULL){
			queue_push(&queue,(*temp_name).left);
		}

		queue_pop(&queue);

	}

}


int search_bitree_ab(struct tree_list **bi,wchar_t *a,wchar_t *b,const wchar_t *c){

	if(bi != NULL){
	
		if (*bi != NULL){

			if (wcscmp(c,(**bi).e.pc) == 0){
				if (a != NULL)wcscpy( a,(**bi).e.pa);
				if (b != NULL)wcscpy( b,(**bi).e.pb);
				return 1;
			}else if ((**bi).next != NULL){
				
				struct sub_list *next = (**bi).next;

				while(next != NULL){
					
					if (wcscmp(c,(*next).e.pc) == 0){
						if (a != NULL)wcscpy( a,(*next).e.pa);
						if (b != NULL)wcscpy( b,(*next).e.pb);
						return 1;
					}

					next = (*next).next;
				}

			}

			if (search_bitree_ab(&((**bi).left),a,b,c) == 1){
				return 1;
			}

			if (search_bitree_ab(&((**bi).right),a,b,c) == 1){
				return 1;
			}
		}
	}

	return 0;
}


int search_bitree_c(struct tree_list **bi,const wchar_t *a,const wchar_t *b,wchar_t *c){

	if (bi == NULL)return 0;

	if (*bi != NULL){

		if (wcscmp(a,(**bi).e.pa) == 0 && _wtoi(b) == _wtoi((**bi).e.pb)){
			wcscpy( c,(**bi).e.pc);
			return 1;
		}else if ((**bi).next != NULL){
			
			struct sub_list *next = (**bi).next;

			while(next != NULL){
				
				if (wcscmp(a,(*next).e.pa) == 0 && _wtoi(b) == _wtoi((*next).e.pb)){
					wcscpy( c,(*next).e.pc);
					return 1;
				}

				next = (*next).next;
			}

		}

		if (search_bitree_c(&((**bi).left),a,b,c) == 1){
			return 1;
		}

		if (search_bitree_c(&((**bi).right),a,b,c) == 1){
			return 1;
		}
	}

	return 0;
}


int bitree_delete_node(struct tree_list **bi,wchar_t *a,wchar_t *b,wchar_t *c){

	int value_of_return = 0;

	if (bi != NULL){
	
		if (*bi != NULL){

			if (bitree_delete_node(&((**bi).left),a,b,c) == 1 )value_of_return = 1;
			if (bitree_delete_node(&((**bi).right),a,b,c) == 1 )value_of_return = 1;

			if (wcscmp(a,(**bi).e.pa) == 0 && _wtoi(b) == _wtoi((**bi).e.pb) && wcscmp(c,(**bi).e.pc) == 0 ){
				delete_bitree_node_operation(bi);
				value_of_return = 1;

			}else if ((**bi).next != NULL){
				
				struct sub_list **next = &((**bi).next);
				struct sub_list *preserve_cur = NULL;

				while(*next != NULL){
					if (wcscmp(a,(**next).e.pa) == 0 && _wtoi(b) == _wtoi((**next).e.pb) && wcscmp(c,(**next).e.pc) == 0 ){
						preserve_cur = *next;
						*next = (**next).next;
						free(preserve_cur);
						value_of_return = 1;
					}

					next = &((**next).next);
				}
			}

			update_and_checkout_height_and_balance_factor(bi);

		}

	}

	return value_of_return;
}


void generate_codec(struct tree_list *single_words,wchar_t *c,wchar_t *a1,wchar_t *a2,wchar_t *a3,wchar_t *a4){
	if (single_words == NULL)return;
	if (a1 == NULL)return;
	if (a2 == NULL)return;
	if (a3 == NULL)return;
	if (a4 == NULL)return;
	if (c == NULL)return;

	wchar_t buffer[ENOUGH_SPACES]={L'\0'};
	wchar_t buffer_temp[ENOUGH_SPACES]={L'\0'};
	wchar_t buffer_c[ENOUGH_SPACES]={L'\0'};

	int error_collect = 0;
	switch (wcslen(c)){
		case 1:{
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer,0);
			break;
		}case 2:{
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,0);
			if(wcslen(buffer_temp) < 2){OUTPUT_MESSAGE(L"(%ls)编码(%ls)长度小于2,无法编码(%ls).\n",buffer_c,buffer_temp,c);break;}
			set_b_for_wcscat(buffer,buffer_temp,2);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,1);
			if(wcslen(buffer_temp) < 2){OUTPUT_MESSAGE(L"(%ls)编码(%ls)长度小于2,无法编码(%ls).\n",buffer_c,buffer_temp,c);break;}
			set_b_for_wcscat(buffer,buffer_temp,2);
			break;
		}case 3:{
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,0);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,1);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,2);
			if(wcslen(buffer_temp) < 2){OUTPUT_MESSAGE(L"(%ls)编码(%ls)长度小于2,无法编码(%ls).\n",buffer_c,buffer_temp,c);break;}
			set_b_for_wcscat(buffer,buffer_temp,2);
			break;
		}case 4:{
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,0);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,1);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,2);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,3);
			set_b_for_wcscat(buffer,buffer_temp,1);
			break;
		}default:{
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,0);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,1);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,2);
			set_b_for_wcscat(buffer,buffer_temp,1);
			error_collect += use_position_to_get_word(single_words,c,buffer_c,buffer_temp,wcslen(c)-1);
			set_b_for_wcscat(buffer,buffer_temp,1);
			break;
		}

	}

	if (error_collect > 0)buffer[0] = L'\0';
	full_codec_disperate(buffer,a1,a2,a3,a4);
}


void table_format_convert(FILE *in,wchar_t *in_line_format,wchar_t *order,int (*line_filter)(wchar_t *input_line),FILE *out,wchar_t *out_line_format){

	if (in == NULL)return;
	if (in_line_format == NULL)return;
	if (out == NULL)return;
	if (out_line_format == NULL)return;

	if (order == NULL){
		order = L"abc";
	}

	if (line_filter == NULL){
		line_filter = line_filter_default;
	}

	wchar_t *a = (wchar_t *)malloc(sizeof(wchar_t) * ENOUGH_SPACES);
	wchar_t *b = (wchar_t *)malloc(sizeof(wchar_t) * ENOUGH_SPACES);
	wchar_t *c = (wchar_t *)malloc(sizeof(wchar_t) * ENOUGH_SPACES);

	memset( a,0,sizeof(wchar_t) * ENOUGH_SPACES);
	memset( b,0,sizeof(wchar_t) * ENOUGH_SPACES);
	memset( c,0,sizeof(wchar_t) * ENOUGH_SPACES);

	wcscpy( a,L"NULL");
	wcscpy( b,L"NULL");
	wcscpy( c,L"NULL");

	wchar_t line[ENOUGH_SPACES+3];
	int line_count;

	wchar_t d = !(wchar_t)EOF;

	while(d != (wchar_t)EOF){

		line_count = 0;
		while ((d = fgetwc(in)) != L'\n' && d != (wchar_t)EOF){
			if (line_count <= ENOUGH_SPACES){
				line[line_count]=d;
				line_count ++;
			}
		}

		if (d == L'\n'){
			line[line_count] = d;
			line_count++;
		}

		line[line_count] = L'\0';

		if (line_filter(line) == 1){
			if (0 < line_count){

				if (wcscmp(order, L"abc") == 0) {
					swscanf(line, in_line_format, a, b, c);
				} else if (wcscmp(order, L"acb") == 0) {
					swscanf(line, in_line_format, a, c, b);
				} else if (wcscmp(order, L"bca") == 0) {
					swscanf(line, in_line_format, b, c, a);
				} else if (wcscmp(order, L"bac") == 0) {
					swscanf(line, in_line_format, b, a, c);
				} else if (wcscmp(order, L"cab") == 0) {
					swscanf(line, in_line_format, c, a, b);
				} else if (wcscmp(order, L"cba") == 0) {
					swscanf(line, in_line_format, c, b, a);
				} else {
					swscanf(line, in_line_format, a, b, c); // 默认情况
				}

				/*
				if (_wtoi(b) == 0 && b[0] != L'0'){//b是空或其它不能认的则为1
					wcscpy(b,L"1");
				}*/

				fwprintf(out,out_line_format,a,b,c);
				/*
				if (wcscmp(order, L"abc") == 0) {
					fwprintf(out,out_line_format,a,b,c);
				} else if (wcscmp(order, L"acb") == 0) {
					fwprintf(out,out_line_format,a,c,b);
				} else if (wcscmp(order, L"bca") == 0) {
					fwprintf(out,out_line_format,b,c,a);
				} else if (wcscmp(order, L"bac") == 0) {
					fwprintf(out,out_line_format,b,a,c);
				} else if (wcscmp(order, L"cab") == 0) {
					fwprintf(out,out_line_format,c,a,b);
				} else if (wcscmp(order, L"cba") == 0) {
					fwprintf(out,out_line_format,c,b,a);
				} else {
					fwprintf(out,out_line_format,a,b,c);
				}*/
			}
		}

	}

}


int line_filter_default(wchar_t *input_line){
	if (input_line == NULL)return 0;

	if (input_line[0] == L'#'){
		return 0;//no pass
	}
	
	if (input_line[0] == L' '){
		return 0;//no pass
	}

	if (input_line[0] == L'$'){
		return 0;//no pass
	}

	if (input_line[0] == L'\n'){
		return 0;//no pass
	}

	if (input_line[0] == (wchar_t)EOF){
		return 0;//no pass
	}

	return 1;//pass
}


void file_reverse(FILE *fp_in,FILE *fp_out){
	
	if (fp_in == NULL)return;
	if (fp_out == NULL)return;

	fseek(fp_in,2,SEEK_SET);//2 to jump BOM

	wchar_t line_buffer[ENOUGH_SPACES];
	int line_count;
	wchar_t cur = !(wchar_t)EOF;
	struct line_list *file_line_list = NULL;
	struct line_list **cur_line_list_position = &file_line_list;

	while (cur != (wchar_t)EOF){

		line_count = 0;
		while ( (cur = fgetwc(fp_in)) != L'\n' && cur != (wchar_t)EOF){

			if (line_count < ENOUGH_SPACES - 2 - 1){
				line_buffer[line_count] = cur;
				line_count ++;
			}

		}

		line_buffer[line_count] = L'\n';
		line_count ++;
		line_buffer[line_count] = L'\0';

		if (*cur_line_list_position != NULL){
			write_into_line_list(line_buffer,&((**cur_line_list_position).latter_line),cur_line_list_position);
			cur_line_list_position = &((**cur_line_list_position).latter_line);
		}else{
			write_into_line_list(line_buffer,cur_line_list_position,NULL);
		}

	}

	reverse_output_line_list(&file_line_list,fp_out);
	free_line_list(&file_line_list);
}


void free_line_list(struct line_list **fll) {
	if (fll == NULL)return;

	if (*fll == NULL){
		return;
	}

	struct line_list *f_temp_llp = *fll;
	struct line_list *head_llp = NULL;

	while ((*f_temp_llp).latter_line != NULL){
		head_llp = f_temp_llp;
		f_temp_llp = (*f_temp_llp).latter_line;
		free(head_llp);
	}

	free(f_temp_llp);
}


void in_order_output_test_theory(struct tree_list **bitree,FILE *test_theory){
	if (test_theory == NULL)return;

	if (bitree != NULL){
	
		if (*bitree != NULL){
			in_order_output_test_theory(&((**bitree).right),test_theory);

			output_bitree_node_to_test_theory(bitree,test_theory);

			in_order_output_test_theory(&((**bitree).left),test_theory);
		}
	}
}


static void output_bitree_node_to_files(struct tree_list **bitree,FILE *no_child,FILE *with_child,FILE *f_merge,FILE **f_list){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (no_child == NULL)return;
	if (with_child == NULL)return;
	if (f_merge == NULL)return;

	if (f_list == NULL)return;


	if (f_list[0] == NULL)return;

	if (f_list[1] == NULL)return;
	if (f_list[2] == NULL)return;
	if (f_list[3] == NULL)return;
	if (f_list[4] == NULL)return;

	if (f_list[5] == NULL)return;

	if (f_list[6] == NULL)return;

	if (f_list[7] == NULL)return;
	if (f_list[8] == NULL)return;

/*
file_analyse_list[] = {f_practice1,
		f_cl_1 , f_cl_2 , f_cl_3 , f_cl_4 ,
		f_practice1_codec , f_test_theory ,f_practice2 , f_practice2_codec
		};
*/

	int codec_length;
	int codec_num;

	fwprintf(f_merge,L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);//merge

	//1 2 3 4 practice practice_codec
	codec_length = wcslen((**bitree).e.pa);
	codec_num = _wtoi((**bitree).e.pb);

	if (codec_length <= CODEC_LENGTH_SETTING && codec_length >= 1){
		fwprintf(f_list[codec_length],L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);

		if ((codec_length == 1 && codec_num <=2)||(codec_length == 2 && codec_num ==1)){
			if(wcslen((**bitree).e.pc) == 1){
				fwprintf(f_list[0],L"%ls",(**bitree).e.pc);
				fwprintf(f_list[5],L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);
			}
		}

	}

	//####
	if ((**bitree).next == NULL){//no_child
		
		fwprintf(no_child,L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);

	}else{//with_child

		fwprintf(with_child,L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);
		
		struct sub_list *next = (**bitree).next;

		while(next != NULL){
			fwprintf(with_child,L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);
			fwprintf(f_merge,L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);//merge


			//1 2 3 4 practice practice_codec
			codec_num = _wtoi((*next).e.pb);

			if (codec_length <= CODEC_LENGTH_SETTING && codec_length >= 1){
				fwprintf(f_list[codec_length],L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);

				if ((codec_length == 1 && codec_num <=2)||(codec_length == 2 && codec_num ==1)){
					if(wcslen((*next).e.pc) == 1){
						fwprintf(f_list[0],L"%ls",(*next).e.pc);
						fwprintf(f_list[5],L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);
					}

				}

				if (codec_length == 2 && codec_num ==2){
					if(wcslen((*next).e.pc) == 2){
						fwprintf(f_list[8],L"%ls",(*next).e.pc);
						fwprintf(f_list[7],L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);
					}
				}
			}

			next = (*next).next;
		}
			fwprintf(with_child,L"\n");
	}

	if (codec_length >= 0 && codec_length <= 5){
		fwprintf(f_list[codec_length],L"\n");
	}

	//####

}

void write_into_line_list(wchar_t *buf,struct line_list **fll,struct line_list **prelp){

	if (fll == NULL)return;
	if (buf == NULL)return;

	if (*fll == NULL){
		*fll = (struct line_list *)malloc(sizeof(struct line_list));
		if (*fll == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}

		if (prelp == NULL){
			(**fll).previos_line= NULL;
		}else{
			(**fll).previos_line= (*prelp);
		}

		(**fll).latter_line= NULL;

		wcscpy( (**fll).line ,buf);
	}else{
		write_into_line_list(buf,&((**fll).latter_line),fll);
	}

}

void queue_push(struct queue_for_tree_node **queue,struct tree_list *node){
	if (queue == NULL)return;

	if (*queue == NULL){

		*queue = (struct queue_for_tree_node *)malloc(sizeof(struct queue_for_tree_node));
		if (*queue == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}

		(**queue).list = (struct tree_node *)malloc(sizeof (struct tree_node));
		if ((**queue).list == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}

		(**queue).head = (**queue).list;
		(**queue).tail = (**queue).list;
	}else{
		(*(**queue).tail).next = (struct tree_node *)malloc(sizeof (struct tree_node));
		(**queue).tail = (*(**queue).tail).next;
	}

	(*((**queue).tail)).next = NULL;
	(*((**queue).tail)).node = node;
}

int queue_pop(struct queue_for_tree_node **queue){

	if (queue == NULL)return 1;
	if (*queue == NULL)return 1;

	if((**queue).list != NULL){

		if ((*((**queue).list)).next != NULL){
			(**queue).list = (*((**queue).list)).next;
			free((**queue).head);
			(**queue).head = (**queue).list;
			return 0;
		}else{
			(**queue).list = NULL;
			free((**queue).head);
			(**queue).head = NULL;
			return 1;
		}

	}
	
	return 1;
}

static void init_bitree(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){
	if (bitree == NULL)return;

	if (*bitree == NULL){
		*bitree = (struct tree_list *)malloc(sizeof(struct tree_list));
		if (*bitree == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}
	}

	(**bitree).height = 1;
	(**bitree).balance_factor = 0;

	wcscpy( (**bitree).e.pa,a);
	wcscpy( (**bitree).e.pb,b);
	wcscpy( (**bitree).e.pc,c);

	(**bitree).next = NULL;
	(**bitree).left = NULL;
	(**bitree).right = NULL;

}


static void init_next(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	
	struct sub_list **next = &((**bitree).next);
	struct sub_list *temp_next= NULL;

	wchar_t temp_a[ENOUGH_SPACES];
	wchar_t temp_b[ENOUGH_SPACES];
	wchar_t temp_c[ENOUGH_SPACES];

	int cmp_wtoi = _wtoi(b) - _wtoi((**bitree).e.pb);
	if (cmp_wtoi < 0){
		wcscpy(temp_a,(**bitree).e.pa);
		wcscpy(temp_b,(**bitree).e.pb);
		wcscpy(temp_c,(**bitree).e.pc);

		wcscpy( (**bitree).e.pa,a);
		wcscpy( (**bitree).e.pb,b);
		wcscpy( (**bitree).e.pc,c);

		a = temp_a;
		b = temp_b;
		c = temp_c;

	}else if (cmp_wtoi == 0){

		#if DEBUG_CODE == 1
			if (wcscmp((**bitree).e.pc,c) != 0){
				OUTPUT_MESSAGE(L"(%ls,%ls=%ls)将被替为(%ls,%ls=%ls)\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc,a,b,c);
			}
		#endif

		wcscpy( (**bitree).e.pa,a);
		wcscpy( (**bitree).e.pb,b);
		wcscpy( (**bitree).e.pc,c);

		return;
	}

	//####
	
	while (*next != NULL){//大于则向下一个链结点
		if (_wtoi(b) > _wtoi((**next).e.pb)){
			next = &((**next).next);
		}else{
			break;
		}
	}

	if (*next == NULL){//null则插入
		*next = (struct sub_list *)malloc(sizeof (struct sub_list));

		if (*next == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}

		(**next).next = NULL;

		wcscpy( (**next).e.pa,a);
		wcscpy( (**next).e.pb,b);
		wcscpy( (**next).e.pc,c);
	}else{
		if (_wtoi(b) < _wtoi((**next).e.pb)){//小于 前插 继承
			
			temp_next = *next;
			*next = (struct sub_list *)malloc(sizeof (struct sub_list));

			if (*next == NULL){
				OUTPUT_MESSAGE(L"malloc error");
				return;
			}

			(**next).next = temp_next;

			wcscpy( (**next).e.pa,a);
			wcscpy( (**next).e.pb,b);
			wcscpy( (**next).e.pc,c);
		}else{//等于则替换
			
			#if DEBUG_CODE == 1
				if (wcscmp((**bitree).e.pc,c) != 0){
					OUTPUT_MESSAGE(L"(%ls,%ls=%ls)将被替为(%ls,%ls=%ls)\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc,a,b,c);
				}
			#endif
			wcscpy( (**next).e.pa,a);
			wcscpy( (**next).e.pb,b);
			wcscpy( (**next).e.pc,c);
		}
	}
	//####
}

static void init_next_no_replace(struct tree_list **bitree,wchar_t *a,wchar_t *b,wchar_t *c){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	
	struct sub_list **next = &((**bitree).next);
	struct sub_list *temp_next= NULL;

	wchar_t temp_a[ENOUGH_SPACES];
	wchar_t temp_b[ENOUGH_SPACES];
	wchar_t temp_c[ENOUGH_SPACES];

	int cmp_wtoi = _wtoi(b) - _wtoi((**bitree).e.pb);
	if (cmp_wtoi < 0){
		wcscpy(temp_a,(**bitree).e.pa);
		wcscpy(temp_b,(**bitree).e.pb);
		wcscpy(temp_c,(**bitree).e.pc);

		wcscpy( (**bitree).e.pa,a);
		wcscpy( (**bitree).e.pb,b);
		wcscpy( (**bitree).e.pc,c);

		a = temp_a;
		b = temp_b;
		c = temp_c;

	}
	//####
	
	while (*next != NULL){//大于则向下一个链结点
		if (_wtoi(b) > _wtoi((**next).e.pb)){
			next = &((**next).next);
		}else{
			break;
		}
	}

	if (*next == NULL){//null则插入
		*next = (struct sub_list *)malloc(sizeof (struct sub_list));

		if (*next == NULL){
			OUTPUT_MESSAGE(L"malloc error");
			return;
		}

		(**next).next = NULL;

		wcscpy( (**next).e.pa,a);
		wcscpy( (**next).e.pb,b);
		wcscpy( (**next).e.pc,c);
	}else{
		if (_wtoi(b) <= _wtoi((**next).e.pb)){//小于等于 前插 继承
			
			temp_next = *next;
			*next = (struct sub_list *)malloc(sizeof (struct sub_list));

			if (*next == NULL){
				OUTPUT_MESSAGE(L"malloc error");
				return;
			}

			(**next).next = temp_next;

			wcscpy( (**next).e.pa,a);
			wcscpy( (**next).e.pb,b);
			wcscpy( (**next).e.pc,c);
		}
	}
	//####
}


static void left_rotate(struct tree_list **bitree){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;

	struct tree_list *temp_left = (*((**bitree).right)).left;
	struct tree_list *temp_right = (**bitree).right;

	(*((**bitree).right)).left = *bitree;
	(**bitree).right = temp_left;
	*bitree = temp_right;


	calculate_hight_and_factor(&((**bitree).left));
	calculate_hight_and_factor(bitree);

	#if DEBUG_CODE == 1
		output_balance_bitree_to_terminal(bitree);//####dbg
	#endif

}


static void right_rotate(struct tree_list **bitree){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;

	struct tree_list *temp_right = (*((**bitree).left)).right;
	struct tree_list *temp_left = (**bitree).left;

	(*((**bitree).left)).right = *bitree;
	(**bitree).left = temp_right;
	*bitree = temp_left;


	calculate_hight_and_factor(&((**bitree).right));
	calculate_hight_and_factor(bitree);

	#if DEBUG_CODE == 1
	output_balance_bitree_to_terminal(bitree);//####dbg
	#endif
}


static void calculate_hight_and_factor(struct tree_list **bitree){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;

	struct tree_list **bitree_left = &((**bitree).left);
	struct tree_list **bitree_right = &((**bitree).right);

	int temp_left_height,temp_right_height;

	if ((*bitree_left) == NULL){
		temp_left_height = 0;
	}else{
		temp_left_height = (**bitree_left).height;
	}

	if ((*bitree_right) == NULL){
		temp_right_height = 0;
	}else{
		temp_right_height = (**bitree_right).height;
	}

	(**bitree).height = 1 + ((temp_left_height >= temp_right_height)?temp_left_height:temp_right_height);

	(**bitree).balance_factor = temp_left_height - temp_right_height;//lh - rh

}


static void update_and_checkout_height_and_balance_factor(struct tree_list **bitree){


	if (bitree != NULL){
		if (*bitree != NULL){
		
			struct tree_list **bitree_left = &((**bitree).left);
			struct tree_list **bitree_right = &((**bitree).right);

			calculate_hight_and_factor(bitree);

			//checkout
			enum situation{NORMAL = 0,LL,RR,LR,RL};
			enum situation value;
			
			value = NORMAL;

			#if DEBUG_CODE == 1
			OUTPUT_MESSAGE(L"update_balance:%ls,%ls=%ls\tbf:%d\th:%d\n",\
					(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc,(**bitree).balance_factor,(**bitree).height);//####dbg
			#endif

			if((**bitree).balance_factor == 2 && *bitree_left != NULL){

				if ((**bitree_left).balance_factor == 1){//LL
					value = LL;
				}else if((**bitree_left).balance_factor == 0){//2 0 LL
					value = LL;
				}else if((**bitree_left).balance_factor == -1){//LR
					value = LR;
				}else{
					#if DEBUG_CODE == 1
					OUTPUT_MESSAGE(L"#######left######\n");
					output_balance_bitree_to_terminal(bitree);//####dbg
					OUTPUT_MESSAGE(L"erorr");
					#endif
				}

			}

			if ((**bitree).balance_factor == -2 && *bitree_right != NULL){

				if ((**bitree_right).balance_factor == 1){//RL
					value = RL;
				}else if((**bitree_right).balance_factor == 0){//-2 0 RR
					value = RR;
				}else if((**bitree_right).balance_factor == -1){//RR
					value = RR;
				}else{
					#if DEBUG_CODE == 1
					OUTPUT_MESSAGE(L"#######right######\n");
					output_balance_bitree_to_terminal(bitree);//####dbg
					OUTPUT_MESSAGE(L"erorr");
					#endif
				}

			}


			if (value != NORMAL){
			
				if (value == LL){
					right_rotate(bitree);
				}else if (value == RR){
					left_rotate(bitree);
				}else if (value == LR){
					left_rotate(bitree_left);
					right_rotate(bitree);
				}else if (value == RL){
					right_rotate(bitree_right);
					left_rotate(bitree);
				}

			}

		}
	}

}


void output_bitree_to_files(struct tree_list **bitree,FILE *no_child,FILE *with_child,FILE *f_merge,FILE **f_list){
	if (bitree == NULL)return;

	if (*bitree != NULL){

		output_bitree_to_files(&((**bitree).left),no_child,with_child,f_merge,f_list);
		output_bitree_node_to_files(bitree,no_child,with_child,f_merge,f_list);
		output_bitree_to_files(&((**bitree).right),no_child,with_child,f_merge,f_list);

	}
}


static void free_bitree_node(struct tree_list **bitree){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	
	if ((**bitree).next != NULL){//with_child

		struct sub_list *pre = NULL;
		struct sub_list *cur = (**bitree).next;

		while(cur != NULL){
			pre = cur;
			cur = (*cur).next;

			free((void *)pre);
		}
	}

	free((void*) *bitree);//now no_child
}


static void specially_copy(FILE *f_input,FILE *f_output){

	if (f_input == NULL)return;
	if (f_output == NULL)return;

	fseek(f_input,2,SEEK_SET);//use 2 to pass <fffe>

	wchar_t line[ENOUGH_SPACES*3];
	int line_count;
	wchar_t d;


	while(!feof(f_input) && !ferror(f_input)){

		line_count = 0;
		while ((d = fgetwc(f_input)) != L'\n' && !feof(f_input) && !ferror(f_input)){
			line[line_count]=d;
			line_count++;
		}

		if (d == L'\n'){
			line[line_count] = d;
			line_count++;
		}

		line[line_count] = L'\0';

		if (line[0] != L' ' && line[0] != L';' && line[0] != L'\n'){
			if (0 < line_count){
				fputws(line,f_output);
			}
		}
	}


}


void phrases_generate(FILE *sougou_phrases,FILE *f1,FILE *f2,FILE *f3,FILE *f4){

	if (sougou_phrases == NULL)return;
	if (f1 == NULL)return;
	if (f2 == NULL)return;
	if (f3 == NULL)return;
	if (f4 == NULL)return;

	specially_copy(f1,sougou_phrases);
	specially_copy(f2,sougou_phrases);
	specially_copy(f3,sougou_phrases);
	specially_copy(f4,sougou_phrases);

}


static void output_balance_bitree_node_to_file_default(struct tree_list **bitree,FILE *fp){
	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (fp == NULL)return;

	if ((**bitree).next == NULL){//no_child

		fwprintf(fp,L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);

	}else{//with_child

		fwprintf(fp,L"%ls,%ls=%ls\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc);
		
		struct sub_list *next = (**bitree).next;

		while(next != NULL){
			fwprintf(fp,L"%ls,%ls=%ls\n",(*next).e.pa,(*next).e.pb,(*next).e.pc);
			next = (*next).next;
		}
	}

}


static void output_balance_bitree_node_to_file_number_order(struct tree_list **bitree,FILE *fp){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (fp == NULL)return;

	int number;
	number = 1;

	if ((**bitree).next == NULL){//no_child

		fwprintf(fp,L"%ls,%d=%ls\n",(**bitree).e.pa,number,(**bitree).e.pc);

	}else{//with_child

		fwprintf(fp,L"%ls,%d=%ls\n",(**bitree).e.pa,number,(**bitree).e.pc);
		
		struct sub_list *next = (**bitree).next;

		while(next != NULL){
			number++;
			fwprintf(fp,L"%ls,%d=%ls\n",(*next).e.pa,number,(*next).e.pc);
			next = (*next).next;
		}
	}

}


static void output_balance_bitree_node_to_file_number_reverse_order(struct tree_list **bitree,FILE *fp){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (fp == NULL)return;

	int number;
	number = 1;

	if ((**bitree).next == NULL){//no_child

		fwprintf(fp,L"%ls,%d=%ls\n",(**bitree).e.pa,number,(**bitree).e.pc);

	}else{//with_child

		struct sub_list *next = (**bitree).next;

		while(next != NULL){
			number++;
			next = (*next).next;
		}

		fwprintf(fp,L"%ls,%d=%ls\n",(**bitree).e.pa,number,(**bitree).e.pc);

		next = (**bitree).next;
		while(next != NULL){
			number--;
			fwprintf(fp,L"%ls,%d=%ls\n",(*next).e.pa,number,(*next).e.pc);
			next = (*next).next;
		}

	}

}


static void output_balance_bitree_node_to_file(struct tree_list **bitree,FILE *fp,int way){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (fp == NULL)return;

	if (way == NUMBER_ORDER){
		output_balance_bitree_node_to_file_number_order(bitree,fp);
	}else if (way == NUMBER_REVERSE_ORDER){
		output_balance_bitree_node_to_file_number_reverse_order(bitree,fp);
	}else{
		output_balance_bitree_node_to_file_default(bitree,fp);
	}

}


static void output_balance_bitree_to_terminal(struct tree_list **bitree){//###dbg

	if (bitree == NULL)return;

	if (*bitree != NULL){
		OUTPUT_MESSAGE(L"%ls,%ls=%ls\tbf:%d\th:%d\n",(**bitree).e.pa,(**bitree).e.pb,(**bitree).e.pc,(**bitree).balance_factor,(**bitree).height);
		output_balance_bitree_to_terminal(&((**bitree).left));
		output_balance_bitree_to_terminal(&((**bitree).right));
	}

}


static void output_bitree_node_to_test_theory(struct tree_list **bitree,FILE *test_theory){

	if (bitree == NULL)return;
	if (*bitree == NULL)return;
	if (test_theory == NULL)return;

	int codec_length;
	int codec_num;

	codec_length = wcslen((**bitree).e.pa);
	codec_num = _wtoi((**bitree).e.pb);

	fwprintf(test_theory,L"%ls\t%ls%ls",(**bitree).e.pc,(**bitree).e.pa,\
			codec_num ==1 ? L"" : (**bitree).e.pb );//test_theory

	if (codec_num == 1 && codec_length < 4){
		fwprintf(test_theory,L"_\n");
	}else{
		fwprintf(test_theory,L"\n");
	}
}


static void reverse_output_line_list(struct line_list **fll,FILE *f_output){

	if (fll == NULL)return;
	if (*fll == NULL)return;
	if (f_output == NULL)return;

	struct line_list **f_temp_line_listp = fll;

	while ((**f_temp_line_listp).latter_line != NULL){
		f_temp_line_listp = &((**f_temp_line_listp).latter_line );
	}

	while (*f_temp_line_listp != NULL){
		fwprintf(f_output,L"%ls",(**f_temp_line_listp).line);
		f_temp_line_listp = &((**f_temp_line_listp).previos_line);
	}

}


//addresses of obj_node,obj_next both belong in parent node spaces.
static int search_bitree_delete_node(struct tree_list **bi,wchar_t *a,wchar_t *b,wchar_t *c,struct tree_list **obj_node,struct sub_list **obj_next){

	if (bi == NULL)return 0;

	if (*bi != NULL){

		if (wcscmp(a,(**bi).e.pa) == 0 && _wtoi(b) == _wtoi((**bi).e.pb) && wcscmp(c,(**bi).e.pc) == 0 ){
			obj_node = bi;
			obj_next = NULL;
			return 1;
		}else if ((**bi).next != NULL){
			
			struct sub_list *next = (**bi).next;

			while(next != NULL){
				if (wcscmp(a,(*next).e.pa) == 0 && _wtoi(b) == _wtoi((*next).e.pb) && wcscmp(c,(*next).e.pc) == 0 ){
				obj_node = bi;
				obj_next = &next;
				return 1;
				}

				next = (*next).next;
			}

		}

		if (search_bitree_c(&((**bi).left),a,b,c) == 1){
			return 1;
		}

		if (search_bitree_c(&((**bi).right),a,b,c) == 1){
			return 1;
		}
	}

	return 0;
}


static void copy_bitree_node_for_delete(struct tree_list **node_a,const struct tree_list **node_b){
	if (node_a == NULL)return;
	if (node_b == NULL)return;
	if (*node_a == NULL)return;
	if (*node_b == NULL)return;

	wcscpy((**node_a).e.pa,(**node_b).e.pa);
	wcscpy((**node_a).e.pb,(**node_b).e.pb);
	wcscpy((**node_a).e.pc,(**node_b).e.pc);
	(**node_a).next = (**node_b).next;

}


static void free_for_delete(struct tree_list *bi){

	if (bi != NULL){
		if ((*bi).next != NULL){
			wcscpy((*bi).e.pa,(*((*bi).next)).e.pa);
			wcscpy((*bi).e.pb,(*((*bi).next)).e.pb);
			wcscpy((*bi).e.pc,(*((*bi).next)).e.pc);

			struct sub_list *next = (*bi).next;
			(*bi).next = bi -> next -> next;
			free(next);
		}else{
			free(bi);
		}
	}

}


static void delete_bitree_node_operation(struct tree_list **obj_node){

	if (obj_node != NULL){
		if(*obj_node != NULL){
			struct tree_list *temp_node = NULL;

			if ((**obj_node).right == NULL && (**obj_node).left == NULL){//leaf
				free_for_delete((*obj_node));
				*obj_node = NULL;
			}else if((**obj_node).right == NULL){//one left child
				temp_node = (*obj_node);
				*obj_node = (**obj_node).left;
				free_for_delete(temp_node);
			}else if ((**obj_node).left == NULL){//one right child
				temp_node = (*obj_node);
				*obj_node = (**obj_node).right;
				free_for_delete(temp_node);
			}else{//double child
				//find direct successor
				if ((**obj_node).next != NULL){
					free_for_delete(*obj_node);
				}else{
					struct tree_list **temp_node_parent_p = &((**obj_node).right);
					struct tree_list **temp_node_grand_p = obj_node;

					while ((**temp_node_parent_p).left != NULL){
						temp_node_grand_p = temp_node_parent_p;
						temp_node_parent_p = &((**temp_node_parent_p).left);
					}
					
					copy_bitree_node_for_delete(obj_node , (const struct tree_list **)temp_node_parent_p);
					delete_bitree_node_operation(temp_node_parent_p);
					//balance temp_node_parent_p
					update_and_checkout_height_and_balance_factor(temp_node_grand_p);
				}

			}

		}
	}
}


static void full_codec_disperate(wchar_t *buffer,wchar_t *a1,wchar_t *a2,wchar_t *a3,wchar_t *a4){
	if (buffer == NULL)return;

	int len_buffer = wcslen(buffer);

	if (len_buffer >= 4){
		a4[0]=buffer[0];
		a4[1]=buffer[1];
		a4[2]=buffer[2];
		a4[3]=buffer[3];
		a4[4]=L'\0';
	}else{
		a4[0]=L'\0';
	}

	if (len_buffer >= 3){
		a3[0]=buffer[0];
		a3[1]=buffer[1];
		a3[2]=buffer[2];
		a3[3]=L'\0';
	}else{
		a3[0]=L'\0';
	}

	if (len_buffer >= 2){
		a2[0]=buffer[0];
		a2[1]=buffer[1];
		a2[2]=L'\0';
	}else{
		a2[0]=L'\0';
	}

	if (len_buffer >= 1){
		a1[0]=buffer[0];
		a1[1]=L'\0';
	}else{
		a1[0]=L'\0';
	}

}


static int use_position_to_get_word(struct tree_list *single_words,wchar_t *c,wchar_t *buffer_c,wchar_t *buffer_temp,int bufc_position){

	buffer_c[0] = c[bufc_position];
	buffer_c[1] = L'\0';
	if (search_bitree_ab(&single_words,buffer_temp,NULL,buffer_c) != 1){
		OUTPUT_MESSAGE(L"单字全码树中无:%ls(%0.2x %0.2x)\t来自字词:%ls\n",buffer_c,((unsigned char*)buffer_c)[1],((unsigned char*)buffer_c)[0],c);
		return 1;
	}

	return 0;
}


static void set_b_for_wcscat(wchar_t *a,wchar_t *b,int b_pos){
	if (wcslen(b) < b_pos){
		OUTPUT_MESSAGE(L"error %ls less than %d(b_pos).\n",b,b_pos);
	}else{
		b[b_pos] = L'\0';
		wcscat(a,b);
	}
}



//Written by Vito Devlin
