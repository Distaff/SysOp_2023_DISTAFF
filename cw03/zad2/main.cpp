#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>
#include <unistd.h> 

using namespace std::string_literals;

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	std::cout << args[0] << '\n';
	std::cout.flush();

	execl("/bin/ls", "ls", args[1].c_str(), (char*)0);

	return 0;
}
