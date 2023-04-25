#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "grid.h"


class ThreadedUpdater {
public:
	ThreadedUpdater(int num_threads, char*& foreground, char*& background);
	//~ThreadedUpdater();	//No need for destructor - threads will be killed when going out of scope which is when threads vector is deleted

	void waitForFrame(int64_t thread_frame_ctr);
	void nextFrame();

	char*& next_frame_data;
	char*& last_frame_data;

private:
	std::vector<std::thread> threads;

	std::atomic<int64_t> frame_ctr = 0;
	std::mutex mtx;
	std::condition_variable cv;
};

void threadMain(int threadID, int threadCount, ThreadedUpdater& updater);