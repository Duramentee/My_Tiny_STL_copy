#ifndef MYTINYSTL_MEMORY_H_
#define MYTINYSTL_MEMORY_H_

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr

#include <cstddef>
#include <cstdlib>
#include <climits>
#include <bits/error_constants.h>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl
{
// 获取对象地址
template <class Tp>
constexpr Tp* address_of(Tp& value) noexcept
{
	return &value;
}

// 获取以及释放临时缓冲区

// 根据给定的长度申请一块内存缓冲区，并返回一个指向这块内存的指针以及实际分配的大小。
// 大小指定为 len * sizeof(T)
// ptrdiff_t 是一种用于表示指针之间差异的有符号整数类型
template <class T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*)
{
	// 为确保 len 的值在乘以 sizeof(T) 之前不会导致溢出,做检查
	if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
		len = INT_MAX / sizeof(T);
	while (len > 0)
	{
		T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
		if (tmp)
			return pair<T*, ptrdiff_t>(tmp, len);
		// 申请失败时减少 len 的大小
		len /= 2;
	}
	return pair<T*, ptrdiff_t>(nullptr, 0);
}

// 申请缓冲区内存
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
{
	return get_buffer_helper(len, static_cast<T*>(nullptr));
}

// 重载版本增加了第二个参数,增加了灵活性,提供了类型信息
template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*)
{
	return get_buffer_helper(len, static_cast<T*>(nullptr));
}

// 直接 free 释放,不需要提供长度信息(长度信息已经存在于malloc申请的内存块中)
template <class T>
void release_temporary_buffer(T* ptr)
{
	free(ptr);
}

// 类模板 temporary_buffer
// 进行临时缓冲区的申请与释放
template <class ForwardIterator, class T>
class temporary_buffer
{
private:
	ptrdiff_t original_len; // 缓冲区申请的大小
	ptrdiff_t len; // 缓冲区实际的大小
	T* buffer; // 指向缓冲区的指针

public:
	temporary_buffer(ForwardIterator first, ForwardIterator last);

	~temporary_buffer()
	{
		mystl::destroy(buffer, buffer + len);
		free(buffer);
	}

	// 弃置拷贝构造
	temporary_buffer(const temporary_buffer&) = delete;

	// 弃置拷贝赋值
	void operator=(const temporary_buffer&) = delete;

public:
	// 函数返回用于临时缓冲区的实际大小，类型为ptrdiff_t
	ptrdiff_t size() const noexcept { return len; }
	// 函数返回请求的缓冲区大小，即用户最初要求的大小
	ptrdiff_t requested_size() const noexcept { return original_len; }
	// 返回指向缓冲区开始的指针
	T* begin() noexcept { return buffer; }
	// 返回指向缓冲区结束位置的指针（即末尾）
	T* end() noexcept { return buffer + len; }

private:
	void allocate_buffer();
	void initialize_buffer(const T&, std::true_type) {}

	void initialize_buffer(const T& value, std::false_type)
	{
		mystl::uninitialized_fill_n(buffer, len, value);
	}
};

// 构造函数
template <class ForwardIterator, class T>
temporary_buffer<ForwardIterator, T>::temporary_buffer(ForwardIterator first, ForwardIterator last)
{
	try
	{
		// len 为缓冲区长度,长度为两迭代器间距离
		len = mystl::distance(first, last);
		allocate_buffer();
		if (len > 0)
			initialize_buffer(*first, std::is_trivially_default_constructible<T>());
	}
	catch (...)
	{
		free(buffer);
		buffer = nullptr;
		len = 0;
	}
}

// allocate_buffer 函数
template <class ForwardIterator, class T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer()
{
	original_len = len;
	if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
		len = INT_MAX / sizeof(T);
	while (len > 0)
	{
		buffer = static_cast<T*>(malloc(len * sizeof(T)));
		if (buffer)
			break;
		// 申请失败时减少申请空间大小
		len /= 2;
	}
}

// 模板类: auto_ptr
// 一个具有严格对象所有权的小型智能指针
template <class T>
class auto_ptr
{
public:
	typedef T elem_type;

private:
	T* m_ptr; // 内部实际的指针

public:
	// 构造,不传参则默认用nullptr构造
	explicit auto_ptr(T* p = nullptr) : m_ptr(p) {}

	// 构造函数,用另一个auto_ptr构造,两个 auto_ptr 内部元素应当相同
	auto_ptr(auto_ptr& rhs) : m_ptr(rhs.release()) {}

	// 构造函数,用另一个auto_ptr构造,两个 auto_ptr 内部元素可以不同
	template <class U>
	auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

	auto_ptr& operator=(auto_ptr& rhs)
	{
		// 排除自赋值
		if (this != &rhs)
		{
			// 析构自己的资源,获取目标资源
			delete m_ptr;
			m_ptr = rhs.release();
		}
		return *this;
	}

	template <class U>
	auto_ptr& operator=(auto_ptr<U>& rhs)
	{
		// 排除自赋值
		if (this->get() != rhs.get())
		{
			delete m_ptr;
			m_ptr = rhs.release();
		}
		return *this;
	}

	~auto_ptr() { delete m_ptr; }

public:
	// 重载 operator* 和 operator->
	T& operator*() const { return *m_ptr; }
	T* operator->() const { return m_ptr; }

	// 获得指针
	T* get() const { return m_ptr; }

	// 释放指针
	T* release()
	{
		// 获取自身指针,随后置空自身指针
		T* tmp = m_ptr;
		m_ptr = nullptr;
		return tmp;
	}

	void reset(T* p = nullptr)
	{
		// 先删除自身的,再获取传入的
		if (m_ptr != p)
		{
			delete m_ptr;
			m_ptr = p;
		}
	}
};

} // namespace mystl
#endif // !MYTINYSTL_MEMORY_H_
