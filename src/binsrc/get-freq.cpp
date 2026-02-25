

#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 频表.utf8txt 待获频表.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	string error;

	table::table_t from(argv[1],table_category::key_word);
	if (!from.error.empty()){throw std::runtime_error(from.error.c_str());}

	table::table_t to(argv[2],table_category::key_word);
	if (!to.error.empty()){throw std::runtime_error(to.error.c_str());}

	epw(to.get_freq(from),error);
	if(!error.empty())cerr << error << endl;

	epw(to.output_table(cout),error);
	if(!error.empty())cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}



