#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 集合a.utf8txt 集合b.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	error_type error = nullptr;
	bool is_ok = false;

	table::table_t a(argv[1],table_category::key_word);
	if (!a.is_ok){throw std::runtime_error(a.error);}

	table::table_t b(argv[2],table_category::key_word);
	if (!b.is_ok){throw std::runtime_error(b.error);}

	epcall(a.word_set_unite(b),error,is_ok);
	if(!is_ok)cerr << error << endl;

	epcall(a.output_table(cout),error,is_ok);
	if(!is_ok)cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}




