#pragma once
#include <cstdio>
#include <utility>

template <typename T>
struct my_shared_ptr
{
	my_shared_ptr() = delete;

	my_shared_ptr(T* p) : cnt(nullptr), ptr(p)
	{
		if (p)
		{
			try
			{
				cnt = new size_t(1);
			}
			catch (...)
			{
				delete p;
				throw;
			}
		}
	}

	my_shared_ptr(my_shared_ptr const& other) noexcept : cnt(other.cnt), ptr(other.ptr)
	{
		if (ptr) {
			++(*cnt);
		}
	}

	my_shared_ptr(my_shared_ptr&& other) noexcept : cnt(other.cnt), ptr(other.ptr) 
	{
		other.ptr = nullptr;
	}

	my_shared_ptr& operator=(my_shared_ptr const& other) noexcept
	{
		my_shared_ptr tmp(other);
		swap(tmp);
		return *this;
	}

	my_shared_ptr& operator=(my_shared_ptr&& other) noexcept
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

	void swap(my_shared_ptr& other) noexcept
	{
		std::swap(cnt, other.cnt);
		std::swap(ptr, other.ptr);
	}

	~my_shared_ptr()
	{
		clear();
	}

private:
	size_t* cnt;
	T* ptr;

	void clear() noexcept
	{
		if (ptr)
		{
			if (*cnt == 1) {
				delete ptr;
				delete cnt;
			}
			else
			{
				--(*cnt);
			}
		}
	}
};

