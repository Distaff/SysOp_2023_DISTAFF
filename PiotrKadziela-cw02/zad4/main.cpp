#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <map>
#include <filesystem>


using namespace std::string_literals;

std::map<std::string, int> mapFileSizes(std::string dirPath) {
	std::map<std::string, int> res;
	std::filesystem::recursive_directory_iterator dirIter(dirPath);	//ftw()

	for (auto& dirEntry : dirIter) {	//readdir()
		if(!dirEntry.is_directory())	//!S_ISDIR(bufor_stat.st_mode)
			res[dirEntry.path()] = std::filesystem::file_size(dirEntry.path());
	}

	return res;
}

int main(int argc, char** argv)
{
	std::string pathname;

	if (argc == 1)
		pathname = "./";
	else if (argc == 2)
		pathname = std::string(argv[1]);
	else
		throw std::runtime_error("Imporper arguments count!"s);


	std::cout << "Listing files in directory " << pathname << ":\n"
		<< std::setw(96) << std::left << "FILENAME:" << "SIZE (BYTES):\n";


	for (auto& i : mapFileSizes(pathname))
		std::cout << std::setw(96) << std::left << i.first + ' ' << i.second << '\n';

	return 0;
}
