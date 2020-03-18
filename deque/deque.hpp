#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

#define __DEFAULT_BLOCK_SIZE   (sizeof (T) <= 1 ? 16 \
    : sizeof (T) <= 2 ? 8 \
    : sizeof (T) <= 4 ? 4 \
    : sizeof (T) <= 8 ? 2 \
    : 1)

namespace sjtu
{
	
	template<class T>
	class deque
	{
		typedef T *pointer;
		
		struct __data_node
		{
			union __val_ptr
			{
				pointer val;
				char *buf;
			} start, finish, end_of_storage;
			__data_node *next, *prev;
			
			__data_node(const size_t &_capacity, __data_node *const &_next = nullptr, __data_node *const &_prev = nullptr):
					next(_next), prev(_prev)
			{
				start.buf = finish.buf = new char[sizeof(T) * _capacity];
				end_of_storage.val = start.val + _capacity;
			}
			
			~__data_node()
			{
				for(pointer ptr = start.val; ptr != finish.val; ptr++)
					ptr->~T();
				delete[] start.buf;
			}
			
			size_t size() const
			{
				return finish.val - start.val;
			}
			
			size_t capacity() const
			{
				return end_of_storage.val - start.val;
			}
			
			pointer insert(size_t ind, const T &value)
			{
				pointer ret;
				if(finish == end_of_storage)
				{
					__data_node *pre_next = next;
					next = new __data_node(capacity(), pre_next, this);
					pre_next->prev = next;
					size_t half = (size() + 1) >> 1;
					if(ind <= half)
					{
						next->finish.val = next->start.val + size() - half;
						for(size_t i = size() - 1; i >= half; i--)
						{
							new(next->start.val + i - half) T(*(start.val + i));
							delete *(start.val + i);
						}
						for(pointer p = start.val + half; p != start.val + ind; p--)
						{
							new(p) T(*(p - 1));
							delete *(p - 1);
						}
						ret = new(start.val + ind)T(value);
						finish.val = start.val + half + 1;
					}
					else
					{
						next->finish.val = next->start.val + size() - half + 1;
						for(size_t i = next->size() - 1; i >= ind; i--)
						{
							new(next->start.val + i - half + 1) T(*(start.val + i));
							delete *(start.val + half + i);
						}
						ret = new(next->start.val + ind - half)T(value);
						for(size_t i = ind - 1; i >= half; i--)
						{
							new(next->start.val + i - half) T(*(start.val + i));
							delete *(start.val + i);
						}
					}
				}
				else
				{
					for(pointer p = start.val + size(); p != start.val + ind; p--)
					{
						new(p) T(*(p - 1));
						delete *(p - 1);
					}
					ret = new(start.val + ind)T(value);
					finish.val++;
				}
				return ret;
			}
			
			pointer erase(size_t ind)
			{
				
			}
		} *head = nullptr, *tail = nullptr;
		
		const size_t __block_size;
	
	
	public:
		class const_iterator;
		
		class iterator
		{
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			deque *const f;
		public:
			/**
			 * return a new iterator which pointer n-next elements
			 *   even if there are not enough elements, the behaviour is **undefined**.
			 * as well as operator-
			 */
			iterator operator+(const int &n) const
			{
				//TODO
			}
			
			iterator operator-(const int &n) const
			{
				//TODO
			}
			
			// return th distance between two iterator,
			// if these two iterators points to different deques, throw invalid_iterator.
			int operator-(const iterator &rhs) const
			{
				//TODO
			}
			
			iterator &operator+=(const int &n)
			{
				//TODO
			}
			
			iterator &operator-=(const int &n)
			{
				//TODO
			}
			
			/**
			 * TODO iter++
			 */
			iterator operator++(int)
			{
			}
			
			/**
			 * TODO ++iter
			 */
			iterator &operator++()
			{
			}
			
			/**
			 * TODO iter--
			 */
			iterator operator--(int)
			{
			}
			
			/**
			 * TODO --iter
			 */
			iterator &operator--()
			{
			}
			
			/**
			 * TODO *it
			 */
			T &operator*() const
			{
			}
			
			/**
			 * TODO it->field
			 */
			pointer operator->() const noexcept
			{
			}
			
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const
			{
			}
			
			bool operator==(const const_iterator &rhs) const
			{
			}
			
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const
			{
			}
			
			bool operator!=(const const_iterator &rhs) const
			{
			}
		};
		
		class const_iterator
		{
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator()
			{
				// TODO
			}
			
			const_iterator(const const_iterator &other)
			{
				// TODO
			}
			
			const_iterator(const iterator &other)
			{
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
		};
		
		/**
		 * TODO Constructors
		 */
		explicit deque(const size_t &___block_size = __DEFAULT_BLOCK_SIZE): __block_size(___block_size)
		{
		}
		
		deque(const deque &other)
		{
		}
		
		/**
		 * TODO Deconstructor
		 */
		~deque()
		{
		}
		
		/**
		 * TODO assignment operator
		 */
		deque &operator=(const deque &other)
		{
		}
		
		/**
		 * access specified element with bounds checking
		 * throw index_out_of_bound if out of bound.
		 */
		T &at(const size_t &pos)
		{
		}
		
		const T &at(const size_t &pos) const
		{
		}
		
		T &operator[](const size_t &pos)
		{
		}
		
		const T &operator[](const size_t &pos) const
		{
		}
		
		/**
		 * access the first element
		 * throw container_is_empty when the container is empty.
		 */
		const T &front() const
		{
		}
		
		/**
		 * access the last element
		 * throw container_is_empty when the container is empty.
		 */
		const T &back() const
		{
		}
		
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin()
		{
		}
		
		const_iterator cbegin() const
		{
		}
		
		/**
		 * returns an iterator to the end.
		 */
		iterator end()
		{
		}
		
		const_iterator cend() const
		{
		}
		
		/**
		 * checks whether the container is empty.
		 */
		bool empty() const
		{
		}
		
		/**
		 * returns the number of elements
		 */
		size_t size() const
		{
		}
		
		/**
		 * clears the contents
		 */
		void clear()
		{
		}
		
		/**
		 * inserts elements at the specified locat on in the container.
		 * inserts value before pos
		 * returns an iterator pointing to the inserted value
		 *     throw if the iterator is invalid or it point to a wrong place.
		 */
		iterator insert(iterator pos, const T &value)
		{
		}
		
		/**
		 * removes specified element at pos.
		 * removes the element at pos.
		 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
		 */
		iterator erase(iterator pos)
		{
		}
		
		/**
		 * adds an element to the end
		 */
		void push_back(const T &value)
		{
		}
		
		/**
		 * removes the last element
		 *     throw when the container is empty.
		 */
		void pop_back()
		{
		}
		
		/**
		 * inserts an element to the beginning.
		 */
		void push_front(const T &value)
		{
		}
		
		/**
		 * removes the first element.
		 *     throw when the container is empty.
		 */
		void pop_front()
		{
		}
	};
	
}

#endif
