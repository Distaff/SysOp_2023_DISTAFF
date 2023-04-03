#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include <chrono>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


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

	if(args.size() != 3 && args.size() != 5)
		throw std::runtime_error("Two or four arguments expected!");
	
	double rectangle_size = (stod(args[1]));
	int n_processes = (stoi(args[2]));


	if (args.size() == 5) {
		double interval_begin = stod(args[3]);
		double interval_end = stod(args[4]);

		FILE* return_pipe = fopen("/tmp/sysopy_pipe", "wa");
		
		double res = integral([](double x) -> double { return 4.0 / (x * x + 1.0); }, interval_begin, interval_end, rectangle_size);
		fwrite(&res, sizeof(res), 1, return_pipe);

		fclose(return_pipe);
	}
	else {
		mkfifo("/tmp/sysopy_pipe", 0777);
		double interval_size = 1.0 / n_processes;

		for (int i = 0; i < n_processes; i++) {
			if (!fork()) {
				std::ostringstream sstr1, sstr2;
				sstr1 << std::setprecision(15) << (interval_size * i);
				sstr2 << std::setprecision(15) << (interval_size * i + interval_size);
				std::string arg3(sstr1.str());
				std::string arg4(sstr2.str());

				execl("./bin/out", "./bin/out", 
					args[1].c_str(),
					args[2].c_str(),
					arg3.c_str(),
					arg4.c_str(),
					NULL
				);
			}
		}

		FILE* return_pipe = fopen("/tmp/sysopy_pipe", "r");

		std::cout << "Calculating value using " << n_processes << " worker sub-programs.\n";
		auto start = std::chrono::high_resolution_clock::now();


		double sum = 0.0;
		double buff;
		for (int i = 0; i < n_processes; i++) {
			fread(&buff, sizeof(buff), 1, return_pipe);
			sum += buff;
		}
		fclose(return_pipe);


		std::cout << "Calculated value: " << sum;

		auto stop = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000.0;
		std::cout << "\nElapsed time: " << std::fixed << std::setprecision(3) << duration << "ms\n";
	}

	return 0;
}
