#ifndef _VS_TREE_H
#define _VS_TREE_H

#include <functional>
#include <initializer_list>
#include <iterator>
#include <stack>

#include "versioned.h"
#include "revision.h"
#include "strategy.h"

namespace vs
{
	template<typename _Key, typename _Comp>
	class vs_tree_strategy;

	/* internal class */

	/**
	 * @brief Manages all balancing logic and correlating allocations;
	 */
	template<typename _Key, typename _Comp = std::less<_Key>>
	struct _vs_tree_node
	{
		public:

		typedef _vs_tree_node<_Key, _Comp>* _Ptr_type;
		typedef int size_type;

		_Key value;
		size_type height = 0;
		_Ptr_type left = nullptr;
		_Ptr_type right = nullptr;

		/* ------------------ Constructors ----------------------*/

		_vs_tree_node(const _Key& _value)
		: value(_value){ }

		_vs_tree_node(const _vs_tree_node& _node)
		: value(_node.value), height(_node.height) { }

		/* ------------------- Balancing ----------------------*/

		void refresh_node_height()
		{
			size_type lh = (left ? left->height : 0);
			size_type rh = (right ? right->height : 0);

			height = (lh > rh ? lh : rh) + 1;
		}

		size_type
		node_delta_height()
		{
			size_type lh = (left ? left->height : 0);
			size_type rh = (right ? right->height : 0);
			
			return lh - rh;
		}

		_Ptr_type
		rebalance()
		{
			refresh_node_height();

			switch (node_delta_height())
			{
				case 2:
					if (left->node_delta_height() < 0)
						left = left->turnleft();

					return turnright();

				case -2:
					if (right->node_delta_height() > 0)
						right = right->turnright();
					return turnleft();
				
				default:
					return this;
			}
		}

		_Ptr_type
		turnleft(){
			_Ptr_type child = right;
			right = child->left;
			child->left = this;
			refresh_node_height();
			child->refresh_node_height();
			return child;
		}

		_Ptr_type
		turnright(){
			_Ptr_type child = left;
			left = child->right;
			child->right = this;
			refresh_node_height();
			child->refresh_node_height();
			return child;
		}

		/**
		 * @brief Fall down recursively, insert and rebalance on the way up
		 */
		_Ptr_type
		node_insert(const _Key& _value, _Comp comp = _Comp{})
		{
			if (comp(_value, this->value))
			{
				if (left)
				{
					left = left->node_insert(_value);
				}
				else
				{
					left = new _vs_tree_node(_value);
				}
			}
			else
			{
				if (right)
				{
					right = right->node_insert(_value);
				}
				else
				{
					right = new _vs_tree_node(_value);
				}
			}

			return rebalance();
		}

	};

	template<typename _Key, typename _Comp = std::less<_Key>>
	struct _vs_tree_iterator
	{
		public:

		typedef _vs_tree_node<_Key, _Comp>::_Ptr_type _Ptr_type;

		typedef _Key  value_type;
		typedef _Key& reference;
		typedef _Key* pointer;

		typedef std::forward_iterator_tag iterator_category;
		typedef ptrdiff_t			 difference_type;

		typedef _vs_tree_iterator<_Key, _Comp> _Self;

		_vs_tree_iterator()
		: parents(), node() { }

		explicit
		_vs_tree_iterator(_Ptr_type __x)
		: parents(), node(__x) { }

		reference
		operator*()
		{ return node->value; }

		pointer
		operator->()
		{ return &(node->value); }

		/* ------------------ Post/Pre-increment ----------------------*/
		_Self&
		operator++()
		{
			dfs_increment();
			return *this;
		}

		_Self
		operator++(int)
		{
			_Self __tmp = *this;
			dfs_increment();
			return __tmp;
		}

		// _Self&
		// operator--()
		// {
		// 	dfs_decrement();
		// 	return *this;
		// }

		// _Self
		// operator--(int)
		// {
		// 	_Self __tmp = *this;
		// 	dfs_decrement();
		// 	return __tmp;
		// }

		friend bool
		operator==(const _Self& __x, const _Self& __y)
		{ return __x.node == __y.node; }

		friend bool
		operator!=(const _Self& __x, const _Self& __y)
		{ return __x.node != __y.node; }

