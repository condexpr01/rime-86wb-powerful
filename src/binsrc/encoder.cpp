
#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 单字表.utf8txt 待编码表.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	string error;

	table::table_t s(argv[1],table_category::key_word);
	if (!s.error.empty()){throw std::runtime_error(s.error.c_str());}

	table::table_t t(argv[2],table_category::key_word);
	if (!t.error.empty()){throw std::runtime_error(t.error.c_str());}

	epw(t.encoder(s),error);
	if(!error.empty())cerr << error << endl;

	//epw(t.output_table(cout,table_t::encoder_filter),error);
	epw(t.output_table(cout),error);
	if(!error.empty())cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}



