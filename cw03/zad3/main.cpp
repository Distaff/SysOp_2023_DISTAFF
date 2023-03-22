#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>
#include <map>
#include <filesystem>
#include <unistd.h> 

using namespace std::string_literals;

void processDir(std::string dir_path, std::string looked_for) {
	// On WSL this prevents recursive iteration through mounted Windows drives (on larger 
	// filesystems like mine that's effectively a fork bomb - ends with freeze or BSOD)
	if (dir_path.substr(0, 5) == "/mnt/")
		exit(0);

	std::filesystem::directory_iterator dir_iter(dir_path);
	for (auto& dir_entry : dir_iter) {
		if (dir_entry.is_directory()) {
			if (!fork()) {
				processDir(dir_entry.path(), looked_for);
				exit(0);
			}
			else continue;
		}
		else {
			std::ifstream processed_file(dir_entry.path());
			std::string buff;
			processed_file >> buff;
			buff = buff.substr(0, looked_for.length());
			if (looked_for == buff) {
				std::cout << "\nFound:\t" << dir_entry.path() << "\tPID:\t" << getpid();
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	if (args.size() != 3)
		throw std::runtime_error("Invaild arguments count!"s);

	processDir(args[1], args[2]);

	return 0;
}