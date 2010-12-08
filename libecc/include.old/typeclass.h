#ifndef _TYPECLASS_H_
#define _TYPECLASS_H_

/** Values returned by __builtin_type_class()
 */
enum type_class {
    no_type_class = -1,                 ///< None.
    void_type_class,                    ///< Void.
    integer_type_class,                 ///< Any integer.
    char_type_class,                    ///< Any character.
    enumeral_type_class,                ///< Any enumeration.
    boolean_type_class,                 ///< Bool.
    pointer_type_class,                 ///< Pointer.
    reference_type_class,               ///< A reference.
    offset_type_class,                  ///< An offset.
    real_type_class,                    ///< Floating point.
    complex_type_class,                 ///< A complex number.
    function_type_class,                ///< A function.
    method_type_class,                  ///< A method.
    record_type_class,                  ///< A struct/class.
    union_type_class,                   ///< A union.
    array_type_class,                   ///< An array.
    string_type_class,                  ///< A string.
    lang_type_class
};

#endif
