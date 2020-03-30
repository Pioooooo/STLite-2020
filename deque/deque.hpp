#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

#define  __DEFAULT_BLOCK_SIZE 512
//#define __DEFAULT_BLOCK_SIZE   (sizeof (T) <= 1 ? 16 \
//    : sizeof (T) <= 2 ? 8 \
//    : sizeof (T) <= 4 ? 4 \
//    : 2)

namespace sjtu
{
	
	template<class T>
	class deque
	{
		typedef T *pointer;
		typedef const T *const_pointer;
		
		struct __data_node
		{
			union __val_ptr
			{
				pointer val;
				char *buf;
			} start, finish, end_of_storage;
			__data_node *next, *prev;
			
			explicit __data_node(const size_t &_capacity, __data_node *const &_prev = nullptr, __data_node *const &_next = nullptr):
					prev(_prev), next(_next)
			{
				start.buf = finish.buf = new char[sizeof(T) * _capacity];
				end_of_storage.val = start.val + _capacity;
			}
			
			__data_node(const __data_node &other, __data_node *const &_prev = nullptr, __data_node *const &_next = nullptr):
					prev(_prev), next(_next)
			{
				start.buf = new char[sizeof(T) * other.capacity()];
				size_t sz = other.size();
				for(size_t i = 0; i < sz; i++)
					new(start.val + i) T(other.start.val[i]);
				finish.val = start.val + sz;
				end_of_storage.val = start.val + other.capacity();
			}
			
			~__data_node()
			{
				for(pointer cur = start.val; cur != finish.val; cur++)
					cur->~T();
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

//			void insert(size_t ind, const T &value)
//			{
//				if(finish.val == end_of_storage.val)
//				{
//					__data_node *pre_next = next;
//					next = new __data_node(capacity(), this, pre_next);
//					pre_next->prev = next;
//					size_t half = (size() + 1) >> 1;
//					if(ind <= half)
//					{
//						next->finish.val = next->start.val + size() - half;
//						for(size_t i = size() - 1; i >= half; i--)
//						{
//							new(next->start.val + i - half) T(start.val[i]);
//							(start.val + i)->~T();
//						}
//						for(pointer p = start.val + half; p != start.val + ind; p--)
//						{
//							new(p) T(*(p - 1));
//							(p - 1)->~T();
//						}
//						new(start.val + ind)T(value);
//						finish.val = start.val + half + 1;
//					}
//					else
//					{
//						next->finish.val = next->start.val + size() - half + 1;
//						for(size_t i = next->size() - 1; i >= ind; i--)
//						{
//							new(next->start.val + i - half + 1) T(start.val[i]);
//							(start.val + half + i)->~T();
//						}
//						new(next->start.val + ind - half)T(value);
//						for(size_t i = ind - 1; i >= half; i--)
//						{
//							new(next->start.val + i - half) T(start.val[i]);
//							(start.val + i)->~T();
//						}
//					}
//				}
//				else
//				{
//					for(pointer p = start.val + size(); p != start.val + ind; p--)
//					{
//						new(p) T(*(p - 1));
//						(p - 1)->~T();
//					}
//					new(start.val + ind)T(value);
//					finish.val++;
//				}
//			}
//
//			void erase(size_t ind)
//			{
//				(start.val + ind)->~T();
//				for(pointer p = start.val + ind; p != finish.val; p++)
//				{
//					new(p) T(*(p + 1));
//					(p + 1)->~T();
//				}
//				finish.val--;
//				if(start.val == finish.val && prev != nullptr)
//				{
//					prev->next = next;
//					if(next != nullptr)
//						next->prev = prev;
//					__data_node *_prev = prev;
//					delete this;
//				}
//			}
			
			T &operator[](const size_t &ind)
			{
				if(ind >= size())
					throw index_out_of_bound();
				return start.val[ind];
			}
			
			const T &operator[](const size_t &ind) const
			{
				if(ind >= size())
					throw index_out_of_bound();
				return start.val[ind];
			}
			
			
		} *head = nullptr, *tail = nullptr;
		
