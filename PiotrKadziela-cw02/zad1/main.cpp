#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include <fcntl.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>

constexpr int BUFFER_SIZE = 4096;

using namespace std::string_literals;	//for "foobar"s style std::string literals

std::string md5OfFile(std::string path) {
	int errCode = system(("md5sum "s + path + " > /tmp/md5OfFile").c_str());
	if (errCode)
		throw std::system_error(errCode, std::system_category(), "Error encountered when using md5sum"s);

	std::ifstream md5File("/tmp/md5OfFile");
	if (!md5File) 
		throw std::runtime_error("Error: cannot open md5 temp file."s);
	std::string res;
	md5File >> res;
	return res;
}

void copyFileWithReplacement(char toReplace, char replacement, std::ifstream &inFile, std::ofstream &outFile) {
	std::vector<char> buff(BUFFER_SIZE);
	int bytesRead = 0xDEADBEEF;

	while (inFile) {
		bytesRead = inFile.read(&buff[0], buff.size()).gcount();

		for (auto& i : buff)
			if (i == toReplace) i = replacement;

		outFile.write(&buff[0], bytesRead);
	}
}

void copyFileWithReplacement(char toReplace, char replacement, int inFile, int outFile) {
	std::vector<char> buff(BUFFER_SIZE);
	int bytesRead = 0xDEADBEEF;

	while (bytesRead) {
		bytesRead = read(inFile, buff.data(), buff.size());

		for (auto& i : buff)
			if (i == toReplace) i = replacement;

		write(inFile, buff.data(), bytesRead);
	}
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	int durationMicros = 0xDEADBEEF;
	std::string md5;

	if (args.size() != 4 || args[0].length() != 1 || args[1].length() != 1)
		throw std::runtime_error("Imporper arguments! Expected: char, char, string, string"s);


	std::ifstream std_inFile(args[2]);
	std::ofstream std_outFile(args[3]);
	int posix_inFile = open(args[2].c_str(), O_RDONLY);
	int posix_outFile = open(args[3].c_str(), O_WRONLY | O_TRUNC);

	if (!std_inFile)
		throw std::runtime_error("Error: cannot open fstream of input file."s);
	if (!std_outFile)
		throw std::runtime_error("Error: cannot open fstream of output file."s);
	if (!posix_inFile)
		throw std::runtime_error("Error: cannot open POSIX input file."s);
	if (!posix_outFile)
		throw std::runtime_error("Error: cannot open POSIX output file."s);


	/* STD LIBRARY TESTING */

	std::cout << "\nReplacing chars using std...\n";

	start = std::chrono::high_resolution_clock::now();
	copyFileWithReplacement(args[0][0], args[1][0], std_inFile, std_outFile);
	stop = std::chrono::high_resolution_clock::now();

	durationMicros = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
	std::cout << "Replacing done. \n"
		<< " Elapsed time: " << std::fixed << std::setprecision(3) << durationMicros / 1000.0 << "ms\n";

	md5 = md5OfFile(args[3]);


	/* POSIX SYSTEM CALLS TESTING */

	std::cout << "\nReplacing chars using POSIX system calls...\n";

	start = std::chrono::high_resolution_clock::now();
	copyFileWithReplacement(args[0][0], args[1][0], posix_inFile, posix_outFile);
	stop = std::chrono::high_resolution_clock::now();

	durationMicros = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
	std::cout << "Replacing done. \n"
		<< " Elapsed time: " << std::fixed << std::setprecision(3) << durationMicros / 1000.0 << "ms\n";



	std::cout << ((md5 == md5OfFile(args[3])) ? "\nFiles are identical." : "\nERROR: files are not identical.");

	return 0;
}
