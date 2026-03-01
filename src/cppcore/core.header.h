
#include <iostream>
#include <fstream>

#include <concepts>

#include <expected>
#include <filesystem>

#include <unordered_map>
#include <vector>
#include <array>

#include <string>
#include <cstring>

#include <string_view>

namespace table{
	
	//异常ep unep
	template<typename T>
	using ep = std::expected<T,std::string>;
	using unep = std::unexpected<std::string>;
	
	//ep wrapper, 保留值
	template<typename T>
	inline void epw(ep<T> &&r,std::string &s,T &value){
		if (r.has_value()){
			s.clear();
			value = std::move(r.value());
		}else s = std::move(r.error());
	};
	
	//ep wrapper void重载,弃值
	template<typename T>
	inline void epw(ep<T> &r,std::string &s){
		if (r.has_value()){
			s.clear();
		}else s = r.error();
	};

	//ep wrapper void重载,弃值
	template<typename T>
	inline void epw(ep<T> &&r,std::string &s){
		if (r.has_value()){
			s.clear();
		}else s = std::move(r.error());
	};
	
	//ios
	using std::cout;
	using std::cin;
	using std::clog;
	using std::cerr;
	using std::endl;
	
	//fsys
	using std::filesystem::directory_iterator;

	//流
	using std::ifstream;
	using std::ofstream;
	using std::istream;
	using std::ostream;
	using std::format;
	
	//串
	using std::getline;
	using std::string;
	using std::string_view;

	//容器
	using std::unordered_multimap;
	using std::vector;
	using std::array;
	using std::pair;
	using std::get;

	//concepts
	using std::same_as;
	using std::remove_cvref_t;
	using std::is_const_v;


	//表flag
	//cat为key_codec,期望键存在编码,对应位置:[编码],序号=字词,频数
	//cat为key_word ,期望键存在字词,对应位置:编码,序号=[字词],频数
	enum class table_category: int{
		key_none,  //无键状态
		key_codec, //编码为键哈希表的flag
		key_word,  //字词为键哈希表的flag
	};

	//表对象类
	class table_t;

	//从命令行参数中检测文件
	inline ep<ifstream> detect_file_from_args(const int& argc ,const char ** &argv);

	//递归地显示目录结构
	inline ep<void> dir_layout(ostream& output,const string &s_path);

	//用vector<pair<string,vector<string>>>表示的table,
	//与table_t不同的是, 元素顺序与读入先后顺序有关
	ep<void> make_vector_table(ifstream& ifs,vector<pair<string,vector<string>>>& v,const table_category cat);

	//string切片视图,闭开区间(返回必正常,无unep)
	inline string_view string_slice(const string_view &s,
		const size_t startpos,const size_t endpos = string::npos)
	noexcept{
		if (startpos == endpos)return {};
		if (startpos > endpos) return {};

		//startpos < endpos :
		if (startpos >= s.size()) return {};
		if (endpos == string::npos || endpos >= s.size()) return s.substr(startpos);
		else return s.substr(startpos, endpos - startpos);
	}

	//将sep_set中的字符作为分隔符，切片s到vector中
	ep<void> string_sep_vector(const string & s, const string & sep_set, vector<string> &v);
	
	//字符串加法
	ep<std::string> string_add(const std::string& num1, const std::string& num2);

	//字符串减法(负数会变为0, 因为不希望出现负频数)
	ep<std::string> string_sub(const std::string& num1, const std::string& num2);

	//u8长度(返回必正常,无unep)
	inline size_t utf8_length(const string &s) noexcept;
	
	//u8从begin开始的结束位置定位,闭开[begin,end)(返回必正常,无unep)
	inline size_t utf8_word_locate(const string &s, size_t begin) noexcept;

	//过滤不可见字符(返回必正常,无unep)
	string string_visiable(const string &s);
	
	//编码器(从single_word_table编码target)
	ep<void> encoder(const table_t &single_word_table, table_t &target);

	//从from取频数到to
	ep<void> get_freq(const table_t &from, table_t &to);

	
	//交集
	ep<void> word_set_intersect(table_t &intersection,const table_t &with);

	//并集
	ep<void> word_set_unite(table_t &union_set,const table_t &with);

	//差集
	ep<void> word_set_difference(table_t &difference,const table_t &with);

	ep<void> codec_set_unite(table_t &union_set,const table_t &with);
	ep<void> codec_set_intersect(table_t &intersection,const table_t &with);
	ep<void> codec_set_difference(table_t &difference,const table_t &with);
}



