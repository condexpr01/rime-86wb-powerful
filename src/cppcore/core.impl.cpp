#include "core.header.h"
#include <algorithm>

namespace table{

	class table_t{
	public:
		//预期格式：编码,序号=字词,频数
		using table_type = unordered_multimap<string,vector<string>>;

		//表对象的哈希表
		table_type table;

		//表对象的flag
		table_category category = table_category::key_none;

		//构造表对象时的错误信息
		string error;

		using iterator = table_type::iterator;
		using const_iterator = table_type::const_iterator;
		
		//用单字表编码当前表
		inline ep<void> encoder(const table_t &single_word_table){
			return table::encoder(single_word_table,(*this));};

		//从from表获取频数到当前表
		inline ep<void> get_freq(table_t &from){return table::get_freq(from,(*this));};
		
		//当前表变成与with表的交集
		inline ep<void> word_set_intersect(const table_t &with){
			return table::word_set_intersect((*this),with);};

		//当前表变成与with表的并集
		inline ep<void> word_set_unite(const table_t &with){
			return table::word_set_unite((*this),with);};

		//当前表变成与with表的差集
		inline ep<void> word_set_difference(const table_t &with){
			return table::word_set_difference((*this),with);};

		inline ep<void> codec_set_intersect(const table_t &with){
			return table::codec_set_intersect((*this),with);};

		inline ep<void> codec_set_unite(const table_t &with){
			return table::codec_set_unite((*this),with);};

		inline ep<void> codec_set_difference(const table_t &with){
			return table::codec_set_difference((*this),with);};


		//默认输出不过滤器(返回true:输出这项，返回false不输出这项)
		static bool default_filter(const pair<string,vector<string>> p){return true;}

		//在编码后，要求不输出编码失败的项
		static bool encoder_filter(const pair<string,vector<string>> p){
			if (p.second[0].empty())return false;
			return true;}

		//在获频后，要求不输出无频的项
		static bool get_freq_filter(const pair<string,vector<string>> p){
			if (p.second[2].empty())return false;
			return true;}


		//输出表对象到输出流
		template <typename T = bool(*)(pair<string,vector<string>>)>
		requires requires(T f,pair<string,vector<string>> p){{f(p)}->same_as<bool>;}
		ep<void> output_table(ostream &out, T fileter = table_t::default_filter);
		

		//从输入流构造表对象, cat为表flag(返回必正常,无unep)
		ep<void> make_table(ifstream& ifs, table_category cat = table_category::key_codec);

		//从文件构造表对象, cat为表flag
		table_t(const string &filename, table_category cat = table_category::key_codec){

			ifstream ifs{filename};

			if (ifs.is_open()){
				epw((*this).make_table(ifs,cat), error);
			}else{
				error =  "Failed to open the file.";
			}
		}

		//从ep<ifstream>构造表对象, cat为表flag
		template<typename EP>
		requires same_as<remove_cvref_t<EP>, ep<ifstream>>
		table_t(EP &&ep_ifs, table_category cat = table_category::key_codec){
			ifstream ifs;
			epw(std::move(ep_ifs),error,ifs);

			if (error.empty()){epw((*this).make_table(ifs,cat),error);}
		}


		//相同的key的list上迭代, 用给range for
		struct key_list{
			//iterator.first  string
			//iterator.second vector<string>
			pair<iterator,iterator> range;//从equal_range构造

			iterator begin() noexcept{return range.first;}
			iterator end()   noexcept{return range.second;}
			const_iterator begin()const noexcept{return range.first;}
			const_iterator end()  const noexcept{return range.second;}
		};

		key_list find_list(const string &key)
			{return key_list{table.equal_range(key)};}
	

		~table_t() = default;
		table_t() = default;

		void operator=(table_t &t){
			(*this).table = t.table;
			(*this).category = t.category;
		}

		void operator=(table_t &&t){
			(*this).table = std::move(t.table);
			(*this).category = t.category;
		}

