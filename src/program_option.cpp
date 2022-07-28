#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <thread>

struct ProgramConfig
{
	unsigned threads;
	std::string user;
	std::vector<std::string> messages;

	std::string host;
};


int parseArguments(int argc, char *argv[], ProgramConfig &config)
{
	// declare a group of options that will be allowed only on command line
	boost::program_options::options_description generic_options("Options");
	generic_options.add_options()("version,v", "Show version")
		                         ("help,h", "Show this help message")
		                         ("depth,d", "Debug depth");

	// declare a group of options that will be allowed on command line
	boost::program_options::options_description config_options("Configuration");
	config_options.add_options()
		("threads,t",
		 boost::program_options::value<unsigned int>(&config.threads)->default_value(std::thread::hardware_concurrency()),
		 "Number of threads to use")
		("user,u",
		 boost::program_options::value<std::string>(&config.user),
		 "user name")
		("messages,m",
		 boost::program_options::value<std::vector<std::string>>(&config.messages)->composing(),
		 "message");

	// hidden options, will be allowed on command line, but will not be shown to the user
	boost::program_options::options_description hidden_options("Hidden options");
	hidden_options.add_options()(
		"input,i", // "input" used in positional_options
		boost::program_options::value<std::string>(&config.host),
		"host address");

	// positional option
	boost::program_options::positional_options_description positional_options;
	positional_options.add("input", 1);

	// combine above options for parsing
	boost::program_options::options_description cmdline_options;
	cmdline_options.add(generic_options).add(config_options).add(hidden_options);

	const auto *executable = argv[0];
	boost::program_options::options_description visible_options(
		"Usage: " + boost::filesystem::path(executable).filename().string() +
		" <host address> [options]");
	visible_options.add(generic_options).add(config_options);

	// parse command line options
	boost::program_options::variables_map option_variables;
	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
			.options(cmdline_options)
			.positional(positional_options)
			.run(),
			option_variables);
	}
	catch (const boost::program_options::error &e)
	{
		std::cout << e.what();
		return -1;
	}

	if (option_variables.count("version"))
	{
		std::cout << 1 << std::endl;
		return 1;
	}

	if (option_variables.count("help"))
	{
		std::cout << visible_options;
		return 1;
	}

	boost::program_options::notify(option_variables);

	if (!option_variables.count("input"))
	{
		std::cout << visible_options;
		return 1;
	}

	if (option_variables.count("depth"))
	{
		std::cout << "Debug depth is " << option_variables.count("depth");
	}

	return 0;
}

int main(int argc, char ** argv)
{
	ProgramConfig config;
	const int ret = parseArguments(argc, argv, config);

	if (config.user.size()) {
		std::cout << "user " << config.user;
	}

	if (config.messages.size()) {
		int t = 1;
		for (const auto& message : config.messages) {
			std::cout << "message " << t++ << "=> " << message << '\n';
		}
	}

	return ret;
}