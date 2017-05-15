#pragma once
#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H
//This head file design for the basic application of iterator


namespace Tiny_STL {

	// 5 basic iterator types
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// input_iterator
	template <class T, class Distance>
	struct input_iterator
	{
		typedef input_iterator_tag          iterator_category;
		typedef T                           value_type;
		typedef Distance                    difference_type;
		typedef T*                          pointer;
		typedef T&                          reference;
	};

	// output_iterator
	template <class T, class Distance>
	struct output_iterator
	{
		typedef output_iterator_tag         iterator_category;
		typedef T                           value_type;
		typedef Distance                    difference_type;
		typedef T*                          pointer;
		typedef T&                          reference;
	};

	// forward_iterator
	template <class T, class Distance>
	struct forward_iterator
	{
		typedef forward_iterator_tag        iterator_category;
		typedef T                           value_type;
		typedef Distance                    difference_type;
		typedef T*                          pointer;
		typedef T&                          reference;
	};

	// bidirectional_iterator
	template <class T, class Distance>
	struct bidirectional_iterator
	{
		typedef bidirectional_iterator_tag  iterator_category;
		typedef T                           value_type;
		typedef Distance                    difference_type;
		typedef T*                          pointer;
		typedef T&                          reference;
	};

	// random_access_iterator
	template <class T, class Distance>
	struct random_access_iterator
	{
		typedef random_access_iterator_tag  iterator_category;
		typedef T                           value_type;
		typedef Distance                    difference_type;
		typedef T*                          pointer;
		typedef T&                          reference;
	};

}

#endif // !TINYSTL_ITERATOR_H
