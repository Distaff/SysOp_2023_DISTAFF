#pragma once

#include "common.hpp"

constexpr int MIN_HOLIDAY_TIME = 5000;
constexpr int MAX_HOLIDAY_TIME = 10000;


void reindeer(int id) {
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<> dist(MIN_HOLIDAY_TIME, MAX_HOLIDAY_TIME);

	santaHut.waitUntilSantaIsBorn();

	while (!santaHut.terminate) {
		std::this_thread::sleep_for(chr::milliseconds(dist(gen)));
		santaHut.reindeerComeback(id);
	}
}