#pragma once
#include <utility>

template <typename T>
struct my_linked_ptr
{
	my_linked_ptr() = delete;

	my_linked_ptr(T* p) noexcept 
		: ptr(p)
		, prev(nullptr)
		, next(nullptr)
	{}

	my_linked_ptr(my_linked_ptr const& other) noexcept
		: ptr(other.ptr)
		, prev(&other)
		, next(other.next)
	{
		connect(*this);
	}

	my_linked_ptr(my_linked_ptr&& other) noexcept 
		: ptr(other.ptr)
		, prev(other.prev)
		, next(other.next)
	{
		other.prev = other.next = nullptr;
		other.ptr = nullptr;
		connect(*this);
	}

	my_linked_ptr& operator=(my_linked_ptr const& other) noexcept
	{
		my_linked_ptr tmp(other);
		swap(tmp);
		return *this;
	}

	my_linked_ptr& operator=(my_linked_ptr&& other) noexcept
	{
		swap(other);
		return *this;
	}

	T* get() const noexcept
	{
		return ptr;
	}

	explicit operator bool() const noexcept
	{
		return (ptr != nullptr);
	}

	void swap(my_linked_ptr& other) noexcept
	{
		if (ptr == other.ptr)
		{
			return;
		}
		using std::swap;
		swap(ptr, other.ptr);
		swap(prev, other.prev);
		swap(next, other.next);
		connect(*this);
		connect(other);
	}

	~my_linked_ptr() noexcept
	{
		clear();
	}
private:
	T* ptr;
	mutable my_linked_ptr const* prev;
	mutable my_linked_ptr const* next;

	void clear() noexcept
	{
		if (prev == next)
		{
			delete ptr;
		}
		else
		{
			if (prev)
			{
				prev->next = next;
			}
			if (next)
			{
				next->prev = prev;
			}
		}
	}


	void connect(my_linked_ptr &p) noexcept
	{
		if (p.next)
		{
			p.next->prev = &p;
		}
		if (p.prev)
		{
			p.prev->next = &p;
		}
	}
};
