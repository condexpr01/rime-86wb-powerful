#include "../cppcore/core.impl.cpp"

using namespace table;

//多音字的多编码对一个字，变成输出一编码一字
//seperator不能为whitesapce(这在make_table的时候会被当噪声去掉)
int main(int argc ,char *argv[]) try {

	error_type error = nullptr;
	bool is_ok = false;

	table_t pinyin{detect_file_from_args(argc,argv),table_category::key_codec};
	if(!pinyin.is_ok){throw std::runtime_error{pinyin.error};}
	
	vector<string> buffer{};
	string seperators{"/"};
	for (auto &&k : pinyin){

		epcall(string_sep_vector(k.first,seperators,buffer),error,is_ok);
		if(!is_ok){throw std::runtime_error{error};}

		for (auto &&kb : buffer){
			cout << format("{},{}={},{}\n",
				kb,k.second[0],k.second[1],k.second[2]);
		}

	}

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}




