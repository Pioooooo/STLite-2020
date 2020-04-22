/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu
{
	
	template<class Key, class T, class Compare = std::less<Key> >
	class map
	{
	public:
		/**
		 * the internal type of data.
		 * it should have a default constructor, a copy constructor.
		 * You can use sjtu::map as value_type by typedef.
		 */
		typedef pair<const Key, T> value_type;
	
	private:
		size_t _size;
		Compare _cmp;
		
		enum _color_type
		{
			RED = 0,
			BLACK = 1
		};
		
		struct _node
		{
			value_type *val;
			_color_type col;
			_node *ch[2], *p;
			
			_node(const value_type &_val, const _color_type &_col = RED, _node *const _p = nullptr, _node *const _lc = nullptr, _node *const _rc = nullptr):
					val(new value_type(_val)), col(_col), ch({_lc, _rc}), p(_p)
			{
			}
			
			_node(const _node &other, _node *const _p = nullptr, _node *const _lc = nullptr, _node *const _rc = nullptr):
					val(new value_type(*other.val)), col(other.col), ch({_lc, _rc}), p(_p)
			{
			}
			
			~_node()
			{
				if(val != nullptr)
					delete val;
			}
			
			_node *grandparent() const
			{
				if(p == nullptr)
					return nullptr;
				return p->p;
			}
			
			size_t _pos() const
			{
				return p->ch[1] == this;
			}
			
			_node *sibling() const
			{
				if(p == nullptr)
					return nullptr;
				return p->ch[!_pos()];
			}
			
			_node *uncle() const
			{
				if(p == nullptr)
					return nullptr;
				return p->sibling();
			}
		} *root;
		
		bool _is_equal(const Key &a, const Key &b) const
		{
			return !(_cmp(a, b) || _cmp(b, a));
		}
		
		void _swap(_node *a, _node *b)
		{
			std::swap(a->col, b->col);
			if(b->p == a)
				std::swap(a, b);
			if(a->p == b)
			{
				size_t da = a->_pos();
				if(b->p == nullptr)
					root = a;
				else
					b->p->ch[b->_pos()] = a;
				std::swap(a->ch[!da], b->ch[!da]);
				b->ch[da] = a->ch[da];
				a->ch[da] = b;
				a->p = b->p;
				b->p = a;
				for(size_t i = 0; i < 2; i++)
				{
					if(a->ch[i] != nullptr)
						a->ch[i]->p = a;
					if(b->ch[i] != nullptr)
						b->ch[i]->p = b;
				}
				return;
			}
			std::swap(a->p, b->p);
			if(a->p != nullptr)
				a->p->ch[a->p->ch[1] == b] = a;
			else
				root = a;
			if(b->p != nullptr)
				b->p->ch[b->p->ch[1] == a] = b;
			else
				root = b;
			for(size_t i = 0; i < 2; i++)
			{
				std::swap(a->ch[i], b->ch[i]);
				if(a->ch[i] != nullptr)
					a->ch[i]->p = a;
				if(b->ch[i] != nullptr)
					b->ch[i]->p = b;
			}
		}
		
		void _rotate(_node *p)
		{
			_node *gp = p->grandparent(), *fa = p->p;
			size_t posp = p->_pos();
			if(gp != nullptr)
				gp->ch[fa->_pos()] = p;
			else
				root = p;
			p->p = gp;
			fa->ch[posp] = p->ch[!posp];
			if(p->ch[!posp] != nullptr)
				p->ch[!posp]->p = fa;
			p->ch[!posp] = fa;
			fa->p = p;
		}
		
		void _insert_adjust(_node *p)
		{
			if(p->p == nullptr)
			{
				p->col = BLACK;
				root = p;
				return;
			}
			if(p->p->col == BLACK)
				return;
			if(p->uncle() != nullptr && p->uncle()->col == RED)
			{
				p->p->col = p->uncle()->col = BLACK;
				p->grandparent()->col = RED;
				_insert_adjust(p->grandparent());
				return;
			}
			if(p->_pos() != p->p->_pos())
			{
				_node *tmp = p->p;
				_rotate(p);
				p = tmp;
			}
			p->grandparent()->col = RED;
			p->p->col = BLACK;
			_rotate(p->p);
		}
		
		void _erase_adjust(_node *p)
		{
			if(p->p == nullptr)
			{
				root = p;
				return;
			}
			_node *s = p->sibling(), *fa = p->p;
			if(s->col == RED)
			{
				fa->col = RED;
				s->col = BLACK;
				_rotate(s);
			}
			s = p->sibling();
			if(fa->col == BLACK && (s->ch[0] == nullptr || s->ch[0]->col == BLACK) && (s->ch[1] == nullptr || s->ch[1]->col == BLACK))
			{
				s->col = RED;
				_erase_adjust(fa);
				return;
			}
			if(fa->col == RED && (s->ch[0] == nullptr || s->ch[0]->col == BLACK) && (s->ch[1] == nullptr || s->ch[1]->col == BLACK))
			{
				s->col = RED;
				fa->col = BLACK;
				return;
			}
			size_t d = p->_pos();
			if(s->ch[d] != nullptr && s->ch[d]->col == RED && (s->ch[!d] == nullptr || s->ch[!d]->col == BLACK))
			{
				s->col = RED;
				s->ch[d]->col = BLACK;
				_rotate(s->ch[d]);
			}
			s = p->sibling();
			s->col = fa->col;
			fa->col = BLACK;
			s->ch[!d]->col = BLACK;
			_rotate(s);
		}
		
		void _clear(_node *&p)
		{
			for(_node *ch:p->ch)
				if(ch != nullptr)
					_clear(ch);
			delete p;
			p = nullptr;
		}
		
		void _copy(_node *&p, const _node *const &other)
		{
			if(other == nullptr)
				return;
			p = new _node(*other);
			for(size_t i = 0; i < 2; i++)
			{
				_copy(p->ch[i], other->ch[i]);
				if(p->ch[i] != nullptr)
					p->ch[i]->p = p;
			}
		}
	
	public:
		/**
		 * see BidirectionalIterator at CppReference for help.
		 *
		 * if there is anything wrong throw invalid_iterator.
		 *     like it = map.begin(); --it;
		 *       or it = map.end(); ++end();
		 */
		class const_iterator;
		
		class iterator
		{
			friend class map;
		
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			_node *dat;
			map *f;
			
			iterator(_node *const &p, map *const &_f): dat(p), f(_f)
			{
			}
		
		public:
			iterator(): dat(nullptr), f(nullptr)
			{
				// TODO
			}
			
			iterator(const iterator &other): dat(other.dat), f(other.f)
			{
				// TODO
			}
			
			iterator(const const_iterator &other): dat(other.dat), f(other.f)
			{
				// TODO
			}
			/**
			 * return a new iterator which pointer n-next elements
			 *   even if there are not enough elements, just return the answer.
			 * as well as operator-
			 */
			/**
			 * TODO iter++
			 */
			iterator operator++(int)
			{
				if(dat == nullptr)
					throw invalid_iterator();
				iterator tmp(*this);
				if(dat->ch[1] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(!p->_pos())
						{
							dat = p->p;
							return tmp;
						}
						p = p->p;
					}
					dat = nullptr;
					return tmp;
				}
				_node *p = dat->ch[1];
				while(p->ch[0] != nullptr)
					p = p->ch[0];
				dat = p;
				return tmp;
			}
			
			/**
			 * TODO ++iter
			 */
			iterator &operator++()
			{
				if(dat == nullptr)
					throw invalid_iterator();
				if(dat->ch[1] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(!p->_pos())
						{
							dat = p->p;
							return *this;
						}
						p = p->p;
					}
					dat = nullptr;
					return *this;
				}
				_node *p = dat->ch[1];
				while(p->ch[0] != nullptr)
					p = p->ch[0];
				dat = p;
				return *this;
			}
			
			/**
			 * TODO iter--
			 */
			iterator operator--(int)
			{
				iterator tmp(*this);
				if(dat == nullptr)
				{
					dat = f->root;
					if(dat != nullptr)
					{
						while(dat->ch[1] != nullptr)
							dat = dat->ch[1];
					}
					else
						throw invalid_iterator();
					return tmp;
				}
				if(dat->ch[0] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(p->_pos())
						{
							dat = p->p;
							return tmp;
						}
						p = p->p;
					}
					throw invalid_iterator();
				}
				_node *p = dat->ch[0];
				while(p->ch[1] != nullptr)
					p = p->ch[1];
				dat = p;
				return tmp;
			}
			
			/**
			 * TODO --iter
			 */
			iterator &operator--()
			{
				if(dat == nullptr)
				{
					dat = f->root;
					if(dat != nullptr)
					{
						while(dat->ch[1] != nullptr)
							dat = dat->ch[1];
					}
					else
						throw invalid_iterator();
					return *this;
				}
				if(dat->ch[0] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(p->_pos())
						{
							dat = p->p;
							return *this;
						}
						p = p->p;
					}
					throw invalid_iterator();
				}
				_node *p = dat->ch[0];
				while(p->ch[1] != nullptr)
					p = p->ch[1];
				dat = p;
				return *this;
			}
			
			value_type &operator*() const
			{
				return *dat->val;
			}
			
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const
			{
				return dat == rhs.dat && f == rhs.f;
			}
			
			bool operator==(const const_iterator &rhs) const
			{
				return dat == rhs.dat && f == rhs.f;
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
			
			/**
			 * for the support of it->first.
			 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			 */
			value_type *operator->() const noexcept
			{
				return dat->val;
			}
		};
		
		class const_iterator
		{
			friend class map;
		
		private:
			/**
			 * TODO add data members
			 *   just add whatever you want.
			 */
			_node *dat;
			const map *f;
			
			const_iterator(_node *const &p, const map *const &_f): dat(p), f(_f)
			{
			}
		
		public:
			const_iterator(): dat(nullptr), f(nullptr)
			{
				// TODO
			}
			
			const_iterator(const const_iterator &other): dat(other.dat), f(other.f)
			{
				// TODO
			}
			
			const_iterator(const iterator &other): dat(other.dat), f(other.f)
			{
				// TODO
			}
			/**
			 * return a new const_iterator which pointer n-next elements
			 *   even if there are not enough elements, just return the answer.
			 * as well as operator-
			 */
			/**
			 * TODO iter++
			 */
			const_iterator operator++(int)
			{
				if(dat == nullptr)
					throw invalid_iterator();
				const_iterator tmp(*this);
				if(dat->ch[1] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(!p->_pos())
						{
							dat = p->p;
							return tmp;
						}
						p = p->p;
					}
					dat = nullptr;
					return tmp;
				}
				_node *p = dat->ch[1];
				while(p->ch[0] != nullptr)
					p = p->ch[0];
				dat = p;
				return tmp;
			}
			
			/**
			 * TODO ++iter
			 */
			const_iterator &operator++()
			{
				if(dat == nullptr)
					throw invalid_iterator();
				if(dat->ch[1] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(!p->_pos())
						{
							dat = p->p;
							return *this;
						}
						p = p->p;
					}
					dat = nullptr;
					return *this;
				}
				_node *p = dat->ch[1];
				while(p->ch[0] != nullptr)
					p = p->ch[0];
				dat = p;
				return *this;
			}
			
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int)
			{
				const_iterator tmp(*this);
				if(dat == nullptr)
				{
					dat = f->root;
					if(dat != nullptr)
					{
						while(dat->ch[1] != nullptr)
							dat = dat->ch[1];
					}
					else
						throw invalid_iterator();
					return tmp;
				}
				if(dat->ch[0] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(p->_pos())
						{
							dat = p->p;
							return tmp;
						}
						p = p->p;
					}
					throw invalid_iterator();
				}
				_node *p = dat->ch[0];
				while(p->ch[1] != nullptr)
					p = p->ch[1];
				dat = p;
				return tmp;
			}
			
			/**
			 * TODO --iter
			 */
			const_iterator &operator--()
			{
				if(dat == nullptr)
				{
					dat = f->root;
					if(dat != nullptr)
					{
						while(dat->ch[1] != nullptr)
							dat = dat->ch[1];
					}
					else
						throw invalid_iterator();
					return *this;
				}
				if(dat->ch[0] == nullptr)
				{
					_node *p = dat;
					while(p->p != nullptr)
					{
						if(p->_pos())
						{
							dat = p->p;
							return *this;
						}
						p = p->p;
					}
					throw invalid_iterator();
				}
				_node *p = dat->ch[0];
				while(p->ch[1] != nullptr)
					p = p->ch[1];
				dat = p;
				return *this;
			}
			
			const value_type &operator*() const
			{
				return *dat->val;
			}
			
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const
			{
				return dat == rhs.dat && f == rhs.f;
			}
			
			bool operator==(const const_iterator &rhs) const
			{
				return dat == rhs.dat && f == rhs.f;
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
			
			/**
			 * for the support of it->first.
			 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			 */
			const value_type *operator->() const noexcept
			{
				return dat->val;
			}
		};
		
		/**
		 * TODO two constructors
		 */
		map(): root(nullptr), _size(0)
		{
		}
		
		map(const map &other): root(nullptr)
		{
			*this = other;
		}
		
		/**
		 * TODO assignment operator
		 */
		map &operator=(const map &other)
		{
			if(this == &other)
				return *this;
			clear();
			_size = other._size;
			_copy(root, other.root);
			return *this;
		}
		
		/**
		 * TODO Destructors
		 */
		~map()
		{
			clear();
		}
		
		/**
		 * TODO
		 * access specified element with bounds checking
		 * Returns a reference to the mapped value of the element with key equivalent to key.
		 * If no such element exists, an exception of type `index_out_of_bound'
		 */
		T &at(const Key &key)
		{
			if(root != nullptr && _is_equal(root->val->first, key))
				return root->val->second;
			_node *cur = root;
			while(cur != nullptr)
			{
				if(_cmp(key, cur->val->first))
				{
					cur = cur->ch[0];
					continue;
				}
				if(_cmp(cur->val->first, key))
				{
					cur = cur->ch[1];
					continue;
				}
				return cur->val->second;
			}
			throw index_out_of_bound();
		}
		
		const T &at(const Key &key) const
		{
			if(root != nullptr && _is_equal(root->val->first, key))
				return root->val->second;
			_node *cur = root;
			while(cur != nullptr)
			{
				if(_cmp(key, cur->val->first))
				{
					cur = cur->ch[0];
					continue;
				}
				if(_cmp(cur->val->first, key))
				{
					cur = cur->ch[1];
					continue;
				}
				return cur->val->second;
			}
			throw index_out_of_bound();
		}
		
		/**
		 * TODO
		 * access specified element
		 * Returns a reference to the value that is mapped to a key equivalent to key,
		 *   performing an insertion if such key does not already exist.
		 */
		T &operator[](const Key &key)
		{
			return insert({key, T()}).first->second;
		}
		
		/**
		 * behave like at() throw index_out_of_bound if such key does not exist.
		 */
		const T &operator[](const Key &key) const
		{
			return at(key);
		}
		
		/**
		 * return a iterator to the beginning
		 */
		iterator begin()
		{
			if(root == nullptr)
				return end();
			_node *cur = root;
			while(cur->ch[0] != nullptr)
				cur = cur->ch[0];
			return iterator(cur, this);
		}
		
		const_iterator cbegin() const
		{
			if(root == nullptr)
				return cend();
			_node *cur = root;
			while(cur->ch[0] != nullptr)
				cur = cur->ch[0];
			return const_iterator(cur, this);
		}
		
		/**
		 * return a iterator to the end
		 * in fact, it returns past-the-end.
		 */
		iterator end()
		{
			return iterator(nullptr, this);
		}
		
		const_iterator cend() const
		{
			return const_iterator(nullptr, this);
		}
		
		/**
		 * checks whether the container is empty
		 * return true if empty, otherwise false.
		 */
		bool empty() const
		{
			return !_size;
		}
		
		/**
		 * returns the number of elements.
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
			_size = 0;
			if(root != nullptr)
				_clear(root);
		}
		
		/**
		 * insert an element.
		 * return a pair, the first of the pair is
		 *   the iterator to the new element (or the element that prevented the insertion),
		 *   the second one is true if insert successfully, or false.
		 */
		pair<iterator, bool> insert(const value_type &value)
		{
			if(root == nullptr)
			{
				root = new _node(value, BLACK);
				_size++;
				return {iterator(root, this), true};
			}
			_node *cur = root;
			while(true)
			{
				if(_cmp(value.first, cur->val->first))
				{
					if(cur->ch[0] == nullptr)
					{
						cur = cur->ch[0] = new _node(value, RED, cur);
						break;
					}
					cur = cur->ch[0];
					continue;
				}
				if(_cmp(cur->val->first, value.first))
				{
					if(cur->ch[1] == nullptr)
					{
						cur = cur->ch[1] = new _node(value, RED, cur);
						break;
					}
					cur = cur->ch[1];
					continue;
				}
				return {iterator(cur, this), false};
			}
			_insert_adjust(cur);
			_size++;
			return {iterator(cur, this), true};
		}
		
		/**
		 * erase the element at pos.
		 *
		 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		 */
		void erase(iterator pos)
		{
			if(pos.dat == nullptr || pos.f != this)
				throw invalid_iterator();
			_size--;
			_node *cur, *ch;
			if(pos.dat->ch[1] == nullptr)
			{
				cur = pos.dat;
				ch = cur->ch[0];
			}
			else
			{
				cur = pos.dat->ch[1];
				while(cur->ch[0] != nullptr)
					cur = cur->ch[0];
				_swap(cur, pos.dat);
				cur = pos.dat;
				ch = cur->ch[1];
			}
			if(cur->p == nullptr)
			{
				root = ch;
				if(ch != nullptr)
				{
					ch->col = BLACK;
					ch->p = nullptr;
				}
				delete cur;
				return;
			}
			if(ch != nullptr)
			{
				ch->p = cur->p;
				cur->p->ch[cur->_pos()] = ch;
			}
			if(cur->col == BLACK)
				if(ch != nullptr)
				{
					if(ch->col == RED)
						ch->col = BLACK;
					else
						_erase_adjust(ch);
				}
				else
					_erase_adjust(cur);
			if(ch == nullptr)
				cur->p->ch[cur->_pos()] = nullptr;
			delete cur;
		}
		
		/**
		 * Returns the number of elements with key
		 *   that compares equivalent to the specified argument,
		 *   which is either 1 or 0
		 *     since this container does not allow duplicates.
		 * The default method of check the equivalence is !(a < b || b > a)
		 */
		size_t count(const Key &key) const
		{
			return find(key) != cend();
		}
		
		/**
		 * Finds an element with key equivalent to key.
		 * key value of the element to search for.
		 * Iterator to an element with key equivalent to key.
		 *   If no such element is found, past-the-end (see end()) iterator is returned.
		 */
		iterator find(const Key &key)
		{
			_node *cur = root;
			while(cur != nullptr)
			{
				if(_cmp(key, cur->val->first))
				{
					cur = cur->ch[0];
					continue;
				}
				if(_cmp(cur->val->first, key))
				{
					cur = cur->ch[1];
					continue;
				}
				return iterator(cur, this);
			}
			return end();
		}
		
		const_iterator find(const Key &key) const
		{
			_node *cur = root;
			while(cur != nullptr)
			{
				if(_cmp(key, cur->val->first))
				{
					cur = cur->ch[0];
					continue;
				}
				if(_cmp(cur->val->first, key))
				{
					cur = cur->ch[1];
					continue;
				}
				return const_iterator(cur, this);
			}
			return cend();
		}
	};
	
}

#endif
