#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include "my_pointers/my_shared_ptr.h"
#include "my_pointers/my_linked_ptr.h"

template <typename T, template<typename> class U = my_shared_ptr>
struct persistent_set
{
	typedef T value_type;

	struct iterator;

	persistent_set() noexcept;
	persistent_set(persistent_set const& other) noexcept;
	persistent_set(persistent_set&& other) noexcept;

	~persistent_set() = default;

	persistent_set& operator=(persistent_set const& other) noexcept;
	persistent_set& operator=(persistent_set&& other) noexcept;

	iterator find(value_type const& x) const noexcept;

	std::pair<iterator, bool> insert(value_type x);

	void erase(iterator const& it);

	bool empty() const noexcept;

	iterator begin() const noexcept;
	iterator end() const noexcept;

	void swap(persistent_set& other) noexcept;

private:
	struct base_node;
	struct node;

	typedef U<node> pointer;

	base_node root;

	std::vector<node*> get_path(node* from, value_type const &x) const;
	pointer erase_impl(node* ver, value_type const& odd);
	node* copy_path(std::vector<node*> const& path, node* last);
};

template <typename T, template<typename> class U>
struct persistent_set<T, U>::iterator
{
	friend struct persistent_set<T, U>;
	iterator() noexcept : root(nullptr), ptr(nullptr) {}

	iterator(iterator const&) = default;

	value_type const& operator*() const;

	iterator& operator++();
	iterator operator++(int);

	iterator& operator--();
	iterator operator--(int);

	friend bool operator==(iterator const& a, iterator const& b) noexcept
	{
		return ((a.root == b.root) && (a.ptr == b.ptr));
	}

	friend bool operator!=(iterator const& a, iterator const& b) noexcept
	{
		return ((a.ptr != b.ptr) || (a.root != b.root));
	}
private:
	iterator(base_node const* a, base_node const* b) noexcept : root(a), ptr(b) {}

	base_node const* root;
	base_node const* ptr;

	void pp_impl(value_type const& x, node const* cur, base_node const* prev, bool& task, iterator& res);
	void mm_impl(value_type const& x, node const* cur, node const* prev, bool& task, iterator& res);
};

template <typename T, template<typename> class U>
void swap(persistent_set<T, U>& a, persistent_set<T, U>& b) noexcept
{
	a.swap(b);
}

template <typename T, template<typename> class U>
struct persistent_set<T, U>::base_node
{
	base_node() = delete;

	base_node(base_node const& other) noexcept : left(other.left) {}

	base_node(base_node&& other) noexcept : left(std::move(other.left)) {}

	base_node(pointer l) noexcept : left(std::move(l)) {}

	pointer left;
};

template <typename T, template<typename> class U>
struct persistent_set<T, U>::node : persistent_set<T, U>::base_node
{
	node() = delete;

	node(T d) noexcept(noexcept(T(std::move(d)))) 
		: base_node(pointer(nullptr))
		, right(nullptr)
		, data(std::move(d))
	{}

	node(pointer l, pointer r, const T& d) noexcept(noexcept(T(d)))
		: base_node(std::move(l))
		, right(std::move(r))
		, data(d)
	{}

	pointer right;
	value_type data;
};

template<typename T, template<typename> class U>
void persistent_set<T, U>::swap(persistent_set& other) noexcept
{
	root.left.swap(other.root.left);
}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set() noexcept : root(pointer(nullptr)) {}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set(persistent_set const& other) noexcept : root(other.root) {}

template <typename T, template<typename> class U>
persistent_set<T, U>::persistent_set(persistent_set&& other) noexcept : root(std::move(other.root)) {}

template <typename T, template<typename> class U>
persistent_set<T, U>& persistent_set<T, U>::operator=(persistent_set const& other) noexcept
{
	root.left = other.root.left;
	return *this;
}