		table_t(table_t &t){
			(*this).table = t.table;
			(*this).category = t.category;
		}

		table_t(table_t &&t){
			(*this).table = std::move(t.table);
			(*this).category = t.category;
		}


		iterator begin() noexcept{return table.begin();}
		iterator end()   noexcept{return table.end();}

		const_iterator begin() const noexcept{return table.begin();}
		const_iterator end()   const noexcept{return table.end();}

		const_iterator cbegin() const noexcept{return table.cbegin();}
		const_iterator cend()   const noexcept{return table.cend();}

		pair<iterator,iterator>             equal_range(const string &key)      {return table.equal_range(key);}
		pair<const_iterator,const_iterator> equal_range(const string &key) const{return table.equal_range(key);}

		iterator find(const string &key){return table.find(key);}
		const_iterator find(const string &key) const{return table.find(key);}

		iterator erase(iterator i){return table.erase(i);}
		iterator erase(const_iterator i) {return table.erase(i);}

		size_t size() const {return table.size();};
	};

	inline ep<void> dir_layout(ostream& output,const string &s_path) {
		std::filesystem::path path{s_path};

		if(std::filesystem::is_directory(path)){

			for(auto &&entry : directory_iterator(path)){
				std::string path = entry.path().string();

				if(std::filesystem::is_directory(entry.path())){
					output << "Dir : " << path << endl;
					return dir_layout(output, path);

				}else{
					output << "File: " << path << endl;
				}
			}

		}

		return {};
	}


	inline ep<ifstream> detect_file_from_args(const int& argc ,const char * const* const &argv){

		if (argc == 2){
			ifstream file{argv[1]};
			
			if (file.is_open()){
				return ep<ifstream>{std::move(file)};
			}else{
				return unep{"Failed to open the file."};
			}

		}else{
			return unep{"Too many or too few arguments."};
		}
	}
	

	ep<void> table_t::make_table(ifstream& ifs,const table_category cat){
		ep<void> e{};

		//cat为key_codec,期望键存在编码,对应位置:[编码],序号=字词,频数
		//cat为key_word ,期望键存在字词,对应位置:编码,序号=[字词],频数
		(*this).category = cat;
		
		string line;
		string a,b,c,d;

		const size_t startpos = 0;

		size_t a_endpos;
		size_t b_endpos;
		size_t c_endpos;
		[[maybe_unused]] size_t d_endpos;

		bool a_exist;
		bool b_exist;
		bool c_exist;
		[[maybe_unused]]bool d_exist;
		
		while(true){

			if(getline(ifs,line,'\n')){

				//降噪去0x0d
				if(line.empty()) continue;
				if(line.back() == '\r')line.pop_back();

				//降噪去空白
				line.erase(std::remove_if(line.begin(),line.end(),
						[](char c){return std::isspace(c);})
					,line.end());

				//opt pattern: "编码或字词,"
				a_endpos = line.find(',',startpos);
				a_exist = (a_endpos != string::npos);

				//opt pattern: "序号="
				b_endpos = line.find('=',a_endpos + a_exist);
				b_exist = (b_endpos != string::npos);

				//opt pattern: "字词或编码,"
				c_endpos = line.find(',',b_endpos + b_exist);
				c_exist = (c_endpos != string::npos);

				//opt pattern: "频数"
				//c_endpos到string::npos

				a = string_slice(line,startpos          ,a_endpos);
				b = string_slice(line,a_endpos + a_exist,b_endpos);
				c = string_slice(line,b_endpos + b_exist,c_endpos);
				d = string_slice(line,c_endpos + c_exist,string::npos);

			}else if(!ifs.eof()){
				e = unep{"Error: Didn't get to EOF."};
				break;

			}else{break;}


			//过滤无键名的格式,确保有键名
			//key_codec下a为key 具体位置: "[编码],序号=字词,频数"
			//key_word 下c为key 具体位置: "编码,序号=[字词],频数"
			
			//key_codec
			if(cat == table_category::key_codec) do{
				if(a.empty()){
					cerr << "line: " << string_visiable(line)
						<< " [fmt need]: " << "[编码],序号=字词,频数" << endl;
					break;}

				(*this).table.emplace(a,vector<string>{b,c,d});

			}while(false);
			
			//key_word
			if(cat == table_category::key_word) do{
				if(c.empty()){
					cerr << "line: " << string_visiable(line) 
						<< " [fmt need]: " << "编码,序号=[字词],频数" << endl;
					break;}

				(*this).table.emplace(c,vector<string>{a,b,d});

			}while(false);

		}
		
		return e;
	}

