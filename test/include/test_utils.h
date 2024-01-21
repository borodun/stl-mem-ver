#ifndef ___TEST_UTILS_H__
#define ___TEST_UTILS_H__

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

template<typename Range>
struct EqualsQueueRangeMatcher : Catch::Matchers::MatcherGenericBase {
    EqualsQueueRangeMatcher(Range const& range):
        range{ range }
    {}

    template<typename OtherRange>
    bool match(OtherRange const& other) const {
		 if (range.size() != other.size()) {
			return false;
		}

		Range temp1 = range;
		OtherRange temp2 = other;

		while (temp1.size() != 0) {
			if (temp1.front() != temp2.front()) {
				return false;
			}
			temp1.pop();
			temp2.pop();
		}

		return true;
    }

	std::string queueToString() const {
		Range temp = range;

		std::ostringstream o;
		o << "{ ";
		while (temp.size() != 0) {
			o << temp.front();
			temp.pop();
			if (temp.size() != 0) {
				o << ", ";
			}
		}
		o << " }" << std::endl;
		return o.str();
	}

    std::string describe() const override {
        return "matches queue " + queueToString();
    }

private:
    Range const& range;
};

template<typename Range>
auto EqualsQueue(const Range& range) -> EqualsQueueRangeMatcher<Range> {
    return EqualsQueueRangeMatcher<Range>{range};
}

template<typename Range>
struct EqualsStackRangeMatcher : Catch::Matchers::MatcherGenericBase {
    EqualsStackRangeMatcher(Range const& range):
        range{ range }
    {}

    template<typename OtherRange>
    bool match(OtherRange const& other) const {
		 if (range.size() != other.size()) {
			return false;
		}

		Range temp1 = range;
		OtherRange temp2 = other;

		while (temp1.size() != 0) {
			if (temp1.top() != temp2.top()) {
				return false;
			}
			temp1.pop();
			temp2.pop();
		}

		return true;
    }

	std::string stackToString() const {
		Range temp = range;
		Range reversedStack;

		while (temp.size() != 0) {
			reversedStack.push(temp.top());
			temp.pop();
		}

		std::ostringstream o;
		o << "{ ";
		while (reversedStack.size() != 0) {
			o << reversedStack.top();
			reversedStack.pop();
			if (reversedStack.size() != 0) {
				o << ", ";
			}
		}
		o << " }";
		return o.str();
	}

    std::string describe() const override {
        return "matches stack " + stackToString();
    }

private:
    Range const& range;
};

template<typename Range>
auto EqualsStack(const Range& range) -> EqualsStackRangeMatcher<Range> {
    return EqualsStackRangeMatcher<Range>{range};
}

template<typename Range>
struct EqualsTreeRangeMatcher : Catch::Matchers::MatcherGenericBase {
    EqualsTreeRangeMatcher(Range const& range):
        range{ range }
    {}

    template<typename OtherRange>
    bool match(OtherRange const& other) const {
		if (int(range.size()) != int(other.size())) {
			return false;
		}

		auto it1 = other.begin();
		auto it2 = range.begin();
		for (; it1 != other.end() && it2 != range.end(); ++it1, ++it2) {
			if (*it1 != *it2)
				return false;
		}

		return true;
    }

    std::string describe() const override {
        return "matches tree " + Catch::rangeToString(range);
    }

private:
    Range const& range;
};

template<typename Range>
auto EqualsTree(const Range& range) -> EqualsTreeRangeMatcher<Range> {
    return EqualsTreeRangeMatcher<Range>{range};
}


#endif
