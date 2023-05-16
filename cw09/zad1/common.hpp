#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <random>

namespace chr = std::chrono;


constexpr int REINDEER_COUNT = 9;
constexpr int ELVES_COUNT = 10;

constexpr int MIN_PRESENTS_TIME = 2000;
constexpr int MAX_PRESENTS_TIME = 4000;

constexpr int MIN_HELPING_TIME = 1000;
constexpr int MAX_HELPING_TIME = 2000;


class SantaHut {
public:
	SantaHut() {
		for (auto& i : this->go_to_work)
			i = false;
		for (auto& i : this->go_to_holiday)
			i = false;
	}

	void reindeerComeback(int id);

	void elfHasProblem(int elf_id);

	void waitUntilSantaIsBorn() {
		std::unique_lock santa_lock(santa_mutex);
		santa_cv.wait(santa_lock, [&] {return (this->santa_exists).load(); });
	}

	void santa();

	std::atomic<bool> terminate = false;

private:
	std::mutex reindeer_mutex;
	std::mutex elf_mutex;
	std::mutex santa_mutex;

	std::condition_variable santa_cv;
	std::condition_variable reindeer_cv;
	std::condition_variable elves_cv;

	std::array<std::atomic<bool>, REINDEER_COUNT> go_to_holiday;	//each reindeer has its own boolean through witch santa notifies him
	std::array<std::atomic<bool>, ELVES_COUNT> go_to_work;	//each elf has its own boolean through witch santa notifies him
	std::atomic<int> wake_santa = false;
	std::atomic<bool> santa_exists = false;


	std::atomic<int> reindeer_waiting;
	std::atomic<int> elves_waiting;
	std::array<std::atomic<int>, 3> waiting_elves_ids;
};

void SantaHut::reindeerComeback(int id) {
	std::unique_lock reindeer_lock(reindeer_mutex);
	std::unique_lock santa_lock(santa_mutex);

	if (this->reindeer_waiting < REINDEER_COUNT - 1) {
		reindeer_waiting++;

		std::cout << "Reindeer no. " + std::to_string(id) + ": " + std::to_string(reindeer_waiting) + " waiting for Santa...\n";

		santa_lock.unlock();
		reindeer_cv.wait(reindeer_lock, [&, id] {return (this->go_to_holiday[id]).load(); });
		this->go_to_holiday[id] = false;
	}
	else {
		reindeer_waiting++;
		wake_santa++;
		santa_cv.notify_all();

		std::cout << "Reindeer no. " + std::to_string(id) + ": " + std::to_string(reindeer_waiting) + " waiting for Santa - I'm waking the Boss.\n";

		santa_lock.unlock();
		reindeer_cv.wait(reindeer_lock, [&, id] {return (this->go_to_holiday[id]).load(); });
		this->go_to_holiday[id] = false;
	}
}


void SantaHut::elfHasProblem(int id) {
	std::unique_lock elves_lock(elf_mutex);
	if (this->elves_waiting >= 3) {
		std::cout << "Elf no. " + std::to_string(id) + ": Ugh... fine. I'll handle it myself.\n";
		return;
	}

	std::unique_lock santa_lock(santa_mutex);
	if (this->elves_waiting < 2) {
		waiting_elves_ids[elves_waiting] = id;
		elves_waiting++;

		std::cout << "Elf no. " + std::to_string(id) + ": " + std::to_string(elves_waiting) + " waiting for Santa...\n";

		santa_lock.unlock();
		elves_cv.wait(elves_lock, [&, id] {return (this->go_to_work[id]).load(); });
		this->go_to_work[id] = false;
	}
	else {
		waiting_elves_ids[elves_waiting] = id;
		elves_waiting++;
		wake_santa++;
		santa_cv.notify_all();

		std::cout << "Elf no. " + std::to_string(id) + ": " + std::to_string(elves_waiting) + " waiting for Santa - I'm waking the Boss.\n";

		santa_lock.unlock();
		elves_cv.wait(elves_lock, [&, id] {return (this->go_to_work[id]).load(); });
		this->go_to_work[id] = false;
	}
}


void SantaHut::santa() {
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<> presents_dist(MIN_PRESENTS_TIME, MAX_PRESENTS_TIME);
	std::uniform_int_distribution<> helping_dist(MIN_HELPING_TIME, MAX_HELPING_TIME);

	santa_exists = true;
	std::cout << "Santa has been born!\n";
	santa_cv.notify_all();

	while (!this->terminate) {
		std::unique_lock santa_lock(santa_mutex);
		santa_cv.wait(santa_lock, [&] {return (this->wake_santa).load(); });
		std::cout << "Santa has been awakened\n";

		if (elves_waiting == 3) {
			std::cout << "Santa: helping elves\n";

			std::this_thread::sleep_for(chr::milliseconds(presents_dist(gen)));

			for (auto& i : waiting_elves_ids)
				go_to_work[i] = true;

			elves_waiting = 0;
			std::cout << "Santa: finished helping\n";
			wake_santa--;
			elves_cv.notify_all();
		}
		if (reindeer_waiting == REINDEER_COUNT) {
			std::cout << "Santa: delivering presents\n";
			std::this_thread::sleep_for(chr::milliseconds(presents_dist(gen)));

			for (auto& i : go_to_holiday)
				i = true;

			reindeer_waiting = 0;
			std::cout << "Santa: presents delivered\n";
			wake_santa--;
			reindeer_cv.notify_all();
		}
	}
}

SantaHut santaHut;
