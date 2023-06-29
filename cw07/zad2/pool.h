#pragma once

#include <string>
#include <semaphore.h>
#include <fcntl.h>


template<bool IS_LOCKING>
class Pool {
private:
	sem_t* sem;
	std::string semname;
	int capacity;
public:
	Pool(std::string arg_semname, int arg_capacity) : semname(arg_semname), capacity(arg_capacity) {
		this->sem = sem_open(this->semname.c_str(), O_CREAT, 0777, capacity);
	};

	~Pool() {
		std::cout << "releasing semaphore " + std::string(this->semname.c_str()) + " (process no. " + std::to_string(getpid()) << ")" << std::endl;
		sem_close(this->sem);
		sem_unlink(this->semname.c_str());
	};

	bool takePlace() {
		if constexpr (IS_LOCKING) {
			sem_wait(this->sem);
			return true;
		}
		else {
			return (sem_trywait(this->sem) == 0) ? true : false;
		}
	}
	void releasePlace() {
		sem_post(this->sem);
	}
};

typedef Pool<true> BlockingPool;
typedef Pool<false> NonblockingPool;
