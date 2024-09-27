#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素
// 主要实现了,复制,填充,移动三种功能,填充和移动的功能实现均类似于复制的实现

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl
{
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置

// 当目标类型支持平凡复制时，直接使用复制算法
template <class InputerIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputerIter first, InputerIter last, ForwardIter result, std::true_type)
{
	return mystl::copy(first, last, result);
}

// 当目标类型不支持平凡复制时，使用构造函数逐个构造元素，并处理可能发生的异常
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
	auto cur = result;
	try
	{
		for (; first != last; ++first, ++cur)
			mystl::construct(&*cur, *first);
	}
	catch (...)
	{
		for (; result != cur; --cur)
			mystl::destroy(&*cur);
	}
	return cur;
}

// 根据ForwardIter中元素类型的属性，调用unchecked_uninit_copy处理元素复制
template <class InputIter, class ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
{
	return mystl::unchecked_uninit_copy(first, last, result,
	                                    std::is_trivially_copy_assignable<
		                                    typename iterator_traits<ForwardIter>::
		                                    value_type>{});
}

// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
	return mystl::copy_n(first, n, result).second;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
	auto cur = result;
	try
	{
		for (; n > 0; --n, ++cur, ++first)
			mystl::construct(&*cur, *first);
	}
	catch (...)
	{
		for (; result != cur; --cur)
			mystl::destroy(&*cur);
	}
	return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
{
	return mystl::unchecked_uninit_copy_n(first, n, result,
	                                      std::is_trivially_copy_assignable<
		                                      typename iterator_traits<InputIter>::
		                                      value_type>{});
}

// uninitialized_fill
// 在 [first, last) 区间内填充元素值
template <class ForwardIter, class T>
void
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type)
{
	mystl::fill(first, last, value);
}

template <class ForwardIter, class T>
void
unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type)
{
	auto cur = first;
	try
	{
		for (; cur != last; ++cur)
			mystl::construct(&*cur, value);
	}
	catch (...)
	{
		for (; first != cur; ++first)
			mystl::destroy(&*first);
	}
}

template <class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
{
	mystl::unchecked_uninit_fill(first, last, value,
	                             std::is_trivially_copy_assignable<
		                             typename iterator_traits<ForwardIter>::
		                             value_type>{});
}

// uninitialized_fill_n
// 从 first 位置开始，填充 n 个元素值，返回填充结束的位置

template <class ForwardIter, class Size, class T>
ForwardIter
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
{
	return mystl::fill_n(first, n, value);
}

template <class ForwardIter, class Size, class T>
ForwardIter
unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
{
	auto cur = first;
	try
	{
		for (; n > 0; --n, ++cur)
			mystl::construct(&*cur, value);
	}
	catch (...)
	{
		for (; first != cur; ++first)
			mystl::destroy(&*first);
	}
	return cur;
}

template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
{
	return mystl::unchecked_uninit_fill_n(first, n, value,
	                                      std::is_trivially_copy_assignable<
		                                      typename iterator_traits<ForwardIter>::
		                                      value_type>{});
}

// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置

template <class InputIter, class ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
{
	return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter>
ForwardIter
unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
{
	ForwardIter cur = result;
	try
	{
		for (; first != last; ++first, ++cur)
			mystl::construct(&*cur, mystl::move(*first));
	}
	catch (...)
	{
		mystl::destroy(result, cur);
	}
	return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
{
	return mystl::unchecked_uninit_move(first, last, result,
	                                    std::is_trivially_move_assignable<
		                                    typename iterator_traits<InputIter>::
		                                    value_type>{});
}

// uninitialized_move_n
// 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置

template <class InputIter, class Size, class ForwardIter>
ForwardIter
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
{
	return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter
unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
{
	auto cur = result;
	try
	{
		for (; n > 0; --n, ++first, ++cur)
			mystl::construct(&*cur, mystl::move(*first));
	}
	catch (...)
	{
		for (; result != cur; ++result)
			mystl::destroy(&*result);
		throw;
	}
	return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
{
	return mystl::unchecked_uninit_move_n(first, n, result,
	                                      std::is_trivially_move_assignable<
		                                      typename iterator_traits<InputIter>::
		                                      value_type>{});
}

} // namespace mystl
#endif // !MYTINYSTL_UNINITIALIZED_H_
