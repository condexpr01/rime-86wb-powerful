#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {

	error_type error = nullptr;
	bool is_ok = false;

	vector<pair<string,vector<string>>> table;
	
	ifstream ifs;

	epcall(detect_file_from_args(argc,argv),ifs,error,is_ok);
	if(!is_ok){throw std::runtime_error{error};}

	epcall(make_vector_table(ifs,table,table_category::key_word),error,is_ok);
	if(!is_ok){throw std::runtime_error{error};}

	vector<pair<string,vector<string>>> buffer(table.begin(),table.end());
	sort(buffer.begin(),buffer.end(),[](const auto &a,const auto & b){
		const string worda=a.first;
		const string wordb=b.first;

		//长词在前
		if(worda.length() != wordb.length()) return worda.length() > wordb.length();
		else return a.second[0] < b.second[0];
	});

	for(auto &&v : buffer){
		cout << format("{},{}={},{}\n",v.second[0],v.second[1],v.first,v.second[2]);
	}

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}


