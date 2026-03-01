
#include "../cppcore/core.impl.cpp"

using namespace table;


int main(int argc ,char *argv[]) try {

	string error;

	vector<pair<string,vector<string>>> table;
	
	ifstream ifs;
	epw(detect_file_from_args(argc,argv),error,ifs);
	if(!error.empty()){throw std::runtime_error{error.c_str()};}

	epw(make_vector_table(ifs,table,table_category::key_codec),error);
	if(!error.empty()){throw std::runtime_error{error.c_str()};}
	
	//比table_t的string多套个vector用来聚合
	//seq: first,second[x][0]=second[x][1],second[x][2]
	std::unordered_map<string,vector<vector<string>>> seq;
	for (auto &&k : table){ seq[k.first].push_back(std::move(k.second));}
	
	//聚合后排序
	for(auto &&k : seq){

		std::sort(k.second.begin(),k.second.end(),[](const auto &a,const auto &b){
			if (a.at(0).empty() || b.at(0).empty()) return false;

			if(a[0].length() != b[0].length()) return a[0].length() < b[0].length();
			return a[0] < b[0];
		});
		
		size_t count = 1;
		for (auto &&inner : k.second){
			inner[0] = std::to_string(count);
			count ++;
		}
	}

	vector<pair<string, vector<vector<string>>>> v;
	v.reserve(seq.size());
	for (auto&& [k, val] : seq) {
		v.emplace_back(std::move(k), std::move(val));
	}

	//按 codec 排序
	std::sort(v.begin(), v.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
	});

	//输出
	for(auto &&i : v){
		for (auto &&inner : i.second){

			//wubi限制: 单字不限位置, 词不超过3选
			if(std::stoull(inner[0]) <= 3 || utf8_length(inner[1]) == 1)
			cout << format("{},{}={},{}\n",i.first,inner[0],inner[1],inner[2]);
		}
	}

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}




