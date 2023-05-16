#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "elves.hpp"
#include "reindeers.hpp"
#include "common.hpp"

using namespace std::string_literals;

int main(int argc, char** argv)
{
	//std::vector<std::string> args(argv, argv + argc);
	for (int i = 0; i < REINDEER_COUNT; i++) {
		std::thread th(reindeer, i);
		th.detach();
	}
	for (int i = 0; i < ELVES_COUNT; i++) {
		std::thread th(elf , i);
		th.detach();
	}

	santaHut.santa();

	exit(0);
}
