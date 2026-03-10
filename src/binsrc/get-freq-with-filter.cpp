#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 频表.utf8txt 待获频表.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	error_type error = nullptr;
	bool is_ok = false;

	table::table_t from(argv[1],table_category::key_word);
	if (!from.is_ok){throw std::runtime_error(from.error);}

	table::table_t to(argv[2],table_category::key_word);
	if (!to.is_ok){throw std::runtime_error(to.error);}

	epcall(to.get_freq(from),error,is_ok);
	if(!is_ok)cerr << error << endl;

	epcall(to.output_table(cout, table_t::get_freq_filter),error,is_ok);
	if(!is_ok)cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}



