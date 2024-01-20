#ifndef _VS_STRATEGY_H
#define _VS_STRATEGY_H

#include <ranges>

namespace vs{

	/**
	 * @brief Concept for compile-time type checking passed user strategies
	 */
	template<typename T, typename Container>
	concept IsMergeStrategy = requires(T t, Container& src, Container& dst,
			Container& dstc, Container::value_type& src_elem, Container::value_type& dst_elem)
	{
		/**
		 * @brief Behaviour when asked to merge two collections
		 * 
		 * User is expected to use find() and iterators for containers.
		 */
		//t.merge(std::declval<Container>()&, std::declval<Container>()&);
		t.merge(dst, src);

		/**
		 * @brief What to do with two of the same element?
		 */
		t.merge_same_element(dstc, dst_elem, src_elem);

	};

}

#endif