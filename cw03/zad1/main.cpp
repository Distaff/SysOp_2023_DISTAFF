#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <unistd.h>
#include <sys/wait.h>

using namespace std::string_literals;

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);
	if (args.size() != 2)
		throw std::runtime_error("One argument is needed!"s);

	int children_count = std::stoi(args[1]);

	for (int i = 1; i <= children_count; i++) {
		if (!fork()) {
			std::cout << "Child no. " << i << ":\n"
				<< "\tParent PID:\t" << getppid() << "\n"
				<< "\tProcess PID:\t" << getpid() << "\n";
			return 0;
		}
	}
	while (wait(NULL) > 0);

	std::cout << "Parent:\n"
		<< "\targv[0]:\t" << args[0] << "\n";
	return 0;
}
