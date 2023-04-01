#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

using namespace std::string_literals;

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	switch (args.size())
	{
	case 1: {
		FILE* fortune_pipe = popen("fortune", "r");

		if (!fortune_pipe)
			throw std::runtime_error("Cannot open pipe!");

		char buff[4096];

		fgets(buff, 4096, fortune_pipe);

		std::cout << buff;

	} break;

		
	case 4: {
		FILE* cowsay_pipe = popen("cowsay", "w");

		if (!cowsay_pipe)
			throw std::runtime_error("Cannot open pipe!");

		fputs(("Address: "s + args[1] + "\n"s).c_str(), cowsay_pipe);
		fputs(("Title: "s + args[2] + "\n"s).c_str(), cowsay_pipe);
		fputs((args[3]).c_str(), cowsay_pipe);

		pclose(cowsay_pipe);
	}break;


	default:
		throw std::runtime_error("0 or 3 arguments expected!");
		break;
	}


	return 0;
}
