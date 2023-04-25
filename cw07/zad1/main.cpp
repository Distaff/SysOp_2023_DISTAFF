#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

#include <mutex>

using namespace std::string_literals;

constexpr int MAX_TIME = 8000;	//Max cutting time in ms

class Pool {
	Pool(int size);


};

class PriorityPool : Pool {

};


int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	

	return 0;
}


void handleClient(int client_pid) {
	bool can_wait = false;
	if (waiting_room.hasFreePlaces())
		waiting_room.takePlace();
	else
		can_wait = true;

	if (!seat_pool.takePlace(can_wait))
		exit(0);

	if (!barber_pool.takePlace(can_wait))
		exit(0);

	std::this_thread::sleep_for(std::chrono::duration<std::chrono::milliseconds>(rand % MAX_TIME));
}