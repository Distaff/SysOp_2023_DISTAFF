#pragma once

#include "common.hpp"

constexpr int MIN_WORK_TIME = 2000;
constexpr int MAX_WORK_TIME = 5000;


void elf(int id) {
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<> dist(MIN_WORK_TIME, MAX_WORK_TIME);

	santaHut.waitUntilSantaIsBorn();

	while (!santaHut.terminate) {
		std::this_thread::sleep_for(chr::milliseconds(dist(gen)));
		santaHut.elfHasProblem(id);
	}
}