	ep<void> make_vector_table(ifstream& ifs,
		vector<pair<string,vector<string>>>& v,
						const table_category cat){

		ep<void> e{};
		v.clear();

		//cat为key_codec,期望键存在编码,对应位置:[编码],序号=字词,频数
		//cat为key_word ,期望键存在字词,对应位置:编码,序号=[字词],频数

		string line;
		string a,b,c,d;

		const size_t startpos = 0;

		size_t a_endpos;
		size_t b_endpos;
		size_t c_endpos;
		[[maybe_unused]] size_t d_endpos;

		bool a_exist;
		bool b_exist;
		bool c_exist;
		[[maybe_unused]]bool d_exist;

		while(true){

			if(getline(ifs,line,'\n')){

				//降噪去0x0d
				if(line.empty()) continue;
				if(line.back() == '\r')line.pop_back();

				//降噪去空白
				line.erase(std::remove_if(line.begin(),line.end(),
								 [](char c){return std::isspace(c);})
				  ,line.end());

				//opt pattern: "编码或字词,"
				a_endpos = line.find(',',startpos);
				a_exist = (a_endpos != string::npos);

				//opt pattern: "序号="
				b_endpos = line.find('=',a_endpos + a_exist);
				b_exist = (b_endpos != string::npos);

				//opt pattern: "字词或编码,"
				c_endpos = line.find(',',b_endpos + b_exist);
				c_exist = (c_endpos != string::npos);

				//opt pattern: "频数"
				//c_endpos到string::npos

				a = string_slice(line,startpos          ,a_endpos);
				b = string_slice(line,a_endpos + a_exist,b_endpos);
				c = string_slice(line,b_endpos + b_exist,c_endpos);
				d = string_slice(line,c_endpos + c_exist,string::npos);

			}else if(!ifs.eof()){
				e = unep{"Error: Didn't get to EOF."};
				break;

			}else{break;}


			//过滤无键名的格式,确保有键名
			//key_codec下a为key 具体位置: "[编码],序号=字词,频数"
			//key_word 下c为key 具体位置: "编码,序号=[字词],频数"

			//key_codec
			if(cat == table_category::key_codec) do{
				if(a.empty()){
					cerr << "line: " << string_visiable(line)
						<< " [fmt need]: " << "[编码],序号=字词,频数" << endl;
					break;}

				v.emplace_back(a,vector<string>{b,c,d});

			}while(false);

			//key_word
			if(cat == table_category::key_word) do{
				if(c.empty()){
					cerr << "line: " << string_visiable(line) 
						<< " [fmt need]: " << "编码,序号=[字词],频数" << endl;
					break;}

				v.emplace_back(c,vector<string>{a,b,d});

			}while(false);

		}

		return e;
	}

	template <typename T>
	requires requires(T f,pair<string,vector<string>> p){{f(p)}->same_as<bool>;}
	ep<void> table_t::output_table(ostream &out, T fileter){

		if((*this).category == table_category::key_codec){
			for (auto &&v : (*this).table){
				if(!fileter(v))continue;

				//fmt:字词或编码[键],序号0=编码或字词1,频数2
				out << format("{},{}={},{}\n",
					 string_visiable(v.first),
					 string_visiable(v.second[0]),
					 string_visiable(v.second[1]),
					 string_visiable(v.second[2])
					 );

				out.flush();
			}}

		if((*this).category == table_category::key_word){
			for (auto &&v : (*this).table){
				if(!fileter(v))continue;

				out << format("{},{}={},{}\n",
					 string_visiable(v.second[0]),
					 string_visiable(v.second[1]),
					 string_visiable(v.first),
					 string_visiable(v.second[2])
					 );

				out.flush();
			}}

		return {};
	}

