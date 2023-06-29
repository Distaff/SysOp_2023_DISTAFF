#pragma once

#include <mutex>

template<typename T, int SIZE>
class Queue_FIFO {
	int first = 0;
	int last = 0;
	T data[SIZE] = { 0 };
	std::mutex mtx;

public:
	void push(T val) {
		this->data[this->last] = val;
		this->last = (this->last + 1) % SIZE;
	}
	T peek() {
		return this->data[this->first];
	}
	T pop() {
		int last_val = this->first;
		this->first = (this->first + 1) % SIZE;
		return this->data[last_val];
	}
	auto get_lock() {
		return std::unique_lock(mtx);
	}
};