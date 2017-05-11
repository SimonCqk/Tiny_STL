#pragma once
#ifndef MEMORY_H
#define MEMORY_H
#include<functional>

//include default deleter , unique_ptr and shared_ptr

namespace Tiny_STL {
	//default deleter
	template<typename T>
	class default_delete
	{
	public:
		void operator()(T* t)const {
			if (t) {
				delete t;
			}
		}
	};
	template<typename T>
	class default_delete<T[]>
	{
	public:
		void operator()(T* t)const {
			if (t) {
				delete[] t;
			}
		}
	};

	//declaration of unique_ptr and .swap()
	template<typename, typename> class unique_ptr;
	template<typename T, typename D>
	void swap(unique_ptr<T, D>& lhs, unique_ptr<T, D>& rhs);

	template <typename T, typename... Args>
	unique_ptr<T> make_unique(Args&&... args) {
		return unique_ptr<T>(new T(std::forward<Args>(args)...));
	};


	template <typename T, typename D = default_delete<T>>
	class unique_ptr
	{
		friend void swap<T, D>(unique_ptr<T, D>& lhs, unique_ptr<T, D>& rhs);

	public:
		// preventing copy and assignment
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator = (const unique_ptr&) = delete;

		// default constructor and one taking T*
		unique_ptr() = default;
		explicit unique_ptr(T* up) : ptr(up) { }

		// move constructor
		unique_ptr(unique_ptr&& up) noexcept
			: ptr(up.ptr) {
			up.ptr = nullptr;
		}

		// move assignment
		unique_ptr& operator =(unique_ptr&& rhs) noexcept;

		// std::nullptr_t assignment
		unique_ptr& operator =(std::nullptr_t n) noexcept;



		// operator overloaded :  *  ->  bool
		T& operator  *() const { return *ptr; }
		T* operator ->() const { return &this->operator *(); }
		operator bool() const { return ptr ? true : false; }

		// return the underlying pointer
		T* get() const noexcept { return ptr; }

		// swap member using swap friend
		void swap(unique_ptr<T, D> &rhs) { ::swap(*this, rhs); }

		// free and make it point to nullptr or to p's pointee.
		void reset()     noexcept { deleter(ptr); ptr = nullptr; }
		void reset(T* p) noexcept { deleter(ptr); ptr = p; }

		// return ptr and make ptr point to nullptr.
		T* release();

		D&  get_deleter() { return deleter; }

		~unique_ptr()
		{
			deleter(ptr);
		}
	private:
		T* ptr = nullptr;
		D  deleter = D();
	};


	// swap
	template<typename T, typename D>
	inline void
		swap(unique_ptr<T, D>& lhs, unique_ptr<T, D>& rhs)
	{
		using std::swap;
		swap(lhs.ptr, rhs.ptr);
		swap(lhs.deleter, rhs.deleter);
	}

	// move assignment
	template<typename T, typename D>
	inline unique_ptr<T, D>&
		unique_ptr<T, D>::operator =(unique_ptr&& rhs) noexcept
	{
		// prevent self-assignment
		if (this->ptr != rhs.ptr)
		{
			deleter(ptr);
			ptr = nullptr;
			::swap(*this, rhs);
		}
		return *this;
	}


	// std::nullptr_t assignment
	template<typename T, typename D>
	inline unique_ptr<T, D>&
		unique_ptr<T, D>::operator =(std::nullptr_t n) noexcept
	{
		if (n == nullptr)
		{
			deleter(ptr);
			ptr = nullptr;
		}
		return *this;
	}

	// relinquish contrul by returnning ptr and making ptr point to nullptr.
	template<typename T, typename D>
	inline T*
		unique_ptr<T, D>::release()
	{
		T* ret = ptr;
		ptr = nullptr;
		return ret;
	}

}


template<typename T>
class shared_ptr;

template<typename T>
auto swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs)
{
	using std::swap;
	swap(lhs.ptr, rhs.ptr);
	swap(lhs.ref_count, rhs.ref_count);
	swap(lhs.deleter, rhs.deleter);
}

template<typename T>
class shared_ptr
{
public:
	//
	//  Default Ctor
	//
	shared_ptr()
		: ptr{ nullptr }, ref_count{ new std::size_t(1) }, deleter{ Tiny_STL::default_delete<T>() }
	{ }
	//
	//  Ctor that takes raw pointer
	//
	explicit shared_ptr(T* raw_ptr)
		: ptr{ raw_ptr }, ref_count{ new std::size_t(1) }, deleter{ Tiny_STL::default_delete<T>() }
	{ }
	//
	//  Copy Ctor
	//
	shared_ptr(shared_ptr const& other)
		: ptr{ other.ptr }, ref_count{ other.ref_count }, deleter{ other.deleter }
	{
		++*ref_count;
	}
	//
	//  Move Ctor
	//
	shared_ptr(shared_ptr && other) noexcept
		: ptr{ other.ptr }, ref_count{ other.ref_count }, deleter{ std::move(other.deleter) }
	{
		other.ptr = nullptr;
		other.ref_count = nullptr;
	}
	//
	//  Copy assignment
	//
	shared_ptr& operator=(shared_ptr const& rhs)
	{
		//increment first to ensure safty for self-assignment
		++*rhs.ref_count;
		decrement_and_destroy();
		ptr = rhs.ptr, ref_count = rhs.ref_count, deleter = rhs.deleter;
		return *this;
	}
	//
	//  Move assignment
	//
	shared_ptr& operator=(shared_ptr && rhs) noexcept
	{
		swap(*this, rhs);
		rhs.decrement_and_destroy();
		return *this;
	}
	//
	//  Conversion operator
	//
	operator bool() const
	{
		return ptr ? true : false;
	}
	//
	//  Dereference
	//
	T& operator* () const
	{
		return *ptr;
	}
	//
	//  Arrow
	//
	T* operator->() const
	{
		return &*ptr;
	}
	//
	//  Use count
	//
	auto use_count() const
	{
		return *ref_count;
	}
	//
	//  Get underlying pointer
	//
	auto get() const
	{
		return ptr;
	}
	//
	//  Check if the unique user
	//
	auto unique() const
	{
		return 1 == *refCount;
	}
	//
	//  Swap
	//
	void swap(shared_ptr& rhs)
	{
		Tiny_STL::swap(*this, rhs);
	}
	//
	// Free the object pointed to, if unique
	//
	auto reset()
	{
		decrement_and_destroy();
	}
	//
	// Reset with the new raw pointer
	//
	auto reset(T* pointer)
	{
		if (ptr != pointer)
		{
			decrement_n_destroy();
			ptr = pointer;
			ref_count = new std::size_t(1);
		}
	}
	//
	//  Reset with raw pointer and deleter
	//
	auto reset(T *pointer, const std::function<void(T*)>& d)
	{
		reset(pointer);
		deleter = d;
	}
	//
	//  Dtor
	//
	~shared_ptr()
	{
		decrement_and_destroy();
	}
private:
	T* ptr;
	size_t* ref_count;
	std::function<void(T*)> deleter;

	auto decrement_and_destroy()
	{
		if (ptr && 0 == --*ref_count) {
			delete ref_count,
				deleter(ptr);
		}
		else if (!ptr)
			delete ref_count;
		ref_count = nullptr;
		ptr = nullptr;
	}
};

template<typename T, typename...Args>
shared_ptr<T> make_shared(Args&&... args) {
	return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif // !MEMORY_H
