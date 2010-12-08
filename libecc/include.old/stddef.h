/** Common definitions.
 *      ptrdiff_t       size_t          wchar_t         NULL
 *      offsetof(type, member-designator)
 */

#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/** A type to represent the size of an object.
 * __SIZE_TYPE__ is defined by the compiler.
 */
typedef __SIZE_TYPE__ size_t;                   
/** A type to represent the difference between two pointers.
 * __PTRDIFF_TYPE__ is defined by the compiler.
 */
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#ifndef __cplusplus     // wchar_t is a C++ keyword.
/** A type to represent a wide character.
 * __WCHAR_TYPE__ is defined by the compiler.
 */
typedef __WCHAR_TYPE__ wchar_t;
#endif
/** A pointer to nothing.
 */
#define NULL 0
/** Get the offset of a struct/union member.
 * @param type The struct/union.
 * @param member The member desired.
 * @return The offset to the member.
 */
#define offsetof(type, member) ((size_t)&((type*)NULL)->member)

#ifdef __cplusplus
}
#endif

#endif
