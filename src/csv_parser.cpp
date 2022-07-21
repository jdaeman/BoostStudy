#include <boost/iostreams/device/mapped_file.hpp>

#include <iostream>

int main()
{
	try 
	{
		boost::iostreams::mapped_file_source mmap("abc.txt");
		mmap.close();
	}
	catch (...)
	{
		std::cout << "boost error\n";
	}
	
	return 0;
}
