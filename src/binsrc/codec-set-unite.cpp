
#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 集合a.utf8txt 集合b.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	string error;

	table::table_t a(argv[1],table_category::key_codec);
	if (!a.error.empty()){throw std::runtime_error(a.error.c_str());}

	table::table_t b(argv[2],table_category::key_codec);
	if (!b.error.empty()){throw std::runtime_error(b.error.c_str());}

	epw(a.codec_set_unite(b),error);
	if(!error.empty())cerr << error << endl;

	epw(a.output_table(cout),error);
	if(!error.empty())cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}


