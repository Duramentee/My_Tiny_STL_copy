#ifndef MYTINYSTL_ALGOBASE_H_
#define MYTINYSTL_ALGOBASE_H_

// 这个头文件包含了 mystl 的基本算法

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl
{
// #ifdef 指令用于检查是否已定义某个宏。
// 当宏 max 或 min 已被定义时，后续的代码将会被编译。
// 如果 max 或 min 已经被定义，程序会执行 #pragma message 指令。
// #pragma message 是用于输出编译时警告或信息的指令,提示开发者相应的宏已被取消定义.
// #undef 指令用于取消某个宏的定义,对于 max 和 min，如果它们已经被定义，则会将其取消定义.
// 通过取消定义，后续的代码就可以定义新的 max 和 min 函数或模板，而不会发生命名冲突，确保代码的正常运行.

#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif // min

// 取二者中的较大值，语义相等时保证返回第一个参数
template <class T>
const T& max(const T& lhs, const T& rhs)
{
	return lhs < rhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare comp)
{
	return comp(lhs, rhs) ? rhs : lhs;
}

// 取二者中的较小值, 语义相等时保证返回第一个参数
template <class T>
const T& min(const T& lhs, const T& rhs)
{
	return rhs < lhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作, 用以支持自定义的比较操作
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare comp)
{
	return comp(rhs, lhs) ? rhs : lhs;
}

// iter_swap 将两个迭代器所指对象对调
template <class FIter1, class FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs)
{
	mystl::swap(*lhs, *rhs);
}

// copy 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内

// input_iterator_tag 版本, 中间层辅助函数
template <class InputIter, class OutputIter>
OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
                              mystl::input_iterator_tag)
{
	for (; first != last; ++first, ++result)
		*result = *first;
	return result;
}

// ramdom_access_iterator_tag 版本, 中间层辅助函数
template <class RandomIter, class OutputIter>
OutputIter unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
                              mystl::random_access_iterator_tag)
{
	for (auto n = last - first; n > 0; --n, ++first, ++result)
		*result = *first;
	return result;
}

// 萃取迭代器特性后进行拷贝
template <class InputIter, class OutputIter>
OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result)
{
	return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本

} // namespace mystl
#endif // !MYTINYSTL_ALGOBASE_H_
