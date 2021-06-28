#pragma once
#include <chrono>

template <typename T>
struct TimeTester {
	T avg = 0;
	T sum;
	unsigned int total;
	unsigned int minTrials = 1;
	T slack = 1;
	

	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	std::chrono::duration<double> timetaken;
	
	TimeTester() = default;
	TimeTester(unsigned int min_trials, T given_slack = 1) : minTrials{min_trials}, slack{given_slack} {}
	
	void SetMinimumTotal(const unsigned int min_trials) { minTrials = min_trials; }
	void SetSlack(const T given_slack) { slack = given_slack; }
	
	//Does a time test on the given function, with the following arguments being used as the arguments to the function
	//	The time test goes for at least minTrials trials, and after that stops when the avg changes by less than the slack value.
	template <typename Func, typename ... ArgT>
	void DoTest(Func func, ArgT ... args);
	
	//A short form for DoTest, allowing for layered time tests, since Func can be a functor (something with operator())
	template <typename Func, typename ... ArgT>
	void operator() (Func func, ArgT ... args) { DoTest(func, args...); }
	
	void PrintResults() {
		std::cout << "Averaged " << avg << " Over " << total << " Trials." << std::endl;
	}
};

template <typename T>
template <typename Func, typename ... ArgT>
void TimeTester<T>::DoTest(Func func, ArgT ... args) {
	T avg2;
	sum = 0;
	total = 0;
	func(args...); //untracked first run to offset inconsistencies from caching (I presume that's what's going on)
	do {
		avg2 = avg;
		t1 = std::chrono::high_resolution_clock::now();
		func(args...);
		t2 = std::chrono::high_resolution_clock::now();
		timetaken = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		
		sum += timetaken.count();
		total++;
		avg = sum / total;
	} while (avg2 < avg - slack || avg2 > avg + slack || total < minTrials);
}