		/* dirty hack, but i do not want to store and refresh parents */
		std::stack<_Ptr_type> parents;
		bool came_with_right = false;
		_Ptr_type node;

		private:

		void
		dfs_increment()
		{
			if (node->left)
			{
				parents.push(node);
				node = node->left;
				came_with_right = false;
			}
			else if (node->right)
			{
				parents.push(node);
				node = node->right;
				came_with_right = true;
			}
			else
			{
				while (!parents.empty())
				{
					node = parents.top();

					if (came_with_right)
					{
						parents.pop();
						continue;
					}

					if (node->right)
					{
						node = node->right;
						came_with_right = true;
					}
					else
						parents.pop();
				}

				if (parents.empty())
				{
					/* end() iterator, reached top and cannot advance right */
					node = nullptr;
				}
			}
		}
	};

	/**
	 * @brief serves as stl-like interface from node to vs_tree; 
	 * handles allocations in construction and destruction.
	 */
	template<typename _Key, typename _Comp = std::less<_Key>>
	class _vs_tree
	{
		public:

		/* public typedefs */
		typedef _vs_tree_iterator<_Key, _Comp> iterator;
		typedef _vs_tree_node<_Key, _Comp>::_Ptr_type _Ptr_type;
		typedef _vs_tree_node<_Key, _Comp>::size_type size_type;

		/* needed for concept */
		typedef _Key value_type;

		private:

		_Ptr_type head = nullptr;
		/* _ because of typedef's declaration conflicts */
		size_type _height = 0;
		size_type _size = 0;

		/**
		 * @brief tree deep-copy
		 * 
		 * It is a bit recursive, but it should end up fine for balanced tree
		 * that AVL is.
		 */
		void
		copy_subtree(_Ptr_type dst, _Ptr_type src)
		{
			if (src->left)
			{
				dst->left = new _vs_tree_node(*(src->left));
				copy_subtree(dst->left, src->left);
			}
			if (src->right)
			{
				dst->right = new _vs_tree_node(*(src->right));
				copy_subtree(dst->right, src->right);
			}
		}

		/**
		 * @brief destructor helper
		 * 
		 * It is a bit recursive, but it should end up fine for balanced tree
		 * that AVL is.
		 */
		void
		delete_subtree(_Ptr_type node)
		{
			if (node->left)
				delete_subtree(node->left);

			if (node->right)
				delete_subtree(node->right);

			delete node;
		}

		public:
		/* ------------------ Constructors ----------------------*/

		_vs_tree() = default;

		_vs_tree(const _vs_tree& _tree)
		{
			copy_subtree(this.head, _tree.head);
		}

		~_vs_tree()
		{
			if (head)
				delete_subtree(head);
		}

		/* ------------------ Accessors ----------------------*/

		iterator
		begin()
		{
			return iterator(this->head);
		}
		iterator
		end()
		{
			return iterator(nullptr);
		}

		iterator
		begin() const
		{
			return iterator(this->head);
		}
		iterator
		end() const
		{
			return iterator(nullptr);
		}

		const _Key&
		top()
		{
			return *begin();
		}
		
		/**
		 * @brief simple recursive implementation of find
		 */
		iterator
		find(const _Key& _x)
		{
			return find_subtree(_x, head);
		}

		iterator
		find_subtree(const _Key& _x, _Ptr_type& node, _Comp comp = _Comp{})
		{
			if (node->value == _x)
				return iterator(node);

			if (comp(_x, node->value))
			{
				if (node->left)
					return find_subtree(_x, node->left);
				else
					return end();
			}
			else
			{
				if (node->right)
					return find_subtree(_x, node->right);
				else
					return end();
			}
		}

		size_type
		height() const
		{ return _height; }

		size_type
		size() const
		{ return _size; }

		/* ------------------ Operators ----------------------*/

		void 
		push(const _Key& _value)
		{
			if (head)
				head = head->node_insert(_value);
			else
				head = new _vs_tree_node<_Key, _Comp>(_value);

			_height = head->height + 1;
			_size++;
		}

		// void
		// pop()
		// {
		// 	if (head)
		// 	{
		// 		head->node_delete();
		// 		_height = head.height + 1;
		// 		_size--;
		// 	}
		// }

	};

	/* TODO: add param _Strategy */

