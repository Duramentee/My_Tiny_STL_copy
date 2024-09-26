#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include <memory.h>

#include "construct.h"
#include "util.h"

namespace mystl
{
// 模板类：allocator
// 模板函数代表数据类型

template <class T>
class allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

public:
	// 分配一块内存给单个对象
	static T* allocate();
	// 分配一整片内存给多个对象
	static T* allocate(size_type n);

	// 释放一块内存
	static void deallocate(T* ptr);
	// 释放一整片内存
	static void deallocate(T* ptr, size_type n);

	// 构造
	static void construct(T* ptr);
	// 拷贝构造
	static void construct(T* ptr, const T& value);
	// 移动构造
	static void construct(T* ptr, T&& value);

	// 多参构造,配合完美转发
	template <class... Args>
	static void construct(T* ptr, Args&&... args);

	// 单个对象析构
	static void destroy(T* ptr);
	// 多个对象析构
	static void destroy(T* first, T* last);
};

template <class T>
T* allocator<T>::allocate()
{
	// operator new 类似 malloc 仅分配内存而不构造对象
	return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n)
{
	if (n == 0)
		return nullptr;
	return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T* ptr)
{
	if (ptr == nullptr)
		return;
	::operator delete(ptr);
}

// size_type /*size*/ 是为了告诉编译器,这个参数不使用,这种用法通常用于
// 接口的一致性：在需要保持某个接口的情况下，允许某个参数未使用。
// 清晰的代码：通过注释告诉其他开发者这个参数的存在及其类型，让代码更加可读。
// 此处实现和上一个函数一致,c++释放内存不需要知道内存的大小,使用 ::operator delete 就够了
template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/)
{
	if (ptr == nullptr)
		return;
	::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr)
{
	mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value)
{
	mystl::construct(ptr, value);
}

template <class T>
void allocator<T>::construct(T* ptr, T&& value)
{
	mystl::construct(ptr, mystl::move(value));
}

// 第一次 template 是为了解释类模板的类型参数，而第二次 template 则是为了解释成员函数的参数模板。
template <class T>
template <class... Args>
void allocator<T>::construct(T* ptr, Args&&... args)
{
	mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destroy(T* ptr)
{
	mystl::destroy(ptr);
}

template <class T>
void allocator<T>::destroy(T* first, T* last)
{
	mystl::destroy(first, last);
}

} // namespace mystl
#endif // !MYTINYSTL_ALLOCATOR_H_
