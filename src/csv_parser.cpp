#include <boost/exception/diagnostic_information.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <string>
#include <vector>

struct CSVRow final
{
	std::uint64_t id;
	std::string tag;
	std::double_t weight;
	std::string name;

	CSVRow() : id(0), tag(""), weight(0.), name("") {}

	CSVRow(std::uint64_t id, std::string tag, std::double_t weight, std::string name)
		: id(id), tag(tag), weight(weight), name(name)
	{}
};


BOOST_FUSION_ADAPT_STRUCT(CSVRow,
	(decltype(CSVRow::id), id)
	(decltype(CSVRow::tag), tag)
	(decltype(CSVRow::weight), weight)
	(decltype(CSVRow::name), name))


int main(int argc, char ** argv)
{
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " CSV file path" << '\n';
		return 1;
	}

	const std::string filename = argv[1];

	namespace qi = boost::spirit::qi;
	using Data = CSVRow;
	using Iterator = boost::iostreams::mapped_file_source::iterator;
	using DataRule = boost::spirit::qi::rule<Iterator, Data()>;

	try 
	{
		std::vector<Data> data_list;

		boost::iostreams::mapped_file_source mmap(filename);
		auto first = mmap.begin(), last = mmap.end();
		
		// grammer ref: https://en.wikipedia.org/wiki/Spirit_Parser_Framework
		DataRule rule = qi::ulong_long >> ',' >> *(qi::char_ -',') >> ',' >> qi::double_ >> ',' >> *(qi::char_ - qi::eol);
		const auto ok = qi::parse(first, last, -(rule % qi::eol) >> *qi::eol, data_list);

		if (!ok || first != last)
		{
			auto begin_of_line = first - 1;
			while (begin_of_line >= mmap.begin() && *begin_of_line != '\n')
				--begin_of_line;
			auto line_number = std::count(mmap.begin(), first, '\n') + 1;
			const auto message = boost::format("CSV file %1% malformed on line %2%:\n %3%\n") %
				filename % std::to_string(line_number) %
				std::string(begin_of_line + 1, std::find(first, last, '\n'));

			std::cerr << message << '\n';
		}
		else 
		{
			std::cout << data_list.size() << '\n';
			for (auto value : data_list) {
				std::cout << value.id << " " << value.tag << " " << value.weight << " " << value.name << '\n';
			}
		}

		mmap.close();
	}
	catch (const boost::exception &e)
	{
		const auto message = boost::format("exception in loading %1%:\n %2%") % 
			filename % boost::diagnostic_information(e);

		std::cerr << "Exception " << message << '\n';
	}
	
	return 0;
}