	/**
	 *  @brief A versioned AVL-tree with deep copy constructor in internal classes
	 *
	 *  @param _Key  Type of key objects.
	 *  @param _Comp  Comparison function object type, defaults to less<_Key>.
	 *  @param _Strategy  Custom strategy class for different merge behaviour
	 */
	template<typename _Key, typename _Comp = std::less<_Key>, typename _Strategy = vs_tree_strategy<_Key, _Comp>>
	class vs_tree
	{

	static_assert(vs::IsMergeStrategy<_Strategy, _vs_tree<_Key, _Comp>>, 
	"Provided invalid strategy class in template");

	public:
	/* public typedefs */

	typedef Versioned<_vs_tree<_Key, _Comp>, _Strategy> _Versioned;
	typedef _vs_tree<_Key, _Comp>::iterator iterator;
	typedef _vs_tree<_Key, _Comp>::size_type size_type;

	private:

	_Versioned _v_t;

	public:

	/* ------------------ Constructors ----------------------*/
	/**
	 * @brief  Creates a vs_tree with no elements.
	 * @param  __comp  Comparator to use.
	 */
	explicit
	vs_tree(const _Comp& __comp = _Comp())
	: _v_t(_vs_tree<_Key, _Comp>()) { }

	/**
	 * @brief  Builds a vs_tree from an initializer_list.
	 * @param  __l  An initializer_list.
	 * @param  __comp  Comparator to use.
	 * 
	 * Copy elements of the list to created vs_tree.
	 * Lots of copies in process, but only one version gets added.
	 */
	vs_tree(std::initializer_list<_Key> __l,
		   const _Comp& __comp = _Comp())
	: _v_t(_vs_tree<_Key, _Comp>())
	{
		_v_t.Set(_v_t.Get(),
		[&](_vs_tree<_Key, _Comp>& _tree){
			for (auto& i: __l){
				_tree.push(i);
			}
			return true;
		});
	}

	/**
	 * @brief  vs_tree copy constructor
	 * 
	 * does not inherit versions history
	 */
	vs_tree(const vs_tree& __vs_tree)
	: _v_t(__vs_tree._v_s.Get()) { }

	//*  @brief tree move constructor

	//*  @brief  Builds a tree from a range.


	/* ------------------ Accessors ----------------------*/

	/**
	 * @brief  begin constant iterator
	 * 
	 * Returns an iterator that points to the first
	 * element in the vs_tree. Iteration is done in depth-first order.
	 */
	iterator
	begin()
	{ return _v_t.Get().begin(); }

	/**
	 * @brief end constant iterator
	 * 
	 * Returns an iterator that points one past the last
	 * element in the vs_tree. Iteration is done in depth-first order.
	 */
	iterator
	end()
	{ return _v_t.Get().end(); }

	/**
	 * @brief size of underlying tree
	 */
	size_type
	size() const noexcept
	{ return _v_t.Get().size(); }

	/**
	 * @brief height of underlying tree
	 */
	size_type
	height() const noexcept
	{ return _v_t.Get().height(); }

	/**
	 * @brief find element in tree
	 */
	iterator
	find(const _Key& __x)
	{ return _v_t.Get().find(__x); }

	/* ------------------ Operators ----------------------*/

	/* XXX: move insert */

	/**
	 * @brief Attempts to insert an element into the vs_tree.
	 * @param  __x  Element to be inserted.
	 */
	void
	push(const _Key& __x)
	{
		_v_t.Set(_v_t.Get(), [&](_vs_tree<_Key, _Comp>& _tree){_tree.push(__x); return true;});
	}
	// = (copy)
	// = {}
	};

	/**
	 * @brief simpliest determenistic merge strategy.
	 * 
	 * On merge, puts everything from one stack to other. It is expected to
	 * start with empty stacks and merge remainders, or for user to override
	 * this strategy.
	 */
	template<typename _Key, typename _Comp>
	class vs_tree_strategy
	{
	public:

	void
	merge(_vs_tree<_Key, _Comp>& dst, _vs_tree<_Key, _Comp>& src)
	{
		for (auto& i: src)
		{
			auto found = dst.find(i);
			if (found != dst.end())
				merge_same_element(dst, *found, i);
			else
				dst.push(i);
		}
	}

	void
	merge_same_element(_vs_tree<_Key, _Comp>& dst, _Key& dstk, _Key& srck)
	{
		/* pretend we didnt saw new elements */
	}

	};
}

#endif
