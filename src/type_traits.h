#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息

// use standard header for type_traits
#include <type_traits>

namespace mystl
{

// helper struct

template <class T, T v>
struct m_integral_constant
{
  static constexpr T value = v;
};

//为bool值做特化
//第一个参数为bool
//bool b 即为原模板的第二个模板参数
template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;

using m_true_type = m_bool_constant<true>;
using m_false_type = m_bool_constant<false>;

/*****************************************************************************************/
// type traits

// is_pair

// --- forward declaration begin
template <class T1, class T2>
struct pair;
// --- forward declaration end

template <class T>
struct is_pair : mystl::m_false_type {};

template <class T1, class T2>
struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};

} // namespace mystl

#endif // !MYTINYSTL_TYPE_TRAITS_H_