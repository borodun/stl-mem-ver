#include <ostream>
#include <set>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <atomic>
#include <iostream>
#include <tuple>

#include "vs_set.h"
#include "vs_thread.h"
#include "utils.h"

class ThreadLog
{
private:
	const std::chrono::time_point<std::chrono::system_clock> tp =
		std::chrono::system_clock::now();
	const std::thread::id tid = std::this_thread::get_id();

public:
	friend std::ostream& operator<<(std::ostream& os, const ThreadLog& tl);
	friend bool operator<(const ThreadLog& l, const ThreadLog& r)
	{
		return std::tie(l.tp, l.tid) < std::tie(r.tp, r.tid);
	}
};

std::ostream& operator<<(std::ostream& os, const ThreadLog& tl)
{
	const std::time_t t_c = std::chrono::system_clock::to_time_t(tl.tp);
	os << tl.tid << "/" << std::put_time(std::localtime(&t_c), "%F %T")
		<< std::flush;
	return os;
}

class SetFiller
{
private:
	vs::vs_set<ThreadLog> logs;
	int nthreads;
	std::vector<vs::thread> random_writer_threads;
	std::atomic_bool stop_called = false;

	void random_writer();
public:
	SetFiller(int _nthreads): nthreads(_nthreads) {};
	~SetFiller();

	void start();
	void stop();

	void print(std::ostream& os);
};

SetFiller::~SetFiller()
{
	stop();
}

void SetFiller::start()
{
	for (int i = 0; i < nthreads; i++)
	{

		random_writer_threads.push_back(vs::thread(&SetFiller::random_writer, this));
	}
}

void SetFiller::stop()
{
	stop_called = true;

	for (auto &i : random_writer_threads)
	{
		i.join();
	}
}

void SetFiller::print(std::ostream& out)
{
	for(auto& i: logs)
	{
		out << i << " ";
	}
	out << std::endl;

}

void SetFiller::random_writer()
{
	using namespace std::chrono_literals;

	while (!stop_called)
	{
		logs.insert(ThreadLog());
		print(std::cout);
		std::this_thread::sleep_for(100ms);
	}
}


int main (void)
{
	using namespace std::chrono_literals;

	SetFiller filler(2);

	filler.start();

	std::this_thread::sleep_for(200ms);

	filler.stop();

	filler.print(std::cout);
	return 0;
}