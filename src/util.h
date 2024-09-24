#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等.

#include <cstddef>

#include "type_traits.h"

namespace mystl
{
//move

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
	return static_cast<T&&>(arg);
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
	for (; first1 != last1; ++first, (void)++first2)
	{
		mystl::swap(*first1, *first2);
	}
	return first2;
}

// 专门用于数组类型的 swap
// Tp(&a)[N] 和 Tp(&b)[N] 表示两个同类型、同大小的数组,(注意加小括号,否则代表一个长度为N的数组a,内部为Tp类型的引用)
template <class Tp, size_t N>
void swap(Tp (&a)[N], Tp (&b)[N])
{
	mystl::swap_range(a, a + N, b);
}

// --------------------------------------------------------------------------------------
// pair

// 结构体模板 : pair
// 两个模板参数分别表示两个数据的类型
// 用 first 和 second 来分别取出第一个数据和第二个数据
template <class Ty1, class Ty2>
struct pair
{
	using first_type = Ty1;
	using second_type = Ty2;

	first_type first; // 保存第一个数据
	second_type second; // 保存第二个数据

	// default constructible
	// std::enable_if 限制该构造函数的实例化条件
	// Other1 和 Other2 均可默认构造时该构造函数允许被使用
	template <class Other1 = Ty1, class Other2 = Ty2,
	          typename = typename std::enable_if<
		          std::is_default_constructible<Other1>::value &&
		          std::is_default_constructible<Other2>::value, void>::type>
	constexpr pair() : first(), second()
	{
	}

	// implicit constructiable for this type
	// 满足四个条件时该函数可被调用
	// U1,U2必须可拷贝构造,U1可隐式转换为Ty1,U2可隐式转换为Ty2
	template <class U1 = Ty1, class U2 = Ty2,
	          typename std::enable_if<
		          std::is_copy_constructible<U1>::value &&
		          std::is_copy_constructible<U2>::value &&
		          std::is_convertible<const U1&, Ty1>::value &&
		          std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
	constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(2)
	{
	}

	// explicit constructible for this type
	// 用于从类型不完全相同的参数创建 pair 对象
	// 满足三个条件时该函数可被调用
	// U1,U2可以拷贝构造,U1,U2其中之一不能隐式转换为 Ty1 和 Ty2,换句话说，这个构造函数仅在 U1 或 U2 不能直接转换为 Ty1 或 Ty2 时可用。
	template <class U1 = Ty1, class U2 = Ty2,
	          typename std::enable_if<
		          std::is_copy_constructible<U1>::value &&
		          std::is_copy_constructible<U2>::value &&
		          (!std::is_convertible<const U1&, Ty1>::value ||
			          !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
	explicit constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b)
	{
	}

	// 使用默认拷贝构造和移动构造
	pair(const pair& rhs) = default;
	pair(pair&& rhs) = default;

	// implicit constructiable for other type
	template <class Other1, class Other2,
	          typename std::enable_if<
		          std::is_constructible<Ty1, Other1>::value &&
		          std::is_constructible<Ty2, Other2>::value &&
		          std::is_convertible<Other1&&, Ty1>::value &&
		          std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
	constexpr pair(Other1&& a, Other2&& b)
		: first(mystl::forward<Other1>(a)),
		  second(mystl::forward<Other2>(b))
	{
	}
};
} // namespace mystl

#endif // !MYTINYSTL_UTIL_H_
