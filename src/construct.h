#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

// 主要围绕构造的对象能否进行平凡构造以及平凡析构编写
// _one后缀的函数用于单个构造和析构
// _cat后缀的函数作为辅助函数,被对应函数调用(例如 destroy 被 destroy_one 调用), 依据其构造或析构是否是平凡的而决定具体的函数调用策略

#include <new>

#include "type_traits.h"
#include "iterator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace mystl
{

// construct 构造对象

// 无参
template <class Ty>
void construct(Ty* ptr)
{
	::new (static_cast<void*>(ptr)) Ty();
}

// 单参
template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value)
{
	::new (static_cast<void*>(ptr)) Ty1(value);
}

// 多参
template <class Ty, class... Args>
void construct(Ty* ptr, Args&&... args)
{
	::new (static_cast<void*>(ptr)) Ty(mystl::forward<Args>(args)...);
}

// destroy 析构对象

// 针对单个对象的析构,第二个参数将在后面的函数中使用,用以配合类型系统,实现SFINAE.
// 可平凡析构则调用该函数
template <class Ty>
void destroy_one(Ty*, std::true_type) {}

// 不可平凡析构则调用该函数
template <class Ty>
void destroy_one(Ty* pointer, std::false_type)
{
	if (pointer != nullptr)
	{
		pointer->~Ty();
	}
}

// 中间层,为下面的destroy服务.
// 配合类型系统实现SFINAE,判断传入的Forward具体调用平凡析构或是非平凡析构.
template <class ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template <class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
{
	for (; first != last; ++first)
		destroy(&*first);
}

// 单个对象的析构
template <class Ty>
void destroy(Ty* pointer)
{
	destroy_one(pointer, std::is_trivially_destructible<Ty>{});
}

// 多个对象(一段范围)的析构
template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last)
{
	destroy_cat(first, last, std::is_trivially_destructible<
		        typename iterator_traits<ForwardIter>::value_type> {});
}

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // !MYTINYSTL_CONSTRUCT_H_
