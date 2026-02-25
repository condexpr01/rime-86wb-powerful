//cpp test
//测试程序

#include "../cppcore/core.impl.cpp"

using namespace table;

int main(){

	string error,value;
	epw(string_add("2","8"),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}

	epw(string_add("1234567890","1"),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}

	epw(string_sub("12345","3456"),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}

	epw(string_sub("1","123456789"),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}

	epw(string_add("",""),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}

	epw(string_sub("",""),error,value);
	if (error.empty()){cout << value << endl;}
	else{cout << error << endl;}


	return 0;
}
