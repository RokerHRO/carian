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

typedef std::uniform_int_distribution<unsigned> UID;

po::typed_value<UID>*  Distri(UID* uid, unsigned first, unsigned last)
{
	auto d = new po::typed_value<UID>(uid);
	d->implicit_value( UID(first,last) );
	d->zero_tokens();
	return d;
}


int main(int argc, char** argv)
{
	unsigned line_len = 0;
	UID uid('A', 'Z'); // okay, use A..Z if no command line option is given.
	std::random_device rd;
	
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
		("osmanya,O", Distri(&uid, 0x10480, 0x104A9), "Osmanya")  // FIXME: 1049E, 1049F are not defined!
		("lisu,l",    Distri(&uid,  0xA4D0,  0xA4F7), "Lisu alphabet")
		("tifinagh,t",Distri(&uid,  0x2D30,  0x2D70), "Tifinagh (Tuareg) alphabet")
		("inuktitut,i", Distri(&uid,  0x1400,  0x167F), "Unified Canadian Aboriginal Syllabics")
		("cypriot"    , Distri(&uid, 0x10800, 0x1083F), "Cypriote syllabary") // FIXME: there are gaps in the codeblock!
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

	std::uniform_real_distribution<double> len( 0.0, 1.0 );
	
	for(;;)
	{
		std::cout << toUtf8( uid(rd) );
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
