#include <ostream>
#include <set>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <atomic>
#include <iostream>

class ThreadLog
{
private:
	const std::chrono::time_point<std::chrono::system_clock> tp =
		std::chrono::system_clock::now();
	const std::thread::id tid = std::this_thread::get_id();

public:
	friend std::ostream& operator<<(std::ostream& os, const ThreadLog& tl);
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
	std::set<ThreadLog> logs;
	int nthreads;
	std::vector<std::thread> random_writer_threads;
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
		random_writer_threads.push_back(std::thread(random_writer));
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
	std::set<ThreadLog> clogs(logs);
	for(auto& i: clogs)
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
		std::this_thread::sleep_for(100ms);
	}
}


int main (void)
{
	using namespace std::chrono_literals;

	SetFiller filler(3);
	const int iterations = 10;

	filler.start();

	for (int i = 0; i < iterations; i++)
	{
		filler.print(std::cout);
		std::this_thread::sleep_for(200ms);
	}

	filler.stop();
	return 0;
}