#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>

constexpr int BUFFER_SIZE = 1024;

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

void copyBackwardsBuffer(std::ifstream &inFile, std::ofstream &outFile) {
	std::vector<char> buff(BUFFER_SIZE);
	int totalBytesRead = 0;
	int bytesRead = 0xDEADBEEF;
	
	inFile.seekg(0, std::ios::end);
	int fileLen = inFile.tellg();

	bool abort = false;
	do {
		if (totalBytesRead + BUFFER_SIZE < fileLen) {
			inFile.seekg(-(totalBytesRead + BUFFER_SIZE), std::ios::end);
			bytesRead = inFile.read(&buff[0], BUFFER_SIZE).gcount();
		}
		else {
			inFile.seekg(0, std::ios::beg);
			bytesRead = inFile.read(&buff[0], fileLen - totalBytesRead).gcount();
			abort = true;
		}

		totalBytesRead += bytesRead;

		std::reverse(&buff[0], &buff[bytesRead]);
		outFile.write(&buff[0], bytesRead);
	} while (!abort);

	outFile.flush();
}

void copyBackwardsByteByByte(std::ifstream& inFile, std::ofstream& outFile) {
	char buff;
	int totalBytesRead = 1;	//Ommiting EOF

	inFile.seekg(0, std::ios::end);
	int fileLen = inFile.tellg();

	for (; totalBytesRead <= fileLen; totalBytesRead++) {
		inFile.seekg(-totalBytesRead, std::ios::end);
		inFile.read(&buff, 1);
		outFile.write(&buff, 1);
	}

	outFile.flush();
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	auto start = std::chrono::high_resolution_clock::now();
	auto stop = std::chrono::high_resolution_clock::now();
	int durationMicros = 0xDEADBEEF;
	std::string md5;

	//DEBUG
	//args = { "../../../test/TESTFILE.IN", "../../../test/result/TESTFILE.OUT" };

	if (args.size() != 2)
		throw std::runtime_error("Imporper arguments count!"s);


	std::ifstream inFile(args[0]);
	std::ofstream outFile(args[1]);
	if (!inFile)
		throw std::runtime_error("Error: cannot open input file."s);
	if (!outFile)
		throw std::runtime_error("Error: cannot open output file."s);


	/* TESTING USING BUFFER */

	std::cout << "\nCopying backwards using buffer...\n";

	start = std::chrono::high_resolution_clock::now();
	copyBackwardsBuffer(inFile, outFile);
	stop = std::chrono::high_resolution_clock::now();

	durationMicros = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
	std::cout << "Replacing done. \n"
		<< " Elapsed time: " << std::fixed << std::setprecision(3) << durationMicros / 1000.0 << "ms\n";

	md5 = md5OfFile(args[1]);

	inFile.seekg(0, std::ios::beg);
	outFile.close();
	outFile.clear();
	outFile.open(args[1]);

	if (!outFile)
		throw std::runtime_error("Error: cannot open output file."s);

	/* TESTING WITHOUT BUFFER */

	std::cout << "\nCopying backwards without buffer...\n";

	start = std::chrono::high_resolution_clock::now();
	copyBackwardsByteByByte(inFile, outFile);
	stop = std::chrono::high_resolution_clock::now();

	durationMicros = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
	std::cout << "Replacing done. \n"
		<< " Elapsed time: " << std::fixed << std::setprecision(3) << durationMicros / 1000.0 << "ms\n";


	std::cout << ((md5 == md5OfFile(args[1])) ? "\nFiles are identical." : "\nERROR: files are not identical.");

	return 0;
}
