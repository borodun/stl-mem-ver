#include <iostream>
#include <functional>
#include <list>
#include <set>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "versioned.h"
#include "revision.h"
#include "vs_set.h"
#include "vs_queue.h"
#include "vs_stack.h"
#include "vs_tree.h"
#include "vs_thread.h"
#include "test_utils.h"

TEST_CASE("Test versioning basic objects", "[int][basic]") {
	Versioned<int> x = Versioned<int>(0);
	Versioned<int> y = Versioned<int>(100);

	REQUIRE(x.Get() == 0);
	REQUIRE(y.Get() == 100);

	SECTION("Changing all vars in both revisions") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE(x.Get() == 0);
			REQUIRE(y.Get() == 100);
			x.Set(1);
			y.Set(101);
			REQUIRE(x.Get() == 1);
			REQUIRE(y.Get() == 101);
		});
		x.Set(11);
		y.Set(111);
		REQUIRE(x.Get() == 11);
		REQUIRE(y.Get() == 111);

		thread.join();
		REQUIRE(x.Get() == 1);
		REQUIRE(y.Get() == 101);
	}

	SECTION("Changing vars only in parent revision") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE(x.Get() == 0);
			REQUIRE(y.Get() == 100);
		});
		x.Set(11);
		y.Set(111);
		REQUIRE(x.Get() == 11);
		REQUIRE(y.Get() == 111);

		thread.join();
		REQUIRE(x.Get() == 11);
		REQUIRE(y.Get() == 111);
	}

	SECTION("Changing vars only in child revision") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE(x.Get() == 0);
			REQUIRE(y.Get() == 100);
			x.Set(1);
			y.Set(101);
			REQUIRE(x.Get() == 1);
			REQUIRE(y.Get() == 101);
		});

		thread.join();
		REQUIRE(x.Get() == 1);
		REQUIRE(y.Get() == 101);
	}

	SECTION("Changing different vars in different revisions") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE(x.Get() == 0);
			REQUIRE(y.Get() == 100);
			x.Set(1);
			REQUIRE(x.Get() == 1);
			REQUIRE(y.Get() == 100);
		});
		y.Set(111);
		REQUIRE(x.Get() == 0);
		REQUIRE(y.Get() == 111);

		thread.join();
		REQUIRE(x.Get() == 1);
		REQUIRE(y.Get() == 111);
	}
}

TEST_CASE("Test versioning of the lists", "[list][basic]") {
	Versioned<std::list<int>> x = Versioned<std::list<int>>({0, 1, 2, 3});
	Versioned<std::list<int>> y = Versioned<std::list<int>>({100, 101, 102, 103});

	REQUIRE_THAT(x.Get(), Catch::Matchers::RangeEquals(std::list<int>({0, 1, 2, 3})));
	REQUIRE_THAT(y.Get(), Catch::Matchers::RangeEquals(std::list<int>({100, 101, 102, 103})));

	SECTION("Changing all lists in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x.Get(), Catch::Matchers::RangeEquals(std::list<int>({0, 1, 2, 3})));
			REQUIRE_THAT(y.Get(), Catch::Matchers::RangeEquals(std::list<int>({100, 101, 102, 103})));
			x.Set({0, 1, 2, 4});
			y.Set({100, 101, 102, 104});
			REQUIRE_THAT(x.Get(), Catch::Matchers::RangeEquals(std::list<int>({0, 1, 2, 4})));
			REQUIRE_THAT(y.Get(), Catch::Matchers::RangeEquals(std::list<int>({100, 101, 102, 104})));
		});
		x.Set({1, 2, 3});
		y.Set({101, 102, 103});
		REQUIRE_THAT(x.Get(), Catch::Matchers::RangeEquals(std::list<int>({1, 2, 3})));
		REQUIRE_THAT(y.Get(), Catch::Matchers::RangeEquals(std::list<int>({101, 102, 103})));

		thread.join();
		REQUIRE_THAT(x.Get(), Catch::Matchers::RangeEquals(std::list<int>({0, 1, 2, 4})));
		REQUIRE_THAT(y.Get(), Catch::Matchers::RangeEquals(std::list<int>({100, 101, 102, 104})));
	}
}

TEST_CASE("Test versioning of the sets", "[set][basic]") {
	Versioned<std::set<int>> x = Versioned<std::set<int>>({0, 1, 2, 3});
	Versioned<std::set<int>> y = Versioned<std::set<int>>({100, 101, 102, 103});

	REQUIRE_THAT(x.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3})));
	REQUIRE_THAT(y.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103})));

	SECTION("Changing all lists in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3})));
			REQUIRE_THAT(y.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103})));
			x.Set({0, 1, 2, 4});
			y.Set({100, 101, 102, 104});
			REQUIRE_THAT(x.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 4})));
			REQUIRE_THAT(y.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 104})));
		});
		x.Set({1, 2, 3});
		y.Set({101, 102, 103});
		REQUIRE_THAT(x.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({1, 2, 3})));
		REQUIRE_THAT(y.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({101, 102, 103})));

		thread.join();
		REQUIRE_THAT(x.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 4})));
		REQUIRE_THAT(y.Get(), Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 104})));
	}
}

