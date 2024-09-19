#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数

// 包含 <cstddef> 头文件用于定义 ptrdiff_t。
#include <cstddef>

#include "type_traits.h"

namespace mystl
{

//五种迭代器类型标记
struct input_iterator_tag {};  // 输入迭代器标签
struct output_iterator_tag {};  // 输出迭代器标签
struct forward_iterator_tag : public input_iterator_tag {};  // 前向迭代器标签
struct bidirectional_iterator_tag : public forward_iterator_tag {};  // 双向迭代器标签
struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问迭代器标签

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

//萃取某种迭代器
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <class Iterator>
struct is_iterator
	: public m_bool_constant<is_input_iterator<Iterator>::value
						  || is_output_iterator<Iterator>::value>
{
};

//萃取某个迭代器的category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
	using Category = typename iterator_traits<Iterator>::iterator_category;
	//调用category构造
	return Category();
}

//萃取某个迭代器的distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
}

//以下函数用以计算迭代器之间距离

//distance 的 input_iterator_tag 版本
//提取出的辅助函数
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
{
	typename iterator_traits<InputIterator>::difference_type n = 0;
	while(first != last)
	{
		++first;
		++n;
	}

	return n;
}

//distance 的 random_access_iterator_tag版本
//提取出的辅助函数
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag)
{
	return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
	return distance_dispatch(first, last, iterator_category(first));
}

//以下函数用于让迭代器前进 n 个距离

//advance 的 input_iterator_tag 版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
{
	while (n--)
		++i;
}

//advance 的 bidirectional_iterator_tag 版本
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
	if (n >= 0)
		while (n--)
			++i;
	else
		while (n++)
			--i;
}

//advace 的 random_access_iterator_tag 版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
{
	i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n)
{
	advance_dispatch(i, n, iterator_category(i));
}

// ********************************************************************************************* //

// 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator
{
private:
	Iterator current;  // 记录该反向迭代器对应的正向迭代器

public:
	// 反向迭代器的五种相应型别
	using iterator_category = typename iterator_traits<Iterator>::iterator_category;
	using value_type = typename iterator_traits<Iterator>::value_type;
	using difference_type = typename iterator_traits<Iterator>::difference_type;
	using pointer = typename iterator_traits<Iterator>::pointer;
	using reference = typename iterator_traits<Iterator>::reference;

	using iterator_type = Iterator;
	using self = reverse_iterator<Iterator>;

public:
	//三类构造函数
	reverse_iterator() = default;  // 默认构造
	explicit reverse_iterator(iterator_type i) : current(i) {}  // 单参构造,参数为指定的迭代器型别
	reverse_iterator(const self &rhs) : current(rhs.current) {}  // 拷贝构造

public:
	//获取对应的正向迭代器
	Iterator_type base() const
	{
		return current;
	}

	//重载运算符
	reference operator*() const
	{
		auto tmp = current;
		return *--tmp;
	}
	pointer operator->() const
	{
		return &(operator*());
	}

	// 前进(++)变为后退(--)
	self& operator++()
	{
		--current;
		return *this;
	}
	self operator(int)
	{
		self tmp = *this;
		--current;
		return tmp;
	}

	// 后退(--)变为前进(++)
	self& operator--()
	{
		++current;
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		++current;
		return tmp;
	}

	self& operator+=(difference_type n)
	{
		current -= n;
		return *this;
	}
	self operator+(difference_type n) const
	{
		return self(current - n);
	}
	self& operator-=(difference_type n) const
	{
		current += n;
		return *this;
	}
	self operator-(difference_type n) const
	{
		return self(current + n);
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}
};

//重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
		  const reverse_iterator<Iterator>& rhs)
{
	return rhs.base() - lhs.base();
}

//重载 operator==
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator==(const reverse_iterator<Iterator>& lhs,
		  const reverse_iterator<Iterator>& rhs)
{
	return lhs.base() == rhs.base();
}

//重载 operator<
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator<(const reverse_iterator<Iterator>& lhs,
		  const reverse_iterator<Iterator>& rhs)
{
	return rhs.base() < lhs.base();
}

//重载 operator!=
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
		  const reverse_iterator<Iterator>& rhs)
{
	return !(lsh == rhs);
}

//重载 operator>
template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
{
  return rhs < lhs;
}

//重载 operator<=
template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return !(rhs < lhs);
}

//重载 operator>=
template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return !(lhs < rhs);
}

}  // namespace mystl

#endif // !MYTINYSTL_ITERATOR_H_