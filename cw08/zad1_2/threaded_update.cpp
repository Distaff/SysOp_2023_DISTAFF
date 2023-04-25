#include "threaded_update.h"

#include <iostream>

void threadMain(int threadID, int threadCount, ThreadedUpdater& updater) {

	for(int64_t frame_ctr = 1; true; frame_ctr++) {
		updater.waitForFrame(frame_ctr);

		for (int i = threadID; i < grid_size; i += threadCount) {
			updater.next_frame_data[i] = is_alive(i / grid_width, i % grid_width, updater.last_frame_data);
		}
	}
}

ThreadedUpdater::ThreadedUpdater(int num_threads, char*& foreground, char*& background) : next_frame_data(foreground), last_frame_data(background) {
	for (int i = 0; i < num_threads; i++)
		threads.push_back(std::thread(threadMain, i, num_threads, std::ref(*this)));
}

void ThreadedUpdater::waitForFrame(int64_t frame_no) {
	std::unique_lock lock(mtx);
	cv.wait(lock, [&, frame_no] {return this->frame_ctr >= frame_no; });
}

void ThreadedUpdater::nextFrame() {
	std::swap(this->last_frame_data, this->next_frame_data);
	this->frame_ctr++;
	cv.notify_all();
}
