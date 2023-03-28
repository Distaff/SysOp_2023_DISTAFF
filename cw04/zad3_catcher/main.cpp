#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <exception>
#include <csignal>

#include <unistd.h>
#include <sys/wait.h>

using namespace std::string_literals;

int working_mode = 0;
int mode_change_ctr = 0;
int sender_PID = 0;

void singnal_handler(int sig, siginfo_t* si, void* v) {
	if (working_mode != si->si_value.sival_int) 
		mode_change_ctr++;
	working_mode = si->si_value.sival_int;
	sender_PID = si->si_pid;
	std::cout << "SIGUSR1 recieved. Value: " << working_mode << ". Sender PID: " << sender_PID << '\n';
	kill(sender_PID, SIGUSR1);
}

int main(int argc, char** argv)
{
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	//sigaddset(&sigact.sa_mask, SIGUSR1);
	sigact.sa_flags = SA_SIGINFO;
	sigact.sa_sigaction = singnal_handler;
	sigaction(SIGUSR1, &sigact, NULL);
	
	sigset_t set_sigusr1;
	sigfillset(&set_sigusr1);
	sigdelset(&set_sigusr1, SIGUSR1);

	while (true) {
		switch (working_mode)
		{

		case 0: {
			std::cout << "Mode 0. No work to do. Process PID: " << getpid() << '\n';
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} break;

		case 1: {
			for (int i = 0; i < 100; i+=10) {
				for (int j = 0; j < 10; j++)
					std::cout << i + j << ' ';
				std::cout << '\n';
			}
			sigsuspend(&set_sigusr1);
		} break;

		case 2: {
			auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::cout << "Current time: " << std::ctime(&current_time);
			sigsuspend(&set_sigusr1);
		} break;

		case 3: {
			std::cout << "Working mode changed " <<  mode_change_ctr << " time(s)\n:";
			sigsuspend(&set_sigusr1);
		} break;

		case 4: {
			auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::cout << "Current time: " << std::ctime(&current_time);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} break;

		case 5: {
			exit(0); 
		} break;


		default:
			exit(-1);
			break;
		}
	}

	return 0;
}


