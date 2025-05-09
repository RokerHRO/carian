// generats random text with letters from the Unicode block "Carian"
// (U+102A0 ... U+102D0)

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <boost/program_options.hpp>

namespace po = boost::program_options;


std::string toUtf8(unsigned c)
{
	char s[8] = {0};
	switch(c)
	{
		case 0x000 ... 0x007f : s[0] = char(c); return s;
		case 0x080 ... 0x07ff : s[0] = char( 0xC0 + (c>>6) ); s[1]=char( 0x80 + (c & 63) ); return s;
		case 0x800 ... 0xffff : s[0] = char( 0xE0 + (c>>12)); s[1]=char( 0x80 + ((c>>6)&63)); s[2]=char(0x80 + (c&63)); return s;
		case 0x10000 ... 0x10ffff : s[0]= char( 0xF0 + (c>>18));
		                        s[1]=char( 0x80 + ((c>>12)&63));
		                        s[2]=char( 0x80 + ((c>> 6)&63));
		                        s[3]=char( 0x80 + ( c     &63));
		                        return s;
	}
	throw "Meh!";
}

std::string toUtf8(const std::vector<unsigned>& v)
{
	std::string s;
	for(auto u:v)
	{
		s += toUtf8(u);
	}
	return s;
}


size_t interval_length() { return 0; }

template<class... T>
size_t interval_length(const char32_t* s, T... rest);

template<class... T>
size_t interval_length(unsigned first, unsigned last, T... rest)
{
	return last-first+1 + interval_length(rest...);
}

template<class... T>
size_t interval_length(const char32_t* s, T... rest)
{
	size_t len = 0;
	while(*s)
	{
		++len;
		++s;
	}
	return len + interval_length(rest...);
}


std::vector<unsigned> interval()
{
	return std::vector<unsigned>{};
}

template<class... T>
std::vector<unsigned> interval(const char32_t* s, T... rest);


template<class... T>
std::vector<unsigned> interval(unsigned first, unsigned last, T... rest)
{
//	std::cerr << "Interval(" << std::hex << first << ", " << std::hex << last << " (" << std::dec << sizeof...(rest) << " remaining) \n";
	
	std::vector<unsigned> v;
	v.reserve(interval_length(first, last, rest...));
	for(unsigned u=first; u<=last; ++u)
		v.push_back(u);
	
	if(sizeof...(rest))
	{
		std::vector<unsigned> rv = interval(rest...);
		v.insert(v.end(), rv.begin(), rv.end());
	}
	
	return v;
}


template<class... T>
std::vector<unsigned> interval(const char32_t* s, T... rest)
{
	std::vector<unsigned> v;
	while(*s)
	{
		v.push_back( unsigned(*s) );
		++s;
	}
	
	if(sizeof...(rest))
	{
		std::vector<unsigned> rv = interval(rest...);
		v.insert(v.end(), rv.begin(), rv.end());
	}
	
	return v;
}

class Script
{
public:
	
	template<class... T>
	Script(T... intervals)
	: values{interval(intervals...)}
	, uid{0u, (unsigned)values.size()-1}
	{}
	
	template<class Generator>
	unsigned operator()(Generator& g)
	{
		const unsigned idx = uid(g);
		return values.at(idx);
	}
	
	friend
	std::ostream& operator<<(std::ostream& o, const Script& s);

private:
	std::vector<unsigned> values;
	std::uniform_int_distribution<unsigned> uid;
};

std::ostream& operator<<(std::ostream& o, const Script& s)
{
	return o << "{" << s.values.size() << " values, from " << s.uid.min() << " to " << s.uid.max() << ".}";
}

// dummy:
std::istream& operator>>(std::istream& i, const Script& s)
{
	return i;
}


template<class... T>
po::typed_value<Script>*  Distri(Script* script, T... intervals)
{
	auto d = new po::typed_value<Script>(script);
	d->implicit_value( Script(intervals...) );
	d->zero_tokens();
	return d;
}


