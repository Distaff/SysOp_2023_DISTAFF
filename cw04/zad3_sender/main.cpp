#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std::string_literals;

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if(args.size() != 1)
		throw std::runtime_error("One argument expected!");
	int catcher_PID = std::stoi(args[0]);
	
	sigset_t set_sigusr1;
	sigemptyset(&set_sigusr1);
	sigaddset(&set_sigusr1, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set_sigusr1, NULL);

	int buff = 0;
	while(std::cin >> buff) {
		sigval value;
		value.sival_int = buff;
		int retval = sigqueue(catcher_PID, SIGUSR1, value);
		if(retval) 
			throw std::system_error(errno, std::system_category(), "sigqueue failed");
		std::cout << "Sent: SIGUSR1, " << buff << " to: " << catcher_PID <<  "\n";
		
		
		sigwait(&set_sigusr1, &buff);
		std::cout << "Acknowledge recieved.\n";
	}

	return 0;
}
