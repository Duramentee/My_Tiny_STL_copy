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

// swap
// 用移动语义提高效率
// Tp 一般是 template parameter的缩写
// lhs, rhs 分别是left hand side和right hand side的缩写
template <class Tp>
void swap(Tp& lhs, Tp& rhs)
{
	auto tmp(mystl::move(lhs));
	lhs = mystl::move(rhs);
	rhs = mystl::move(tmp);
}

// 用于一段范围的 swap
// first1, last1 表示第一个范围的开始和结束迭代器. first2 表示第二个范围开始的迭代器
template <class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
{
	// 直到 first1 达到 last 之前,做遍历
	// (void) ++first2,将 ++first2 的结果强制转换为 void,主要目的在于确保 ++first2 的表达式不会产生未使用的结果的警告。
	for (; first1 != last1; ++first, (void) ++first2)
	{
		mystl::swap(*first1, *first2);
	}
	return first2;
}

// 专门用于数组类型的 swap
// Tp(&a)[N] 和 Tp(&b)[N] 表示两个同类型、同大小的数组,(注意加小括号,否则代表一个长度为N的数组a,内部为Tp类型的引用)
template <class Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N])
{
	mystl::swap_range(a, a + N, b);
}

} // namespace mystl

#endif // !MYTINYSTL_UTIL_H_