		size_t __block_size, _size;
	
	public:
		class const_iterator;
		
		class iterator
		{
			friend class deque;
			
			friend class const_iterator;
		
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			deque *f = nullptr;
			__data_node *node = nullptr;
			size_t ind = 0;
			
			int _pos() const
			{
				int ret = 0;
				for(__data_node *cur = f->head; cur != node; cur = cur->next)
					ret += cur->size();
				return ret + ind;
			}
		
		public:
			iterator() = default;
			
			iterator(const iterator &other) = default;
			
			iterator(deque *const &_f, __data_node *const &_node, const size_t &_ind): f(_f), node(_node), ind(_ind)
			{
			}
			
			iterator &operator=(const iterator &other) = default;
			
			/**
			 * return a new iterator which pointer n-next elements
			 *   even if there are not enough elements, the behaviour is **undefined**.
			 * as well as operator-
			 */
			iterator operator+(const int &n) const
			{
				return iterator(*this) += n;
			}
			
			iterator operator-(const int &n) const
			{
				return iterator(*this) -= n;
			}
			
			// return the distance between two iterator,
			// if these two iterators points to different deques, throw invalid_iterator.
			int operator-(const iterator &rhs) const
			{
				//TODO
				if(f != rhs.f)
					throw invalid_iterator();
				return _pos() - rhs._pos();
			}
			
			iterator &operator+=(const int &n)
			{
				if(n < 0)
					return *this -= -n;
				int _n = n;
				while(node->size() - ind <= _n)
				{
					_n -= node->size() - ind;
					node = node->next;
					if(node == nullptr)
						return *this = f->end();
					ind = 0;
				}
				ind += _n;
				return *this;
			}
			
			iterator &operator-=(const int &n)
			{
				if(n < 0)
					return *this += -n;
				int _n = n;
				while(ind < _n)
				{
					if(node->prev == nullptr)
						throw runtime_error();
					_n -= ind + 1;
					node = node->prev;
					ind = node->size() - 1;
				}
				ind -= _n;
				return *this;
			}
			
			/**
			 * TODO iter++
			 */
			iterator operator++(int)
			{
				iterator ret(*this);
				*this += 1;
				return ret;
			}
			
			/**
			 * TODO ++iter
			 */
			iterator &operator++()
			{
				return *this += 1;
			}
			
			/**
			 * TODO iter--
			 */
			iterator operator--(int)
			{
				iterator ret(*this);
				*this -= 1;
				return ret;
			}
			
			/**
			 * TODO --iter
			 */
			iterator &operator--()
			{
				return *this -= 1;
			}
			
			/**
			 * TODO *it
			 */
			T &operator*() const
			{
				if(ind == node->size() && node->next != nullptr)
					return (*node->next)[0];
				return (*node)[ind];
			}
			
			/**
			 * TODO it->field
			 */
			pointer operator->() const noexcept
			{
				if(ind == node->size() && node->next != nullptr)
					return node->next->start.val;
				return node->start.val + ind;
			}
			
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const
			{
				return (node == rhs.node && ind == rhs.ind) || (node->next == rhs.node && ind == node->size() && rhs.ind == 0) || (rhs.node->next == node && rhs.ind == rhs.node->size() && ind == 0);
			}
			
			bool operator==(const const_iterator &rhs) const
			{
				return (node == rhs.node && ind == rhs.ind) || (node->next == rhs.node && ind == node->size() && rhs.ind == 0) || (rhs.node->next == node && rhs.ind == rhs.node->size() && ind == 0);
			}
			
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const
			{
				return !(*this == rhs);
			}
			
			bool operator!=(const const_iterator &rhs) const
			{
				return !(*this == rhs);
			}
		};
		
		class const_iterator
		{
			friend class deque;
		
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			deque *f = nullptr;
			__data_node *node = nullptr;
			size_t ind = 0;
			
