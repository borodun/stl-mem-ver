#include <iostream>
#include <functional>
#include <list>
#include <set>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "versioned.h"
#include "revision.h"
#include "utils.h"
#include "vs_set.h"
#include "vs_queue.h"
#include "vs_stack.h"
#include "vs_tree.h"
#include "vs_thread.h"

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

TEST_CASE("Test versioning of the lists ", "[list][basic]") {
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

TEST_CASE("Test versioning of the sets ", "[set][basic]") {
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

TEST_CASE("Test of the vs_sets ", "[set][custom]") {
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