#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <mutex>

#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <signal.h>

#include "pool.h"
#include "queue.h"

using namespace std::string_literals;

namespace chr = std::chrono;

constexpr int WAITING_ROOM = 5;
constexpr int BARBER_CHAIRS = 3;
constexpr int BARBERS = 4;
constexpr int MIN_TIME = 2000;	//Min cutting time in ms
constexpr int MAX_TIME = 8000;	//Max cutting time in ms
constexpr int CLIENT_RATE = 1000;

using WaitroomQueue = Queue_FIFO<int, WAITING_ROOM>;

constexpr int SHM_SIZE = sizeof(WaitroomQueue) + sizeof(NonblockingPool) + sizeof(BlockingPool) + sizeof(BlockingPool);

void interrupt_handler(int sig);
void handleClient(int id);

WaitroomQueue* waitroom_queue_ptr;
NonblockingPool* waitroom_pool_ptr;
BlockingPool* barber_pool_ptr;
BlockingPool* seat_pool_ptr;

int main(int argc, char** argv)
{
	struct sigaction sigact;
	sigact.sa_handler = interrupt_handler;
	sigaction(SIGINT, &sigact, nullptr);

	//key_t key = ftok("/cw07", 1);

	int shm_id = shm_open("/cw07", O_RDWR | O_CREAT, 0777);
	if (shm_id == -1)
		throw std::runtime_error("Failed to open shared memory: " + std::string(strerror(errno)));

	if (ftruncate(shm_id, SHM_SIZE) == -1)
		throw std::runtime_error("Failed to truncate shared memory: " + std::string(strerror(errno)));

	int* shmem_ptr = (int*) mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
	if (shmem_ptr == (int*) -1)
		throw std::runtime_error("Failed to map shared memory: " + std::string(strerror(errno)));

	waitroom_queue_ptr = reinterpret_cast<WaitroomQueue*>(shmem_ptr);
	waitroom_pool_ptr = reinterpret_cast<NonblockingPool*>(waitroom_queue_ptr + 1);
	barber_pool_ptr = reinterpret_cast<BlockingPool*>(waitroom_pool_ptr + 1);
	seat_pool_ptr = reinterpret_cast<BlockingPool*>(barber_pool_ptr + 1);

	new(waitroom_queue_ptr) WaitroomQueue;
	new(waitroom_pool_ptr) NonblockingPool("/cw07_waitroom", WAITING_ROOM);
	new(barber_pool_ptr) BlockingPool("/cw07_barbers", BARBERS);
	new(seat_pool_ptr) BlockingPool("/cw07_chairs", BARBER_CHAIRS);

	std::cout << "MAIN " << ": Init done." << std::endl;

	for(int i = 0; true; i++) {
		if (!fork()) {
			handleClient(i);
		}
		std::this_thread::sleep_for(chr::milliseconds(CLIENT_RATE));
	}

	return 0;
}

void interrupt_handler(int sig) {
	std::cout << "SIGINT signal received! Exiting...\n";
	shm_unlink("/cw07");
	waitroom_pool_ptr->~Pool();
	barber_pool_ptr->~Pool();
	seat_pool_ptr->~Pool();
	exit(0);
}

void handleClient(int id) {
	std::random_device rd;
	std::default_random_engine gen(rd());
	std::uniform_int_distribution<> dist(MIN_TIME, MAX_TIME);

	if (!(waitroom_pool_ptr->takePlace())) {
		std::cout << "CLIENT " << id << ": No place for me. Exiting." << std::endl;
		exit(0);
	}

	{
		auto lck = waitroom_queue_ptr->get_lock();
		waitroom_queue_ptr->push(id);
		std::cout << "CLIENT " << id << ": Waiting for barber..." << std::endl;
	}

	while (1) {		//this is ugly, should be based on conditional variable
		auto lck = waitroom_queue_ptr->get_lock();
		if (waitroom_queue_ptr->peek() == id) {
			waitroom_queue_ptr->pop();
			break;
		}
		else {
			sleep(0);
		}
	}

	barber_pool_ptr->takePlace();
	std::cout << "CLIENT " << id << ": Barber locked!" << std::endl;
	seat_pool_ptr->takePlace();
	std::cout << "CLIENT " << id << ": Found place! Releasing waitroom and getting haircut." << std::endl;

	waitroom_pool_ptr->releasePlace();
	std::this_thread::sleep_for(chr::milliseconds(dist(gen)));

	seat_pool_ptr->releasePlace();
	barber_pool_ptr->releasePlace();

	std::cout << "CLIENT " << id << ": Done. Leaving." << std::endl;
	exit(0);
}