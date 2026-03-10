#include "../cppcore/core.impl.cpp"

using namespace table;

int main(int argc ,char *argv[]) try {
	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 单字表.utf8txt 待编码表.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	error_type error = nullptr;
	bool is_ok = false;

	table::table_t s(argv[1],table_category::key_word);
	if (!s.is_ok){throw std::runtime_error(s.error);}

	table::table_t t(argv[2],table_category::key_word);
	if (!t.is_ok){throw std::runtime_error(t.error);}

	epcall(t.encoder(s),error,is_ok);
	if(!is_ok)cerr << error << endl;

	//epcall(t.output_table(cout,table_t::encoder_filter),error,is_ok);
	epcall(t.output_table(cout),error,is_ok);
	if(is_ok)cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}



