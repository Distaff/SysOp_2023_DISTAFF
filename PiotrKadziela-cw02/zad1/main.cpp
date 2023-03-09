#include "fileData.h"
#include <iostream>
#include <ios>
#include <string>
#include <sstream>
#include <time.h>

using namespace std::string_literals;	//for "foobar"s style std::string literals

int getIntOrThrow(std::istringstream& iss) {
	int res = 0xDEADBEFF;
	iss >> res;
	if (iss.fail()) {
		std::string buff = iss.str();
		throw std::runtime_error("Integer value expected in: "s + buff);
	}
	return res;
}

int main()
{
	fileData* fd = nullptr;
	std::string buff;
	std::string linestr;
	std::istringstream line;
	int param = 0xDEADBEFF;


	timespec realBeginTime;
	timespec userBeginTime;
	timespec systemBeginTime;
	timespec realEndTime;
	timespec userEndTime;
	timespec systemEndTime;

	while (std::cin.peek() != EOF) {
		clock_gettime(CLOCK_REALTIME, &realBeginTime);
		clock_gettime(CLOCK_REALTIME, &userBeginTime);
		clock_gettime(CLOCK_REALTIME, &systemBeginTime);


		std::getline(std::cin, linestr);
		line.str(linestr);
		line.clear();
		line.seekg(0, std::ios::beg);

		line >> buff;

		if (buff == "init") {
			param = getIntOrThrow(line);
			fd = new fileData(param);
			if (line >> buff)
				throw std::runtime_error("Unexpected parameter: "s + buff);
		}
		else if (buff == "count") {
			line >> buff;

			fd->pushFromFile(buff);

			if (line >> buff)
				throw std::runtime_error("Unexpected parameter: "s + buff);
		}
		else if (buff == "show") {
			param = getIntOrThrow(line);

			singleFileData &ref = (*fd)[param];
			std::cout << "\nRecord "s << param << ":\n"
				<< "  lines: "s << ref.lines << '\n'
				<< "  words: "s << ref.words << '\n'
				<< "  chars: "s << ref.chars << '\n';

			if (line >> buff)
				throw std::runtime_error("Unexpected parameter: "s + buff);
		}
		else if (buff == "delete") {
			while (line.peek() != EOF && line.peek() != '\n' && line.peek() != '\r') {
				param = getIntOrThrow(line);
				fd->erase(param);
			}

		}
		else if (buff == "destroy") {
			delete fd;
			if (line >> buff)
				throw std::runtime_error("Unexpected parameter: "s + buff);
		}
		else {
			throw std::runtime_error("Unrecognized command: "s + buff);
		}

		clock_gettime(CLOCK_REALTIME, &realEndTime);
		clock_gettime(CLOCK_REALTIME, &userEndTime);
		clock_gettime(CLOCK_REALTIME, &systemEndTime);

		std::cout << "Operation '" << linestr.erase(linestr.length() - 1) << "' finished in: \n"
			<< (realEndTime.tv_nsec - realBeginTime.tv_nsec) * 0.001 << "us realtime\n  "
			<< (userEndTime.tv_nsec - userBeginTime.tv_nsec) * 0.001 << "us usertime\n  "
			<< (systemEndTime.tv_nsec - systemBeginTime.tv_nsec) * 0.001 << "us systemtime\n\n";
	}
	return 0;
}