	inline size_t utf8_length(const string &s) noexcept{
		size_t count = 0;

		//统计非0b10xxxxxx的字符
		for(auto &&v : s){if ((v & 0b11000000) != 0b10000000){count++;}}
		return count;
	}

	inline size_t utf8_word_locate(const string &s,const size_t begin) noexcept{
		//1byte:0xxxxxxxx
		//2byte:110xxxxx 10xxxxxx
		//3byte:1110xxxx 10xxxxxx 10xxxxxx
		//4byte:11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		if (begin >= s.size()){return 0;}

		//闭开[begin,end)
		size_t end= string::npos;
		
		do{
			if ((s[begin] & 0b10000000) == 0b00000000){end = begin+1;break;}
			if ((s[begin] & 0b11100000) == 0b11000000){end = begin+2;break;}
			if ((s[begin] & 0b11110000) == 0b11100000){end = begin+3;break;}
			if ((s[begin] & 0b11111000) == 0b11110000){end = begin+4;break;}
			
			end = 0;
		}while(false);


		return end;
	}
	
	string string_visiable(const string &s){

		string result;size_t rpos=0;
		result.resize(s.size());

		for (auto && v : s){
			do{
				if (0x00 <= v && v< 0x20){result[rpos++]='?';break;}
				if (v == 0x7f)           {result[rpos++]='?';break;}

				result[rpos++]=v;
			}while(false);
		}

		return result;
	}

	
	//将sep_set中的字符作为分隔符，切片s到vector中
	ep<void> string_sep_vector(const string & s, const string & sep_set, vector<string> &v){

		if (s.empty() || sep_set.empty()){
			return unep{"Error[string_sep_vector]: string empty"};
		}

		if(!v.empty())v.clear();

		array<bool,256> bitmap{};
		for(unsigned char c: sep_set){bitmap[c]=true;}
		
		//[begin,end)
		for (size_t begin = 0, end = 0; true; begin = end){
			while (bitmap[static_cast<unsigned char>(s[begin])]){
				if (begin < s.size())begin++;
				else return{};
			}

			end = begin;

			while (!bitmap[static_cast<unsigned char>(s[end])]){
				if (end < s.size()){end++;}
				else{

					// slice
					v.emplace_back(string_slice(s,begin,end));
					return{};
				}
			};

			// slice
			v.emplace_back(string_slice(s,begin,end));
		}

		return {};
	}


