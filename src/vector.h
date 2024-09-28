#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
//
// 异常保证：
// mystl::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"

namespace mystl
{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// 模板类: vector
// 模板参数 T 代表类型
template <class T>
class vector
{
	// 传入的类型 T 为 bool 类型时将导致断言失败
	// 在 mystl 中,不支持 vector<bool>, vector<bool>常用于位压缩而非存储bool类型的变量.
	static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");

public:
	// vector 的嵌套型别定义

	// 用于指代分配器的类型
	typedef mystl::allocator<T>                      allocator_type;
	// 用于数据分配的特定分配器
	typedef mystl::allocator<T>                      data_allocator;

	typedef typename allocator_type::value_type      value_type;
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::size_type       size_type;
	typedef typename allocator_type::difference_type difference_type;

	typedef value_type*                              iterator;
	typedef const value_type*                        const_iterator;
	typedef mystl::reverse_iterator<iterator>        reverse_iterator;
	typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

	allocator_type get_allocator() { return data_allocator(); }

private:
	iterator begin_;  // 表示目前使用空间的头部
	iterator end_;    // 表示目前使用空间的尾部
	iterator cap_;    // 表示目前存储空间的尾部

public:
	// 构造,拷贝,移动,析构





private:
	/*****************************辅助函数们***********************************/

	/********用于初始化空间以及销毁空间的函数,包括初始化、构造、清理和内存分配策略******/

	// 尝试初始化一个向量的基础空间,尝试分配一块初始的内存（通常为 16 个元素）
	// 失败不抛异常而是设置指针为nullptr;
	void      try_init() noexcept;

	// 初始化向量的内部空间，按给定的大小和容量进行分配,为 fill_init 和 range_init 服务
	// 分配 cap 大小的内存，并将 size 设定为当前向量的元素数量
	// 分配通常在 try 块中进行，如果分配成功，则初始化 begin_、end_ 和 cap_ 指针
	void      init_space(size_type size, size_type cap);

	// 使用指定的值初始化一个包含 n 个元素的向量
	// 函数调用 init_space 来分配内存，同时使用 mystl::uninitialized_fill_n 来对新分配的内存区域填充指定的值
	// 方法确保了未初始化的内存部分只在需要时才被构造
	void      fill_init(size_type n, const value_type& value);

	// 使用迭代器范围内的元素初始化向量
	// 模板函数计算给定迭代器范围的大小，并利用 init_space 分配合适的内存
	// 然后使用 mystl::uninitialized_copy 将迭代器范围内的元素复制到向量的内部结构中
	template <class Iter>
	void      range_init(Iter first, Iter last);

	// 销毁位于给定范围内的对象并释放相应的内存
	// 函数负责调用 data_allocator::destroy 来销毁 first 到 last 之间的元素
	// 并调用 data_allocator::deallocate 来释放分配的内存空间
	// 这是一个清理操作，确保在向量不再需要这个内存时释放它，减少内存泄漏的风险
	void      destroy_and_recover(iterator first, iterator last, size_type n);

	// 计算增长的长度, 动态数组的核心功能
	// 主要功能是根据当前容量和希望增加的大小来计算新的容器容量
	size_type get_new_cap(size_type add_size);

	/*****************************用于赋值的函数们***********************************/

	// 将当前 vector 的所有元素填充为给定的值 value，并且为 vector 扩展大小以容纳 n 个新元素
	void      fill_assign(size_type n, const value_type& value);

	// 为输入迭代器提供支持, 该模板函数将范围 [first, last) 的元素复制到当前 vector 中
	template <class IIter>
	void      copy_assign(IIter first, IIter last, input_iterator_tag);

	// 为前向迭代器提供支持 该模板函数将范围 [first, last) 的元素复制到当前 vector 中
	template <class FIter>
	void      copy_assign(FIter first, FIter last, forward_iterator_tag);

	/***************************用于重新分配内存的函数*******************************/

	// 在pos指定的位置构造(emplace原地构造),变长参数模板为所要提供的参数
	// 需要插入新元素而当前存储空间不足时，函数将负责重新分配内存，确保足够的空间来容纳新元素，并将其他元素移动到合适的位置
	template <class... Args>
	void       reallocate_emplace(iterator pos, Args&& ...args);

	// 指定位置 pos 插入一个元素 value，如果当前的容量不足以容纳新增的元素，则会进行内存的重新分配
	void       reallocate_insert(iterator pos, const value_type& value);

	/******************************用于插入值的函数*********************************/

	// 在向量中的指定位置 pos 插入 n 个值为 value 的元素
	// 返回插入后新位置的迭代器
	iterator  fill_insert(iterator pos, size_type n, const value_type& value);

	// 在向量的指定位置 pos 插入范围 [first, last) 的元素
	// first 和 last 是输入迭代器，表示要插入元素的范围
	template <class IIter>
	void      copy_insert(iterator pos, IIter first, IIter last);

	/******************************用于收缩到适合大小的函数*********************************/

	// 重新分配内存并调整现有元素的存放位置，以适应新的大小要求
	void      reinsert(size_type size);
};

// 辅助函数们

// try_init 函数, 若分配失败则忽略, 不抛异常
template <class T>
void vector<T>::try_init() noexcept
{
	try
	{
		begin_ = data_allocator::allocate(16);
		end_ = begin_;
		cap_ = begin_ + 16;
	}
	catch (...)
	{

	}
}

} // namespace mystl
#endif // !MYTINYSTL_VECTOR_H_