			int _pos() const
			{
				int ret = 0;
				for(__data_node *cur = f->head; cur != node; cur = cur->next)
					ret += cur->size();
				return ret + ind;
			}
		
		public:
			const_iterator() = default;
			
			const_iterator(const const_iterator &other) = default;
			
			const_iterator(const iterator &other): f(other.f), node(other.node), ind(other.ind)
			{
			}
			
			const_iterator(const deque *const &_f, const __data_node *const &_node, const size_t &_ind):
					f(const_cast<deque *const &>(_f)), node(const_cast<__data_node *const &>(_node)), ind(_ind)
			{
			}
			
			const_iterator &operator=(const const_iterator &other) = default;
			
			/**
			 * return a new const_iterator which pointer n-next elements
			 *   even if there are not enough elements, the behaviour is **undefined**.
			 * as well as operator-
			 */
			const_iterator operator+(const int &n) const
			{
				return const_iterator(*this) += n;
			}
			
			const_iterator operator-(const int &n) const
			{
				return const_iterator(*this) -= n;
			}
			
			// return the distance between two const_iterator,
			// if these two iterators points to different deques, throw invalid_iterator.
			int operator-(const const_iterator &rhs) const
			{
				//TODO
				if(f != rhs.f)
					throw invalid_iterator();
				return this->_pos() - rhs._pos();
			}
			
			const_iterator &operator+=(const int &n)
			{
				if(n < 0)
					return *this -= -n;
				int _n = n;
				while(node->size() - ind <= _n)
				{
					_n -= node->size() - ind;
					node = node->next;
					if(node == nullptr)
						return *this = f->end();
					ind = 0;
				}
				ind += _n;
				return *this;
			}
			
			const_iterator &operator-=(const int &n)
			{
				if(n < 0)
					return *this += -n;
				int _n = n;
				while(ind < _n)
				{
					if(node->prev == nullptr)
						throw runtime_error();
					_n -= ind + 1;
					node = node->prev;
					ind = node->size() - 1;
				}
				ind -= _n;
				return *this;
			}
			
			/**
			 * TODO iter++
			 */
			const_iterator operator++(int)
			{
				const_iterator ret(*this);
				*this += 1;
				return ret;
			}
			
			/**
			 * TODO ++iter
			 */
			const_iterator &operator++()
			{
				return *this += 1;
			}
			
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int)
			{
				const_iterator ret(*this);
				*this -= 1;
				return ret;
			}
			
			/**
			 * TODO --iter
			 */
			const_iterator &operator--()
			{
				return *this -= 1;
			}
			
			/**
			 * TODO *it
			 */
			const T &operator*() const
			{
				if(ind == node->size() && node->next != nullptr)
					return (*node->next)[0];
				return (*node)[ind];
			}
			
			/**
			 * TODO it->field
			 */
			const_pointer operator->() const noexcept
			{
				if(ind == node->size() && node->next != nullptr)
					return node->next->start.val;
				return node->start.val + ind;
			}
			
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const
			{
				return (node == rhs.node && ind == rhs.ind) || (node->next == rhs.node && ind == node->size() && rhs.ind == 0) || (rhs.node->next == node && rhs.ind == rhs.node->size() && ind == 0);
			}
			
			bool operator==(const const_iterator &rhs) const
			{
				return (node == rhs.node && ind == rhs.ind) || (node->next == rhs.node && ind == node->size() && rhs.ind == 0) || (rhs.node->next == node && rhs.ind == rhs.node->size() && ind == 0);
			}
			
			/**
			 * some other operator for const_iterator.
			 */
			bool operator!=(const iterator &rhs) const
			{
				return !(*this == rhs);
			}
			
			bool operator!=(const const_iterator &rhs) const
			{
				return !(*this == rhs);
			}
		};
		
		/**
		 * TODO Constructors
		 */
		explicit deque(const size_t &___block_size = __DEFAULT_BLOCK_SIZE): __block_size(___block_size), _size(0)
		{
			head = tail = new __data_node(___block_size);
		}
		