	//从single_word_table编码target
	ep<void> encoder(const table_t &single_word_table, table_t &target){
		//                             0    1   first 2
		//target           被预期fmt: null,null=字词,null
		//single_word_table被预期fmt: 编码,null=单字,null
		//
		//target         将被做成fmt: 编码,null=字词,null
		//
		//caution: null表示不在乎是什么,不做改变

		if (single_word_table.category != table_category::key_word
					|| target.category != table_category::key_word){
			return unep{"Error(encoder): The category should be both table_category::key_word."};
		}

		string u8w1,u8w2,u8w3,u8w4;
		size_t u8wp1,u8wp2,u8wp3,u8wp4;
		
		for(auto &t : target){

			//编码first -> second[0]
			switch (utf8_length(t.first)){
				case 0:break;

				case 1:{
					u8wp1 = utf8_word_locate(t.first,0);
					u8w1=string_slice(t.first,0,u8wp1);

					auto swte1 = single_word_table.find(u8w1);
					if (swte1== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;}
					
					if((*swte1).second[0].empty()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;
					}

					//t.second[0] = format("{:s}",(*swte1).second[0]);
					t.second[0] = (*swte1).second[0];

					break;
				}

				case 2:{
					u8wp1 = utf8_word_locate(t.first,0);
					u8w1=string_slice(t.first,0,u8wp1);
					u8wp2 = utf8_word_locate(t.first,u8wp1);
					u8w2=string_slice(t.first,u8wp1,u8wp2);

					auto swte1 = single_word_table.find(u8w1);
					if (swte1== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;}

					auto swte2= single_word_table.find(u8w2);
					if (swte2== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w2) << endl;
						break;}

					if((*swte1).second[0].size() >=2 && (*swte2).second[0].size() >=2){
						t.second[0] = format("{:1c}{:1c}{:1c}{:1c}",(*swte1).second[0][0],(*swte1).second[0][1],
																	(*swte2).second[0][0],(*swte2).second[0][1]);
					}else{
						cerr << "[encoder]may need to check this: " << string_visiable(u8w1)
							<< ',' << string_visiable((*swte1).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w2)
							<< ',' << string_visiable((*swte2).second[0]) << endl;
						break;
					}

					break;
				}

				case 3:{
					u8wp1 = utf8_word_locate(t.first,0);
					u8w1=string_slice(t.first,0,u8wp1);
					u8wp2 = utf8_word_locate(t.first,u8wp1);
					u8w2=string_slice(t.first,u8wp1,u8wp2);
					u8wp3 = utf8_word_locate(t.first,u8wp2);
					u8w3=string_slice(t.first,u8wp2,u8wp3);

					auto swte1 = single_word_table.find(u8w1);
					if (swte1== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;}

					auto swte2= single_word_table.find(u8w2);
					if (swte2== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w2) << endl;
						break;}

					auto swte3= single_word_table.find(u8w3);
					if (swte3== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w3) << endl;
						break;}


					if((*swte1).second[0].size() >=1
					&& (*swte2).second[0].size() >=1
					&& (*swte3).second[0].size() >=2){
						t.second[0] = format("{:1c}{:1c}{:1c}{:1c}",(*swte1).second[0][0],(*swte2).second[0][0],
																	(*swte3).second[0][0],(*swte3).second[0][1]);
					}else{
						cerr << "[encoder]may need to check this: " << string_visiable(u8w1)
							<< ',' << string_visiable((*swte1).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w2)
							<< ',' << string_visiable((*swte2).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w3)
							<< ',' << string_visiable((*swte3).second[0]) << endl;
						break;
					}

					break;
				}
				case 4:{
					u8wp1 = utf8_word_locate(t.first,0);
					u8w1=string_slice(t.first,0,u8wp1);
					u8wp2 = utf8_word_locate(t.first,u8wp1);
					u8w2=string_slice(t.first,u8wp1,u8wp2);
					u8wp3 = utf8_word_locate(t.first,u8wp2);
					u8w3=string_slice(t.first,u8wp2,u8wp3);
					u8wp4 = utf8_word_locate(t.first,u8wp3);
					u8w4=string_slice(t.first,u8wp3,u8wp4);

					auto swte1 = single_word_table.find(u8w1);
					if (swte1== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;}

					auto swte2= single_word_table.find(u8w2);
					if (swte2== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w2) << endl;
						break;}

