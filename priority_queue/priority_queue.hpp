#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include <iostream>
#include "exceptions.hpp"

namespace sjtu
{

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
	template<typename T, class Compare = std::less<T>>
	class priority_queue
	{
		struct Node
		{
			Node *children[2] = {nullptr, nullptr};
			T *val;
			int d = 0;
			
			explicit Node(const T &_val): val(new T(_val))
			{
			}
			
			Node(const Node &other): val(new T(*other.val)), d(other.d)
			{
			}
			
			~Node()
			{
				delete val;
			}
			
			Node &operator=(const Node &other)
			{
				*val = *other.val;
				d = other.d;
			}
			
		};
		
		Node *root = nullptr;
		size_t count = 0;
		Compare cmp;
	
	
	private:
		Node *merge(Node *a, Node *b)
		{
			if(nullptr == a)
				return b;
			if(nullptr == b)
				return a;
			if(cmp(*a->val, *b->val))
				std::swap(a, b);
			a->children[1] = merge(a->children[1], b);
			if(nullptr == a->children[0] || nullptr != a->children[1] && a->children[0]->d < a->children[1]->d)
				std::swap(a->children[0], a->children[1]);
			a->d = nullptr != a->children[1] ? a->children[1]->d + 1 : 0;
			return a;
		}
		
		static void destruct(Node *p)
		{
			for(Node *&child:p->children)
				if(nullptr != child)
					destruct(child);
			delete p;
		}
		
		static void copy(Node *cur, Node *p)
		{
			for(size_t i = 0; i < 2; i++)
				if(nullptr != p->children[i])
				{
					cur->children[i] = new Node(*p->children[i]);
					copy(cur->children[i], p->children[i]);
				}
		}
	
	public:
		/**
		 * TODO constructors
		 */
		priority_queue() = default;
		
		priority_queue(const priority_queue &other)
		{
			count = other.count;
			if(nullptr != other.root)
			{
				root = new Node(*other.root->val);
				copy(root, other.root);
			}
		}
		
		/**
		 * TODO deconstructor
		 */
		~priority_queue()
		{
			if(nullptr != root)
				destruct(root);
		}
		
		/**
		 * TODO Assignment operator
		 */
		priority_queue &operator=(const priority_queue &other)
		{
			if(root == other.root)
				return *this;
			count = other.count;
			if(nullptr != other.root)
			{
				if(nullptr != root)
					destruct(root);
				root = new Node(*other.root);
				copy(root, other.root);
			}
			return *this;
		}
		
		/**
		 * get the top of the queue.
		 * @return a reference of the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		const T &top() const
		{
			if(empty())
				throw container_is_empty();
			return *root->val;
		}
		
		/**
		 * TODO
		 * push new element to the priority queue.
		 */
		void push(const T &e)
		{
			root = merge(root, new Node(e));
			count++;
		}
		
		/**
		 * TODO
		 * delete the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		void pop()
		{
			if(empty())
				throw container_is_empty();
			Node *tmp = merge(root->children[0], root->children[1]);
			root->children[0] = root->children[1] = nullptr;
			delete root;
			root = tmp;
			count--;
		}
		
		/**
		 * return the number of the elements.
		 */
		size_t size() const
		{
			return count;
		}
		
		/**
		 * check if the container has at least an element.
		 * @return true if it is empty, false if it has at least an element.
		 */
		bool empty() const
		{
			return count == 0;
		}
		
		/**
		 * return a merged priority_queue with at most O(logn) complexity.
		 */
		void merge(priority_queue &other)
		{
			root = merge(root, other.root);
			count += other.count;
			other.root = nullptr;
			other.count = 0;
		}
	};
}

#endif
