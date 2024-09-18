#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数

// 包含 <cstddef> 头文件用于定义 ptrdiff_t。
#include <cstddef>

#include "type_traits.h"

namespace mystl
{

//五种迭代器类型标记
struct input_iterator_tag {};  // 输入迭代器
struct output_iterator_tag {};  // 输出迭代器
struct forward_iterator_tag : public input_iterator_tag {};  // 前向迭代器
struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向迭代器
struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问迭代器

//iterator模板,定义迭代器的一般属性
template <class Category, class T, class Distance = ptrdiff_t,
		  class Pointer = T*, class Reference = T&>
struct iterator
{
	using iterator_category = Category;
	using value_type        = T;
	using pointer           = Pointer;
	using reference         = Reference;
	using difference_type   = Distance;
};


// 检测某个类型 T 是否具有 iterator_category 成员
// 在实现上利用了两个重载的 test 函数和大小比较来区分有无该成员
template <class T>
struct has_iterator_cat
{
private:
	// 用于 size 比较，确保重载的 test 函数返回不同的类型。
	// 该结构体的大小为2
	struct two 
	{
		char a; char b;
	};

	// 使用了一个可变参数模板，接受任意参数类型并返回 two 结构体的实例。
	// 这个函数主要用于处理没有 iterator_category 成员的情况。
	template <class U> 
	static two test(...);

	// 接受一个指向 U::iterator_category 的指针，通常用于判断 U 是否具有 iterator_category 这个成员。
	// 如果 U 有此成员，那么调用该函数将会成功返回 char 类型。
	template <class U>
	static char test(typename U::iterator_category* = nullptr);

public:
	// 定义了一个公有的静态常量 value，它通过对 test<T>(nullptr) 的调用计算出返回值的大小。
	// 如果 T 具有 iterator_category 成员，则调用第二个 test 函数，返回一个 char，大小为1.
	// 如果没有，则调用第一个 test 函数，返回一个 two 类型的实例，其大小为2。
	// 因此，sizeof(test<T>(nullptr)) == sizeof(char) 为真时意味着 T 存在 iterator_category 成员，反之则为假。
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_impl {};

template <class Iterator>
struct iterator_traits_impl<Iterator, true>
{
	using iterator_category = typename Iterator::iterator_category;
	using value_type        = typename Iterator::value_type;
	using pointer           = typename Iterator::pointer;
	using reference         = typename Iterator::reference;
	using difference_type   = typename Iterator::difference_type;
};

template <class Iterator, bool>
struct iterator_traits_helper {};

template <class Iterator>
struct iterator_traits_helper<Iterator, true>
	: public iterator_traits_impl<Iterator,
	std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
	std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
{
};

// 萃取迭代器的特性
template <class Iterator>
struct iterator_traits 
	: public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

// 针对原生指针的偏特化版本
template <class T>
struct iterator_traits<T*>
{
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using pointer           = T*;
	using reference         = T&;
	using difference_type   = ptrdiff_t;
};

template <class T>
struct iterator_traits<const T*>
{
  using iterator_category = random_access_iterator_tag;
  using value_type        = T;
  using pointer           = const T*;
  using reference         = const T&;
  using difference_type   = ptrdiff_t;
};

template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
	: public m_bool_constant<std::is_convertible<
	typename iterator_traits<T>::iterator_category, U>::value>
{
};

}

#endif // !MYTINYSTL_ITERATOR_H_