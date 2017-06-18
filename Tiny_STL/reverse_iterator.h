#pragma once
#ifndef REVERSE_ITERATOR_H
#define REVERSE_ITERATOR_H
#include"iterator.h"
//defination of reverse iterator

namespace Tiny_STL {

	template <class Iterator>
	class reverse_iterator
	{
	private:
		Iterator current;  // record the normal iterator

	public:
		typedef typename iterator_traits<Iterator>::iterator_category    iterator_category;
		typedef typename iterator_traits<Iterator>::value_type           value_type;
		typedef typename iterator_traits<Iterator>::difference_type      difference_type;
		typedef typename iterator_traits<Iterator>::pointer              pointer;
		typedef typename iterator_traits<Iterator>::reference            reference;

		typedef Iterator                                                 iterator_type;
		typedef reverse_iterator<Iterator>                               self;

	public:
		//constructors
		reverse_iterator() {}
		explicit reverse_iterator(iterator_type it) :current(it) {}
		reverse_iterator(const self& rhs) :current(rhs.current) {}

	public:
		// extract the normal iterator
		iterator_type base()  const { return current; }

		reference operator*() const
		{  // corresponding to the former position of iterator
			auto tmp = current;
			return *--tmp;
		}
		pointer operator->() const
		{
			return &(operator*());
		}

		// change "++" to "--"
		self& operator++()
		{
			--current;
			return *this;
		}
		self operator++(int)
		{
			self tmp = *this;
			--current;
			return tmp;
		}
		// change "--" to "++"
		self& operator--()
		{
			++current;
			return *this;
		}
		self operator--(int)
		{
			self tmp = *this;
			++current;
			return tmp;
		}

		self& operator+=(difference_type n)
		{
			current -= n;
			return *this;
		}
		self operator+(difference_type n) const
		{
			return self(current - n);
		}
		self& operator-=(difference_type n)
		{
			current += n;
			return *this;
		}
		self operator-(difference_type n) const
		{
			return self(current + n);
		}

		reference operator[](difference_type n) const
		{
			return *(*this + n);
		}
	};


	//overloading the operations
	template <class Iterator>
	inline typename reverse_iterator<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs,
			const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() - lhs.base();
	}


	template <class Iterator>
	inline bool operator==(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return lhs.base() == rhs.base();
	}

	template <class Iterator>
	inline bool operator<(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() < lhs.base();
	}

	template <class Iterator>
	inline bool operator!=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Iterator>
	inline bool operator>(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return rhs < lhs;
	}

	template <class Iterator>
	inline bool operator<=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Iterator>
	inline bool operator>=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs < rhs);
	}

}


#endif // !REVERSE_ITERATOR_H