int main(int argc, char** argv)
{
	unsigned line_len = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	Script uid('A', 'Z'); // okay, use A..Z if no command line option is given.
	
	po::options_description desc;
	desc.add_options()
		("help,h", "print this help messages")
		("ascii,A",   Distri(&uid, 0x21, 0x7E), "Printable ASCII characters")
		("carian,c",  Distri(&uid, 0x102A0, 0x102D0), "Carian")
		("arrows,a",  Distri(&uid, 0x02190, 0x021ff), "Arrows")
		("elbasan,e", Distri(&uid, 0x10500, 0x10527), "Elbasan (old Albanian)")
		("gothic,g",  Distri(&uid, 0x10330, 0x1034a), "Gothic")
		("permic,p",  Distri(&uid, 0x10350, 0x10375), "Old Permic")
		("shavian,s", Distri(&uid, 0x10450, 0x1047F), "Shavian")
		("orchon,o",  Distri(&uid, 0x10C00, 0x10C48), "Ochon runes")
		("osmanya,O", Distri(&uid, 0x10480, 0x1049D,
		                           0x104A0, 0x104A9), "Osmanya")
		("lisu,l",    Distri(&uid,  0xA4D0,  0xA4F7), "Lisu alphabet")
		("tifinagh,t",Distri(&uid,  0x2D30,  0x2D70), "Tifinagh (Tuareg) alphabet")
		("inuktitut,i", Distri(&uid,  0x1400,  0x167F), "Unified Canadian Aboriginal Syllabics")
		("cypriot"    , Distri(&uid, 0x10800, 0x10805,
		                             0x10808, 0x10808,
		                             0x1080A, 0x10835,
		                             0x10837, 0x10838,
		                             0x1083C, 0x1083C,
		                             0x1083F, 0x1083F ), "Cypriote syllabary")
		("armenian",    Distri(&uid, 0x0531, 0x0556,
		                             0x0560, 0x0588,
		                             0x058D, 0x058F ), "Armenian")
		("hebrew",      Distri(&uid, 0x05D0, 0x05EA ), "Hebrew")
		("nko",         Distri(&uid, 0x07C0, 0x07E7,
		                             0x07F6, 0x07F7,
		                             0x07FE, 0x07FF ), "NKo")
		("ethiopic",    Distri(&uid, 0x1200, 0x1248,
		                             0x124A, 0x124D,
		                             0x1250, 0x1256,
		                             U"\u1258\u12C0",
		                             0x125A, 0x125D,
		                             0x1260, 0x1288,
		                             0x128A, 0x128D,
		                             0x1290, 0x12B0,
		                             0x12B2, 0x12B5,
		                             0x12B8, 0x12BE,
		                             0x12C2, 0x12C5,
		                             0x12C8, 0x12D6,
		                             0x12D8, 0x1310,
		                             0x1312, 0x1315,
		                             0x1318, 0x135A,
		                             0x135D, 0x137C), "Ethiopic")
		("cherokee",    Distri(&uid, 0x13A0, 0x13F5,
		                             0x13F8, 0x13FD), "Cherokee")
		
	;
	
	try{
	po::variables_map vm;
	po::store( po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 0;
	}
	}catch(const po::error& e)
	{
		std::cerr << e.what() << "\n" << desc << "\n";
		return 1;
	}

//	std::cerr << "Script: " << uid << std::endl;

	std::uniform_real_distribution<double> len( 0.0, 1.0 );
	
	for(;;)
	{
		std::cout << toUtf8( uid(gen) );
		++line_len;
		if( len(rd) < 0.3 || (line_len>70 && len(rd) < (0.3+(line_len-70)*0.1) ))
		{
			if( ++line_len > 70 )
			{
				std::cout << std::endl;
				line_len = 0;
			}else{
				std::cout << ' ';
			}
		}
	}
}