		deque(const deque &other)
		{
			*this = other;
		}
		
		/**
		 * TODO Deconstructor
		 */
		~deque()
		{
			__data_node *cur = head, *next = head;
			while(cur != nullptr)
			{
				next = next->next;
				delete cur;
				cur = next;
			}
		}
		
		/**
		 * TODO assignment operator
		 */
		deque &operator=(const deque &other)
		{
			if(this == &other)
				return *this;
			__data_node *cur = head, *next = head;
			while(cur != nullptr)
			{
				next = next->next;
				delete cur;
				cur = next;
			}
			__block_size = other.__block_size;
			_size = other._size;
			head = new __data_node(*other.head);
			__data_node *ocur = other.head->next;
			cur = head;
			while(ocur != nullptr)
			{
				cur = cur->next = new __data_node(*ocur, cur);
				ocur = ocur->next;
			}
			tail = cur;
			return *this;
		}
		
		/**
		 * access specified element with bounds checking
		 * throw index_out_of_bound if out of bound.
		 */
		T &at(const size_t &pos)
		{
			return (*this)[pos];
		}
		
		const T &at(const size_t &pos) const
		{
			return (*this)[pos];
		}
		
		T &operator[](const size_t &pos)
		{
			if(pos >= _size)
				throw index_out_of_bound();
			__data_node *cur = head;
			size_t _pos = pos;
			while(cur->size() <= _pos)
			{
				_pos -= cur->size();
				cur = cur->next;
			}
			return (*cur)[_pos];
		}
		
		const T &operator[](const size_t &pos) const
		{
			if(pos >= _size)
				throw index_out_of_bound();
			__data_node *cur = head;
			size_t _pos = pos;
			while(cur->size() <= _pos)
			{
				_pos -= cur->size();
				cur = cur->next;
			}
			return (*cur)[_pos];
		}
		
		/**
		 * access the first element
		 * throw container_is_empty when the container is empty.
		 */
		const T &front() const
		{
			if(empty())
				throw container_is_empty();
			return (*head)[0];
		}
		
		/**
		 * access the last element
		 * throw container_is_empty when the container is empty.
		 */
		const T &back() const
		{
			if(empty())
				throw container_is_empty();
			return (*tail)[tail->size() - 1];
		}
		
		/**
		 * returns an iterator to the beginning.
		 */
		iterator begin()
		{
			return iterator(this, head, 0);
		}
		
		const_iterator cbegin() const
		{
			return const_iterator(this, head, 0);
		}
		
		/**
		 * returns an iterator to the end.
		 */
		iterator end()
		{
			return iterator(this, tail, tail->size());
		}
		
		const_iterator cend() const
		{
			return const_iterator(this, tail, tail->size());
		}
		
		/**
		 * checks whether the container is empty.
		 */
		bool empty() const
		{
			return _size == 0;
		}
		
		/**
		 * returns the number of elements
		 */
		size_t size() const
		{
			return _size;
		}
		
		/**
		 * clears the contents
		 */
		void clear()
		{
			__data_node *cur = head->next, *next = head->next;
			while(cur != nullptr)
			{
				next = next->next;
				delete cur;
				cur = next;
			}
			for(pointer p = head->start.val; p != head->finish.val; p++)
				p->~T();
			head->finish.val = head->start.val;
			head->next = nullptr;
			_size = 0;
			tail = head;
		}
		
