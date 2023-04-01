#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include <chrono>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std::string_literals;

double integral(std::function<double(double)> fn, double lowerBound, double upperBound, double step) {
	double res = 0.0;
	double halfStep = step * 0.5;

	lowerBound += halfStep;
	upperBound -= halfStep;

	for (double currX = lowerBound; currX <= upperBound; currX += step) {
		res += fn(currX) * step;
	}

	return res;
}

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	if(args.size() != 3 && args.size() != 0)
		throw std::runtime_error("Two arguments expected!");
	
	double rectangle_size = (stod(args[1]));
	int n_processes = (stoi(args[2]));

	double interval_width = (1.0 / n_processes);

	std::vector<int> child_streams(n_processes);
	int data_pipe[2];
	pipe(data_pipe);

	std::cout << "Calculating value using " << n_processes << " worker processes.\n";
	auto start = std::chrono::high_resolution_clock::now();

	for (auto& i : child_streams) {
		int return_pipe[2];
		pipe(return_pipe);
		if (fork()) {
			close(return_pipe[1]);
			i = return_pipe[0];
		}
		else {
			close(data_pipe[1]);
			
			double buff;
			while (read(data_pipe[0], (void*)(&buff), sizeof(buff))) {
				buff = integral([](double x) -> double { return 4.0 / (x * x + 1.0); }, buff, buff + interval_width, rectangle_size);
				write(return_pipe[1], (void*)(&buff), sizeof(buff));
			}

			close(return_pipe[1]);
			close(data_pipe[0]);
			exit(0);
		}
	}

	for (double interval_beign = 0.0; interval_beign < 1.0; interval_beign += interval_width) {
		write(data_pipe[1], (void*)(&interval_beign), sizeof(interval_beign));
	}
	close(data_pipe[1]);
	
	double sum = 0.0;
	double buff;
	for (auto& i : child_streams) {
		while (read(i, (void*)(&buff), sizeof(buff)))
			sum += buff;
	}


	std::cout << "Calculated value: " << sum;


	auto stop = std::chrono::high_resolution_clock::now();
	double duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000.0;
	std::cout << "\nElapsed time: " << std::fixed << std::setprecision(3) << duration << "ms\n";

	return 0;
}