TEST_CASE("Test of the vs_sets", "[set][custom]") {
	vs::vs_set<int> x{0, 1, 2, 3};
	vs::vs_set<int> y{100, 101, 102, 103};

	REQUIRE_THAT(x, Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3})));
	REQUIRE_THAT(y, Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103})));

	SECTION("Changing all lists in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x, Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3})));
			REQUIRE_THAT(y, Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103})));
			x.insert(4);
			y.insert(104);
			REQUIRE_THAT(x, Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3, 4})));
			REQUIRE_THAT(y, Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103, 104})));
		});
		x.insert(5);
		y.insert(105);
		REQUIRE_THAT(x, Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3, 5})));
		REQUIRE_THAT(y, Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103, 105})));
		thread.join();
		REQUIRE_THAT(x, Catch::Matchers::UnorderedRangeEquals(std::set<int>({0, 1, 2, 3, 4, 5})));
		REQUIRE_THAT(y, Catch::Matchers::UnorderedRangeEquals(std::set<int>({100, 101, 102, 103, 104, 105})));
	}
}

TEST_CASE("Test of the vs_queue", "[queue][custom]") {
	vs::vs_queue<int> x{0, 1, 2, 3};
	vs::vs_queue<int> y{100, 101, 102, 103};

	REQUIRE_THAT(x, EqualsQueue(std::queue<int>({0, 1, 2, 3})));
	REQUIRE_THAT(y, EqualsQueue(std::queue<int>({100, 101, 102, 103})));

	SECTION("Changing all queues in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x, EqualsQueue(std::queue<int>({0, 1, 2, 3})));
			REQUIRE_THAT(y, EqualsQueue(std::queue<int>({100, 101, 102, 103})));
			x.push(4);
			y.push(104);
			REQUIRE_THAT(x, EqualsQueue(std::queue<int>({0, 1, 2, 3, 4})));
			REQUIRE_THAT(y, EqualsQueue(std::queue<int>({100, 101, 102, 103, 104})));
		});
		x.pop();
		y.pop();
		REQUIRE_THAT(x, EqualsQueue(std::queue<int>({1, 2, 3})));
		REQUIRE_THAT(y, EqualsQueue(std::queue<int>({101, 102, 103})));
		thread.join();
		REQUIRE_THAT(x, EqualsQueue(std::queue<int>({1, 2, 3, 0, 1, 2, 3, 4})));
		REQUIRE_THAT(y, EqualsQueue(std::queue<int>({101, 102, 103, 100, 101, 102, 103, 104})));
	}
}

TEST_CASE("Test of the vs_stack", "[stack][custom]") {
	vs::vs_stack<int> x{0, 1, 2, 3};
	vs::vs_stack<int> y{100, 101, 102, 103};

	REQUIRE_THAT(x, EqualsStack(std::stack<int>({0, 1, 2, 3})));
	REQUIRE_THAT(y, EqualsStack(std::stack<int>({100, 101, 102, 103})));

	SECTION("Changing all stacks in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x, EqualsStack(std::stack<int>({0, 1, 2, 3})));
			REQUIRE_THAT(y, EqualsStack(std::stack<int>({100, 101, 102, 103})));
			x.push(4);
			y.push(104);
			REQUIRE_THAT(x, EqualsStack(std::stack<int>({0, 1, 2, 3, 4})));
			REQUIRE_THAT(y, EqualsStack(std::stack<int>({100, 101, 102, 103, 104})));
		});
		x.pop();
		y.pop();
		REQUIRE_THAT(x, EqualsStack(std::stack<int>({0, 1, 2})));
		REQUIRE_THAT(y, EqualsStack(std::stack<int>({100, 101, 102})));
		thread.join();
		REQUIRE_THAT(x, EqualsStack(std::stack<int>({0, 1, 2, 4, 3, 2, 1, 0})));
		REQUIRE_THAT(y, EqualsStack(std::stack<int>({100, 101, 102, 104, 103, 102, 101, 100})));
	}
}

TEST_CASE("Test of the vs_tree", "[tree][custom]") {
	vs::vs_tree<int, std::greater<int>> x{0, 1, 2, 3};
	vs::vs_tree<int> y{100, 101, 102, 103};

	REQUIRE_THAT(x, EqualsTree(std::vector({1, 2, 3, 0})));
	REQUIRE_THAT(y, EqualsTree(std::vector({101, 100, 102, 103})));

	SECTION("Changing all trees in both threads") {
		auto thread = vs::thread([&x, &y]() {
			REQUIRE_THAT(x, EqualsTree(std::vector({1, 2, 3, 0})));
			REQUIRE_THAT(y, EqualsTree(std::vector({101, 100, 102, 103})));
			x.push(4);
			y.push(104);
			REQUIRE_THAT(x, EqualsTree(std::vector({1, 3, 4, 2, 0})));
			REQUIRE_THAT(y, EqualsTree(std::vector({101, 100, 103, 102, 104})));
		});
		thread.join();
		REQUIRE_THAT(x, EqualsTree(std::vector({1, 3, 4, 2, 0})));
		REQUIRE_THAT(y, EqualsTree(std::vector({101, 100, 103, 102, 104})));
	}
}