		/**
		 * inserts elements at the specified locat on in the container.
		 * inserts value before pos
		 * returns an iterator pointing to the inserted value
		 *     throw if the iterator is invalid or it point to a wrong place.
		 */
		iterator insert(iterator pos, const T &value)
		{
			if(pos.f != this || pos.node == nullptr || pos.ind > pos.node->size())
				throw invalid_iterator();
			_size++;
			if(pos.node->finish.val == pos.node->end_of_storage.val)
			{
				__data_node *_next = pos.node->next;
				pos.node->next = new __data_node(pos.node->capacity(), pos.node, _next);
				if(_next != nullptr)
					_next->prev = pos.node->next;
				else
					tail = pos.node->next;
				size_t half = (pos.node->size() + 1) >> 1;
				if(pos.ind <= half)
				{
					pos.node->next->finish.val = pos.node->next->start.val + pos.node->size() - half;
					for(size_t i = pos.node->size() - 1; i >= half; i--)
					{
						new(pos.node->next->start.val + i - half) T(pos.node->start.val[i]);
						(pos.node->start.val + i)->~T();
					}
					for(pointer p = pos.node->start.val + half; p != pos.node->start.val + pos.ind; p--)
					{
						new(p) T(*(p - 1));
						(p - 1)->~T();
					}
					pos.node->finish.val = pos.node->start.val + half + 1;
					new(pos.node->start.val + pos.ind)T(value);
					return pos;
				}
				else
				{
					pos.node->next->finish.val = pos.node->next->start.val + pos.node->size() - half + 1;
					for(size_t i = pos.node->size() - 1; i >= pos.ind; i--)
					{
						new(pos.node->next->start.val + i - half + 1) T(pos.node->start.val[i]);
						(pos.node->start.val + i)->~T();
					}
					new(pos.node->next->start.val + pos.ind - half)T(value);
					for(size_t i = pos.ind - 1; i >= half; i--)
					{
						new(pos.node->next->start.val + i - half) T(pos.node->start.val[i]);
						(pos.node->start.val + i)->~T();
					}
					pos.node->finish.val = pos.node->start.val + half;
					return iterator(pos.f, pos.node->next, pos.ind - half);
				}
			}
			else
			{
				for(pointer p = pos.node->start.val + pos.node->size(); p != pos.node->start.val + pos.ind; p--)
				{
					new(p) T(*(p - 1));
					(p - 1)->~T();
				}
				pos.node->finish.val++;
				new(pos.node->start.val + pos.ind)T(value);
				return pos;
			}
		}
		
		/**
		 * removes specified element at pos.
		 * removes the element at pos.
		 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
		 */
		iterator erase(iterator pos)
		{
			if(pos.f != this || pos.node == nullptr || pos.ind > pos.node->size())
				throw invalid_iterator();
			if(pos.ind == pos.node->size())
				return erase(iterator(pos.f, pos.node->next, 0));
			_size--;
			(pos.node->start.val + pos.ind)->~T();
			for(pointer p = pos.node->start.val + pos.ind + 1; p != pos.node->finish.val; p++)
			{
				new(p - 1) T(*p);
				p->~T();
			}
			pos.node->finish.val--;
			if(pos.node->start.val == pos.node->finish.val)
			{
				if(pos.node->prev != nullptr)
				{
					pos.node->prev->next = pos.node->next;
					if(pos.node->next != nullptr)
						pos.node->next->prev = pos.node->prev;
					else
						tail = pos.node->prev;
					__data_node *_prev = pos.node->prev;
					delete pos.node;
					return iterator(pos.f, _prev, _prev->size());
				}
				else if(pos.node->next != nullptr)
				{
					pos.node->next->prev = nullptr;
					head = pos.node->next;
					delete pos.node;
					return iterator(pos.f, head, 0);
				}
			}
			return pos;
		}
		
		/**
		 * adds an element to the end
		 */
		void push_back(const T &value)
		{
			insert(iterator(this, tail, tail->size()), value);
		}
		
		/**
		 * removes the last element
		 *     throw when the container is empty.
		 */
		void pop_back()
		{
			if(_size == 0)
				throw container_is_empty();
			erase(iterator(this, tail, tail->size() - 1));
		}
		
		/**
		 * inserts an element to the beginning.
		 */
		void push_front(const T &value)
		{
			insert(iterator(this, head, 0), value);
		}
		
		/**
		 * removes the first element.
		 *     throw when the container is empty.
		 */
		void pop_front()
		{
			if(size() == 0)
				throw container_is_empty();
			erase(iterator(this, head, 0));
		}
	};
	
}

#endif