template <typename T, template<typename> class U>
persistent_set<T, U>& persistent_set<T, U>::operator=(persistent_set&& other) noexcept
{
	root.left = std::move(other.root.left);
	return *this;
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::value_type const& persistent_set<T, U>::iterator::operator*() const
{
	return (static_cast<node const*>(ptr))->data;
}

template <typename T, template<typename> class U>
void persistent_set<T, U>::iterator::pp_impl(value_type const& x, node const* cur, base_node const* prev, bool& task, iterator& res)
{
	if (x < cur->data)
	{
		pp_impl(x, cur->left.get(), cur, task, res);
	}
	else if (cur->data < x)
	{
		pp_impl(x, cur->right.get(), cur, task, res);
	}
	else
	{
		if (cur->right)
		{
			cur = cur->right.get();
			while (cur->left)
			{
				cur = cur->left.get();
			}
			res.ptr = cur;
			task = 1;
		}
	}
	if (task)
	{
		return;
	}
	if (prev->left.get() == cur)
	{
		res.ptr = prev;
		task = 1;
	}
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator& persistent_set<T, U>::iterator::operator++()
{
	bool tmp = 0;
	pp_impl(this->operator*(), root->left.get(), root, tmp, *this);
	return *this;
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator persistent_set<T, U>::iterator::operator++(int)
{
	iterator tmp(*this);
	++(*this);
	return tmp;
}

template <typename T, template<typename> class U>
void persistent_set<T, U>::iterator::mm_impl(value_type const& x, node const* cur, node const* prev, bool& task, iterator& res)
{
	if (x < cur->data)
	{
		mm_impl(x, cur->left.get(), cur, task, res);
	}
	else if (cur->data < x)
	{
		mm_impl(x, cur->right.get(), cur, task, res);
	}
	else
	{
		if (cur->left)
		{
			cur = cur->left.get();
			while (cur->right)
			{
				cur = cur->right.get();
			}
			res.ptr = cur;
			task = 1;
		}
	}
	if (task)
	{
		return;
	}
	if (prev->right.get() == cur)
	{
		res.ptr = prev;
		task = 1;
	}
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator& persistent_set<T, U>::iterator::operator--()
{
	if (root == ptr)
	{
		node* ans = root->left.get();
		while (ans->right)
		{
			ans = ans->right.get();
		}
		ptr = ans;
	}
	else
	{
		bool tmp = 0;
		mm_impl(this->operator*(), root->left.get(), nullptr, tmp, *this);
	}
	return *this;
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator persistent_set<T, U>::iterator::operator--(int)
{
	iterator tmp(*this);
	--(*this);
	return tmp;
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator persistent_set<T, U>::begin() const noexcept
{
	base_node const* tmp = &root;
	while (tmp->left)
	{
		tmp = tmp->left.get();
	}
	return iterator(&root, tmp);
}

template <typename T, template<typename> class U>
typename persistent_set<T, U>::iterator persistent_set<T, U>::end() const noexcept
{
	return iterator(&root, &root);
}

template<typename T, template<typename> class U>
typename persistent_set<T, U>::iterator persistent_set<T, U>::find(value_type const& x) const noexcept
{
	node const* tmp = root.left.get();
	while (tmp && tmp->data != x)
	{
		if (x < tmp->data)
		{
			tmp = tmp->left.get();
		}
		else
		{
			tmp = tmp->right.get();
		}
	}
	iterator ans(&root, &root);
	if (tmp)
	{
		ans.ptr = tmp;
	}
	return ans;
}

template<typename T, template<typename> class U>
std::vector<typename persistent_set<T, U>::node*> persistent_set<T, U>::get_path(node* from, value_type const& x) const
{
	std::vector<node*> path;
	while (from)
	{
		path.push_back(from);
		if (x < from->data)
		{
			from = from->left.get();
		}
		else
		{
			from = from->right.get();
		}
	}
	return path;
}

template<typename T, template<typename> class U>
typename persistent_set<T, U>::node* persistent_set<T, U>::copy_path(std::vector<node*> const& path, node* last)
{
	for (size_t i = 1; i < path.size(); ++i)
	{
		if (path[i]->left.get() == path[i - 1])
		{
			last = new node(pointer(last), path[i]->right, path[i]->data);
		}
		else
		{
			last = new node(path[i]->left, pointer(last), path[i]->data);
		}
	}
	return last;
}

template<typename T, template<typename> class U>
typename persistent_set<T, U>::pointer persistent_set<T, U>::erase_impl(node* ver, value_type const& odd)
{
	if (odd < ver->data)
	{
		return pointer(new node(erase_impl(ver->left.get(), odd), ver->right, ver->data));
	}
	else if (ver->data < odd)
	{
		return pointer(new node(ver->left, erase_impl(ver->right.get(), odd), ver->data));
	}
	if (ver->left && ver->right)
	{
		std::vector<node*> path = get_path(ver->right.get(), odd);
		value_type const& new_data = path.back()->data;
		if (path.size() > 1)
		{
			node* last = new node(path.back()->right, path[path.size() - 2]->right, path[path.size() - 2]->data);
			path.pop_back();
			std::reverse(path.begin(), path.end());
			last = copy_path(path, last);
			return pointer(new node(ver->left, pointer(last), new_data));
		}
		else
		{
			return pointer(new node(ver->left, path.back()->right, new_data));
		}
	}
	else if (ver->left)
	{
		return ver->left;
	}
	else
	{
		return ver->right;
	}

}

template<typename T, template<typename> class U>
void persistent_set<T, U>::erase(iterator const& it)
{
	if ((begin() == end()) || (it == end()) || (find(*it) == end()))
	{
		return;
	}
	root.left = erase_impl(root.left.get(), *it);
}

template<typename T, template<typename> class U>
std::pair<typename persistent_set<T, U>::iterator, bool> persistent_set<T, U>::insert(value_type x)
{
	iterator it = find(x);
	if (it != end())
	{
		return { it, 0 };
	}
	node* t = root.left.get();
	std::vector<node*> path = get_path(t, x);
	std::reverse(path.begin(), path.end());
	bool f = ((!path.empty()) && (x < path[0]->data));
	node* last = new node(std::move(x));
	it.ptr = last;

	if (!path.empty()) {
		if (f)
		{
			last = new node(pointer(last), path[0]->right, path[0]->data);
		}
		else
		{
			last = new node(path[0]->left, pointer(last), path[0]->data);
		}
		last = copy_path(path, last);
	}
	root.left = pointer(last);
	return { it, 1 };
}

template<typename T, template<typename> class U>
bool persistent_set<T, U>::empty() const noexcept
{
	return begin() == end();
}