#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/spirit/repository/include/qi_iter_pos.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;
using Iterator = std::string::iterator;


// example) /route/v1/driving/7.416351,43.731205;7.420363,43.736189
struct ParsedURL
{
	std::string service;
	unsigned version;
	std::string profile;
	std::string query;
	std::size_t prefix_length;
};

BOOST_FUSION_ADAPT_STRUCT(ParsedURL,
						 (std::string, service)
	                     (unsigned, version)
	                     (std::string,profile)
	                     (std::string, query))

template <typename Iterator, typename Into>
struct URLParser : qi::grammar<Iterator, Into>
{
	URLParser()
		: URLParser::base_type(start)
	{
		using boost::spirit::repository::qi::iter_pos;

		alpha_numeral = qi::char_("a-zA-Z0-9"); // a~z, A~Z, 0~9
		percent_encoding =
			qi::char_('%') > qi::uint_parser<unsigned char, 16, 2, 2>()[qi::_val = qi::_1];
		// Radix: 16, MinDigits: 2, MaxDigits: 2
		polyline_chars = qi::char_("a-zA-Z0-9_.--[]{}@?|\\~`^") | percent_encoding;
		all_chars = polyline_chars | qi::char_("=,;:&().");

		service = +alpha_numeral;
		version = qi::uint_;
		profile = +alpha_numeral;
		query = +all_chars;

		start = qi::lit('/') > service > qi::lit('/') > qi::lit('v') > version > qi::lit('/') >
			    profile > qi::lit('/') >
			    qi::omit[iter_pos[ph::bind(&ParsedURL::prefix_length, qi::_val) =
			                          qi::_1 - qi::_r1]] > query;

	}

	qi::rule<Iterator, Into> start;

	qi::rule<Iterator, std::string()> service;
	qi::rule<Iterator, unsigned()> version;
	qi::rule<Iterator, std::string()> profile;
	qi::rule<Iterator, std::string()> query;

	qi::rule<Iterator, char()> alpha_numeral;
	qi::rule<Iterator, char()> all_chars;
	qi::rule<Iterator, char()> polyline_chars;
	qi::rule<Iterator, char()> percent_encoding;
};

int main(int argc, char ** argv)
{
	std::string url = "/route/v1/driving/7.416351,43.731205;7.420363,43.736189";
	Iterator iter = url.begin();
	const Iterator end = url.end();

	// second template
	// function
	//	return: ParsedURL
	//	input: Iterator
	URLParser<Iterator, ParsedURL(Iterator)> parser;
	ParsedURL out;

	try
	{
		const auto ok = boost::spirit::qi::parse(iter, end, parser(boost::phoenix::val(iter)), out);

		if (ok && iter == end)
		{
			std::cout << out.service << '\n';
			std::cout << out.profile << '\n';
			std::cout << out.version << '\n';
			std::cout << out.prefix_length << '\n';
			std::cout << out.query << '\n';
		}
		else
		{
			std::cerr << "Unknown" << '\n';
		}
	}
	catch (const qi::expectation_failure<Iterator> &failure)
	{
		// The grammar above using expectation parsers ">" does not automatically increment the
		// iterator to the failing position. Extract the position from the exception ourselves.
		iter = failure.first;
		std::cerr << std::string(iter, end) << '\n';
	}

	return 0;
}


/*struct Pos
{
	int x, y;

	Pos() : x(0), y(0) {}
	Pos(int x, int y) : x(x), y(y) {}
};

struct Parameters
{
	enum types
	{
		First = 0,
		Second = 0x1,
		Third = 0x1 << 1,
		Fourth = 0x1 << 2
	};

	std::vector<Pos> posList;
};

int main(int argc, char ** argv)
{
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " url" << '\n';
		return 1;
	}

	std::string url = argv[1];

	using Iterator = std::string::iterator;
	using Signature = void(Parameters &); // void "anonymous" (Parameters &in)

	namespace ph = boost::phoenix;
	namespace qi = boost::spirit::qi;

	qi::rule<Iterator, Pos()> pos_rule;
	qi::rule<Iterator, Signature> query_rule;
	qi::rule<Iterator, Signature> base_rule;

	pos_rule = (qi::int_ > qi::lit(',') > qi::int_)
		[qi::_val = ph::bind(
			[](int x, int y) {
		        return Pos(x, y); 
	        }, qi::_1, qi::_2) // ?? qi::_0
		];

	query_rule =
		(pos_rule % ';')[ph::bind(&Parameters::posList, qi::_r1) = qi::_1];

	base_rule = query_rule(qi::_r1);


	try
	{
		auto iter = url.begin();
		const auto end = url.end();

		Parameters parameters;
		const auto ok =
			boost::spirit::qi::parse(iter, end, base_rule, parameters);

		if (ok && iter == end)
		{
			return 0;
		}
			
	}
	catch (...)
	{
		std::cerr << "Exception" << '\n';
	}

	return 2;
}*/
