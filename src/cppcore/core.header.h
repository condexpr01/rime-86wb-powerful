
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
	
	//错误串类型
	typedef const char* error_type;

	//错误类型
	template<typename T>
	using ep = std::expected<T,error_type>;
	using unep = std::unexpected<error_type>;

	//错误串空检测
	template<typename errT>
	inline bool is_error_empty(errT error)
	requires requires(errT s){
		{!s}->std::convertible_to<bool>;
		{*s}->std::convertible_to<const char>;
	}{
		return !error || (*error == '\0');
	}

	//便捷调用并转移值
	//当然也可以用expected原生and_then, or_else, transform, transform_error
	template <typename epT,typename returnT>
	void epcall(epT&& epr, returnT &value, error_type &error, bool &is_ok) {
		if (epr.has_value()) {
			is_ok = true;
			error = nullptr;
			value = std::move(epr.value());
		} else {
			is_ok = false;
			error = epr.error(); // 指针无需move
		}
	}
	
	//epcall弃值版重载
	template <typename epT>
	void epcall(epT&& epr, error_type &error, bool &is_ok) {
		if (epr.has_value()) {
			is_ok = true;
			error = nullptr;
		} else {
			is_ok = false;
			error = epr.error();
		}
	}

	//epcall弃值,弃状态版重载
	template <typename epT>
	void epcall(epT&& epr, error_type &error) {
		if (epr.has_value()) {
			error = nullptr;
		}else{
			error = epr.error();
		}
	}

	
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



