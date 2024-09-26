#ifndef MYTINYSTL_EXCEPTDEF_H_
#define MYTINYSTL_EXCEPTDEF_H_

#include <stdexcept>

#include <cassert>

namespace mystl
{

// 符号 / 用于将宏定义的多行内容连接到一起,提高可读性
// 例如下面的宏 MYSTL_DEBUG(expr) 就会被替换成 assert(expr)

// 在调试时检查一个条件表达式。如果条件不成立（为假），程序将终止并显示错误信息
#define MYSTL_DEBUG(expr) \
  assert(expr)

// 如果 expr 为真，则抛出一个 std::length_error 异常，异常信息为 what
#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(what)

// 如果 expr 为真，则抛出一个 std::out_of_range 异常，异常信息为 what
#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

// 如果 expr 为真，则抛出一个 std::runtime_error 异常，异常信息为 what
#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)

} // namepsace mystl

#endif // !MYTINYSTL_EXCEPTDEF_H_

