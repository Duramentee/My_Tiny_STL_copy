#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等.

#include <cstddef>

#include "type_traits.h"

namespace mystl
{

// move

template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
	// static_cast 将arg万能引用强转为右值引用
	return static_cast<typename std::remove_reference<T>::type&&>(arg); 
}

// forward

// 左值引用版本
// std::remove_reference<T>::type 会去掉 T 的引用特性（如果有的话），返回去掉引用后的类型。
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
	return static_cast<T&&>()arg;
}

// 右值引用版本
template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
{
	// 静态断言,如果参数arg为左值引用就断言失败
	static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
	return static_cast<T&&>(arg);
}


}





#endif  //! MYTINYSTL_UTIL_H_