					auto swte3= single_word_table.find(u8w3);
					if (swte3== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w3) << endl;
						break;}

					auto swte4= single_word_table.find(u8w4);
					if (swte4== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w4) << endl;
						break;}

					if((*swte1).second[0].size() >=1
					&& (*swte2).second[0].size() >=1
					&& (*swte3).second[0].size() >=1
					&& (*swte4).second[0].size() >=1){
						t.second[0] = format("{:1c}{:1c}{:1c}{:1c}",(*swte1).second[0][0],(*swte2).second[0][0],
																	(*swte3).second[0][0],(*swte4).second[0][0]);
					}else{

						cerr << "[encoder]may need to check this: " << string_visiable(u8w1)
							<< ',' << string_visiable((*swte1).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w2)
							<< ',' << string_visiable((*swte2).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w3)
							<< ',' << string_visiable((*swte3).second[0]) << endl;
						cerr << "[encoder]may need to check this: " << string_visiable(u8w4)
							<< ',' << string_visiable((*swte4).second[0]) << endl;
						break;
					}

					break;
				}


				//greater than 4
				default:{
					u8wp1 = utf8_word_locate(t.first,0);
					u8w1=string_slice(t.first,0,u8wp1);
					u8wp2 = utf8_word_locate(t.first,u8wp1);
					u8w2=string_slice(t.first,u8wp1,u8wp2);
					u8wp3 = utf8_word_locate(t.first,u8wp2);
					u8w3=string_slice(t.first,u8wp2,u8wp3);
					
					for (size_t p=u8wp3,p4p=p;p!=0;p=utf8_word_locate(t.first,p)){
						u8wp4=p4p;
						p4p=p;}

					u8w4=string_slice(t.first,u8wp4,string::npos);

					auto swte1 = single_word_table.find(u8w1);
					if (swte1== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w1) << endl;
						break;}

					auto swte2= single_word_table.find(u8w2);
					if (swte2== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w2) << endl;
						break;}

					auto swte3= single_word_table.find(u8w3);
					if (swte3== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w3) << endl;
						break;}

					auto swte4= single_word_table.find(u8w4);
					if (swte4== single_word_table.end()){
						cerr << "[encoder]missing: " << string_visiable(u8w4) << endl;
						break;}

					if((*swte1).second[0].size() >=1
					&& (*swte2).second[0].size() >=1
					&& (*swte3).second[0].size() >=1
					&& (*swte4).second[0].size() >=1){
						t.second[0] = format("{:1c}{:1c}{:1c}{:1c}",(*swte1).second[0][0],(*swte2).second[0][0],
																	(*swte3).second[0][0],(*swte4).second[0][0]);
					}else{
						cerr << "[encoder]may not enough: " << string_visiable(u8w1) << endl;
						cerr << "[encoder]may not enough: " << string_visiable(u8w2) << endl;
						cerr << "[encoder]may not enough: " << string_visiable(u8w3) << endl;
						cerr << "[encoder]may not enough: " << string_visiable(u8w4) << endl;
						break;
					}

					break;
				}

			}
		}

		return {};
	}

	ep<void> get_freq(const table_t &from, table_t &to){

		//                       0    1   first 2
		//from        被预期fmt: null,null=字词,频数
		//to          被预期fmt: null,null=字词,null
		//
		//to变成      被变成fmt: null,null=字词,频数
		//
		//caution: null表示不在乎是什么,不做改变

		if (from.category != table_category::key_word
			|| to.category != table_category::key_word){
			return unep{"Error(encoder): The category should be both table_category::key_word."};
		}

		for(auto &t : to){
			auto f = from.find(t.first);

			if (f == from.end()){
				cerr << "[get_freq]missing: " << string_visiable(t.first) << endl;
				continue;}

			if ((*f).second[2].empty()){
				cerr << "[get_freq]empty: " << string_visiable(t.first) << endl;
				continue;}

			t.second[2] = (*f).second[2];
		}

		return {};
	}


	ep<std::string> string_add(const std::string& num1, const std::string& num2) {

		if (num1.empty() || num2.empty()) return unep{"Error[string_add]: empty"};
		
		size_t base = 10;
		size_t i1 = num1.size();
		size_t i2 = num2.size();
		size_t max_size = std::max(i1,i2);
		size_t buf_size = max_size+1;
		
		string number_buf(buf_size,'0');
		string carry_buf (buf_size,'0');
		number_buf.replace(buf_size-i1,i1,num1);//number_buf[0]='0'
		carry_buf.replace (buf_size-i2,i2,num2);

		size_t ci,ni,has_carry; do{
			ci=0,ni=1,has_carry=0;

			for(char c;ni < buf_size;ni++,ci++){
				c= number_buf[ni] + carry_buf[ni] - 2*'0';

				number_buf[ni] = c%base + '0';

				carry_buf[ci]  = c/base + '0';

				if(carry_buf[ci] != '0')has_carry++;
			}

			carry_buf[ci] = '0';

			if(carry_buf[0]=='1'){
				number_buf[0]='1';
				carry_buf[0] ='0';

				if(has_carry > 0)has_carry--;
			}//不要写else去把number_buf设为'0'
			
			#if 0
			cout << format("number_buf: {}\n",number_buf);
			cout << format("carry_buf : {}\n",carry_buf);
			#endif

		}while(has_carry > 0);
		
		if(number_buf[0]=='0'){

			size_t pos = number_buf.find_first_not_of('0');

			if (pos == std::string::npos) {
				number_buf = "0";// 全是0
			} else {
				number_buf.erase(0, pos);
			}
		}

		return number_buf;
	}
	
	//频数的特化减法，num1-num2，但当num1<num2会返回0(也就是不出现负频数),
	ep<std::string> string_sub(const std::string& num1, const std::string& num2) {

		if (num1.empty() || num2.empty()) return unep{"Error[string_sub]: empty"};
		
		size_t base = 10;
		size_t i1 = num1.size();
		size_t i2 = num2.size();
		size_t max_size = std::max(i1,i2);
		size_t buf_size = max_size+1;
		
		string number_buf(buf_size,'0');
		string carry_buf (buf_size,'0');
		number_buf.replace(buf_size-i1,i1,num1);
		carry_buf.replace (buf_size-i2,i2,num2);

		size_t ci,ni;bool has_carry; do{
			ci=0,ni=1,has_carry=false;

			for(char c;ni < buf_size;ni++,ci++){

				carry_buf[ci]  = number_buf[ni] >= carry_buf[ni] ? '0' : '1';

				if(carry_buf[ci]=='0'){
					c = number_buf[ni]        - carry_buf[ni] + '0';
				}else{
					has_carry=true;
					c = number_buf[ni] + base - carry_buf[ni] + '0';
				}

				number_buf[ni] = c;
			}

			carry_buf[ci] = '0';

			#if 0
			cout << format("number_buf: {}\n",number_buf);
			cout << format("carry_buf : {}\n",carry_buf);
			#endif

			if(carry_buf[0]=='1'){
				//不希望出现负数，不够减就设为0（并非unexception)
				return "0";
			}
			

		}while(has_carry);

		if(number_buf[0]=='0'){
			size_t pos = number_buf.find_first_not_of('0');

			if (pos == std::string::npos) {
				number_buf = "0";// 全是0
			} else {
				number_buf.erase(0, pos);
			}
		}

		return number_buf;
	}

	//intersection和with取差集到intersetion
	ep<void> word_set_difference(table_t &difference,const table_t &with){

		//                       0    1   first 2
		//difference   被预期fmt: null,null=字词,null
		//with         被预期fmt: null,null=字词,null
		//
		//difference变成不存在于with,但在diff里存在的子集,或叫两者差集
		//
		//caution: null表示不在乎是什么,不做改变

		if (difference.category != table_category::key_word
			|| with.category != table_category::key_word){
			return unep{"Error(intersect): The category should be both table_category::key_word."};
		}

		string warn{},value{};

		for(auto &&i = difference.begin(); i != difference.end();){
			auto w = with.find((*i).first);

			if (w == with.end()){
				i++;

			}else{
				i=difference.erase(i);

			}

		}

		return {};
	}


	//intersection和with取交集到intersetion
	ep<void> word_set_intersect(table_t &intersection,const table_t &with){

		//                       0    1   first 2
		//intersection 被预期fmt: null,null=字词,null
		//with         被预期fmt: null,null=字词,null
		//
		//intersection变成存在于with中的子集,或叫两者交集
		//
		//caution: null表示不在乎是什么,不做改变

		if (intersection.category != table_category::key_word
			|| with.category != table_category::key_word){
			return unep{"Error(intersect): The category should be both table_category::key_word."};
		}

		string warn{},value{};

		for(auto &&i = intersection.begin(); i != intersection.end();){
			auto w = with.find((*i).first);

			if (w == with.end()){
				i=intersection.erase(i);

			}else{
				do{
					if((*w).second[2].empty())break;
					
					if((*i).second[2].empty()){
						(*i).second[2]=(*w).second[2];
						break;
					}

					epw(string_add((*i).second[2],(*w).second[2]),warn,value);
					if(!warn.empty()){
						cerr << format("Warning(unite): {}\n",warn);
						warn.clear();
						value.clear();
						break;
					}else{(*i).second[2] = value;break;}

				}while(false);

				//下一个
				i++;
			}

		}

		return {};
	}


	//intersection和with取交集到intersetion
	ep<void> word_set_unite(table_t &union_set,const table_t &with){

		//                       0    1   first 2
		//union_set    被预期fmt: null,null=字词,[频率opt]
		//with         被预期fmt: null,null=字词,[频率opt]
		//
		//union_set变成包含with的超集,或叫两者的并集,频率相加
		//
		//caution: null表示不在乎是什么,不做改变

		if (union_set.category != table_category::key_word
			|| with.category != table_category::key_word){
			return unep{"Error(unite): The category should be both table_category::key_word."};
		}

		string warn{},value{};

		for(auto &&w : with){
			auto u = union_set.find(w.first);

			if (u == union_set.end()){
				union_set.table.emplace(w.first,w.second);

			}else{
				
				//加法器
				do{
					if(w.second[2].empty())break;
					
					if((*u).second[2].empty()){
						(*u).second[2]=w.second[2];
						break;
					}

					epw(string_add(w.second[2],(*u).second[2]),warn,value);
					if(!warn.empty()){
						cerr << format("Warning(unite): {}\n",warn);
						warn.clear();
						value.clear();
						break;
					}else{(*u).second[2] = value;break;}

				}while(false);
			}

		}

		return {};
	}

	//intersection和with取交集到intersetion
	//此为合并codec版,无加法器
	ep<void> codec_set_unite(table_t &union_set,const table_t &with){

		//						  first  0  1     2
		//union_set    被预期fmt: 编码,null=null,null
		//with         被预期fmt: 编码,null=null,null
		//
		//union_set变成包含with的超集,或叫两者的并集,频率相加
		//
		//caution: null表示不在乎是什么,不做改变

		if (union_set.category != table_category::key_codec
			|| with.category != table_category::key_codec){
			return unep{"Error(unite): The category should be both table_category::key_codec."};
		}

		for(auto &&w : with){
			auto u = union_set.find(w.first);

			if (u == union_set.end()){
				union_set.table.emplace(w.first,w.second);
			}

		}

		return {};
	}


	//intersection和with取交集到intersetion
	//此为相交codec版,无加法器
	ep<void> codec_set_intersect(table_t &intersection,const table_t &with){

		//						  first  0  1     2
		//union_set    被预期fmt: 编码,null=null,null
		//with         被预期fmt: 编码,null=null,null
		//
		//intersection变成存在于with中的子集,或叫两者交集
		//
		//caution: null表示不在乎是什么,不做改变

		if (intersection.category != table_category::key_codec
			|| with.category != table_category::key_codec){
			return unep{"Error(unite): The category should be both table_category::key_codec."};
		}

		for(auto &&i = intersection.begin(); i != intersection.end();){
			auto w = with.find((*i).first);

			if (w == with.end()){
				i=intersection.erase(i);

			}else{
				//下一个
				i++;
			}

		}

		return {};
	}

	//intersection和with取差集到intersetion
	ep<void> codec_set_difference(table_t &difference,const table_t &with){

		if (difference.category != table_category::key_codec
			|| with.category != table_category::key_codec){
			return unep{"Error(intersect): The category should be both table_category::key_codec."};
		}

		string warn{},value{};

		for(auto &&i = difference.begin(); i != difference.end();){
			auto w = with.find((*i).first);

			if (w == with.end()){
				i++;

			}else{
				i=difference.erase(i);

			}

		}

		return {};
	}

}






