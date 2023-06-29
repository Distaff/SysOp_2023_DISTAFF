#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

#include "server.h"
#include "client.h"


using namespace std::string_literals;

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);
	bool is_server = true;

	if (args.size() == 2) {
		if (args[1] == "-s" || args[1] == "--server")
			is_server = true;
		else if (args[1] == "-c" || args[1] == "--client")
			is_server = false;
		else
			throw std::runtime_error("Incorrect arguments");
	}
	

	if (is_server) {
		std::cout << "Initializing server...\n";
		server::srv_init();
		std::cout << "Initialization complete.\n";
		server::mainLoop();
	}
	else {
		std::cout << "Initializing client...\n";
		client::client_init();
		std::cout << "Initialization complete.\n";
		client::mainLoop();
	}

	return 0;
}
