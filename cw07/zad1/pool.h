#pragma once

#include <string>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>

typedef union {
	int val;
	struct semid_ds* buf;
	unsigned short* array;
	struct seminfo* __buf;
} semun_t;

template<bool IS_LOCKING>
class Pool {
private:
	int semset;
	std::string semname;
	int capacity;
public:
	Pool(std::string arg_semname, int arg_capacity) : semname(arg_semname), capacity(arg_capacity) {
		system(("touch " + arg_semname).c_str());
		this->semset = semget(ftok(arg_semname.c_str(), 1), 1, IPC_CREAT | 0777);
		//std::cout << " creating semaphore " << arg_semname << " " << this->semset << " " << ftok(arg_semname.c_str(), 1) << "Errno:" << strerror(errno) << std::endl;
		semun_t semun;
		semun.val = this->capacity;
		semctl(semset, 0, SETVAL, semun);
	};

	~Pool() {
		std::cout << "releasing semaphore " + std::string(this->semname.c_str()) + " (process no. " + std::to_string(getpid()) << ")" << std::endl;
		semctl(semset, 0, IPC_RMID, 0);
	};

	bool takePlace() {
		if constexpr (IS_LOCKING) {
			sembuf sembufffer = { 0, -1, 0 };
			//std::cout << semname << ": " << semctl(this->semset, 0, GETVAL, nullptr);
			semop(this->semset, &sembufffer, 1);
			//std::cout << " " << semctl(this->semset, 0, GETVAL, nullptr) << std::endl;
			return true;
		}
		else {
			timespec timespec1 = { 0, 0 };
			sembuf sembuf1 = { 0, -1, 0 };
			//std::cout << semname << ": " << semctl(this->semset, 0, GETVAL, nullptr);
			auto retval = (semtimedop(this->semset, &sembuf1, 1, &timespec1) == 0) ? true : false;
			//std::cout << " " << semctl(this->semset, 0, GETVAL, nullptr) << std::endl;
			return retval;
		}
	}
	void releasePlace() {
		sembuf sembufffer = { 0, +1, 0 };
		semop(this->semset, &sembufffer, 1);
	}
};

typedef Pool<true> BlockingPool;
typedef Pool<false> NonblockingPool;
