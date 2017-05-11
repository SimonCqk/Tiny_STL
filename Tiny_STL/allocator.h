#pragma once
#ifndef TINYSTL_ALLOCATOR_H
#define TINYSTL_ALLOCATOR_H
#include"alloc.h"

namespace Tiny_STL {
	//allocator模板类
	//两个参数：对象类型和空间配置器的类型
	//allocate:对象内存的分配，重载了两个版本
	//deallocate:对象内存的销毁，同样重载了两个版本
	//constructor:用于在分配好的内存上构造实例化的对象
	//destroy:销毁实例化对象
	template<typename T>
	class allocator {
	public:
		typedef T			value_type;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		typedef T&			reference;
		typedef const T&     const_reference;
		typedef size_t       size_type;
		typedef ptrdiff_t    difference_type;
	public:
		static T* allocate();
		static T* allocate(size_t n);
		static bool deallocate(T* ptr);
		static bool deallocate(T* ptr, size_t n);
		static void construct(T *ptr);
		static void construct(T *ptr, T&& value);
		static void destroy(T *ptr);
		static void destroy(T *first, T *last);
	};

	//无参版本，返回内存的起始位置
	template<typename T>
	inline T* allocator<T>::allocate() {
		return static_cast<T*>(alloc::allocate(sizeof(T)));
	}
	//指定了需要分配的对象的数量
	template<typename T>
	inline T* allocator<T>::allocate(size_t n) {
		if (n == 0)return nullptr;
		return static_cast<T *>(alloc::allocate(sizeof(T) * n));
	}
	//接受一个指针，释放其内存，成功则返回true,否则false
	template<typename T>
	inline bool allocator<T>::deallocate(T* ptr) {
		if (ptr == nullptr)return false;
		alloc::deallocate(static_cast<void *>(ptr), sizeof(T));
		return true;
	}
	
	//指定需要释放的对象的数量
	template<typename T>
	inline bool allocator<T>::deallocate(T* ptr , size_t n) {
		if (ptr == nullptr)return false;
		alloc::deallocate(static_cast<void *>(ptr), sizeof(T)*n);
		return true;
	}
	//在内存上构造对象
	template<typename T>
	inline void allocator<T>::construct(T *ptr) {
		new(static_cast<void *>(ptr))T();
	}
	//并且接受任意类型(universal ref)
	template<typename T>
	void allocator<T>::construct(T *ptr,T&& value) {
		new(static_cast<void *>(ptr))T(std::forward<T>(value));
	}
	template<typename T>
	inline void allocator<T>::destroy(T *ptr) {
		ptr->~T();
	}
	//接受一对指针（迭代器）版本
	template<typename T>
	inline void allocator<T>::destroy(T *first, T *last) {
		for (; first != last; ++first) {
			first->~T();
		}
	}
}
#endif // !TINYSTL_ALLOCATOR_H
