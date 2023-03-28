#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std::string_literals;

void singnal_handler_siginfo(int sig, siginfo_t* si, void* v) {
	std::cout << "SIGUSR1 recieved. Siginfo: \n" << std::hex
		<< "\tsi_signo:\t0x" << si->si_signo << '\n'
		<< "\tsi_code:\t0x" << si->si_code << '\n'
		<< "\tsi_value:\t0x" << si->si_value.sival_int << '\n'
		<< "\tsi_errno:\t0x" << si->si_errno << '\n'
		<< "\tsi_pid:\t\t0x" << si->si_pid << '\n'
		<< "\tsi_uid:\t\t0x" << si->si_uid << '\n'
		<< "\tsi_addr:\t" << si->si_addr << '\n'
		<< "\tsi_status:\t0x" << si->si_status << '\n'
		<< "\tsi_band:\t0x" << si->si_band << '\n';
}

void singnal_handler_nodefer(int sig, siginfo_t* si, void* v) {
	std::cout << "Handling started...\n";

	static int ctr = 0;
	if(ctr++ < 10)
		raise(SIGUSR1);

	std::cout << "Handling finished.\n";
}

void singnal_handler_resethand(int sig, siginfo_t* si, void* v) {
	std::cout << "Handling SIGUSR1... SA_RESETHAND is set, so next raise should interrupt program. \n";
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if(args.size() != 1)
		throw std::runtime_error("One argument expected!");

	switch (stoi(args[0]))
	{
	case 1: {
		struct sigaction sigact;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = SA_SIGINFO;
		sigact.sa_sigaction = singnal_handler_siginfo;
		sigaction(SIGUSR1, &sigact, NULL);

		sigval value;
		value.sival_int = 0xABBAABBA;
		int retval = sigqueue(getpid(), SIGUSR1, value);
		if (retval)
			throw std::system_error(errno, std::system_category(), "sigqueue failed");
		else
			std::cout << "Signal sent." << '\n';
	} break;

	case 2: {
		struct sigaction sigact;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = SA_NODEFER;
		sigact.sa_sigaction = singnal_handler_nodefer;
		sigaction(SIGUSR1, &sigact, NULL);


		int retval = raise(SIGUSR1);
		if (retval)
			throw std::system_error(errno, std::system_category(), "sigqueue failed");
		else
			std::cout << "Signal sent." << '\n';
	} break;


	case 3: {
		struct sigaction sigact;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = SA_RESETHAND;
		sigact.sa_sigaction = singnal_handler_resethand;
		sigaction(SIGUSR1, &sigact, NULL);


		int retval = raise(SIGUSR1);
		retval = raise(SIGUSR1);
		if (retval)
			throw std::system_error(errno, std::system_category(), "sigqueue failed");
		else
			std::cout << "Signal sent." << '\n';
	} break;


	default:
		throw std::runtime_error("Value of 1, 2 or 3 expected");
		break;
	}


	return 0;
}
