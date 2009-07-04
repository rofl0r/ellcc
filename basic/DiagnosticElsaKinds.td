//==--- DiagnosticElsaKinds.td - libelsa diagnostics ------------------------===//
//
//===----------------------------------------------------------------------===//

let Component = "Elsa" in {

def err_parse_confused : Error<"confused by earlier errors, bailing out">, DefaultFatal;
def err_parse_rejected_alternative_modified_environment
    : Error<"a rejected alternative modified this environment">;
def note_parse_begin_messages_from_ambiguity
    : Note<"---- BEGIN: messages from an ambiguity ----">;
def note_parse_separator_messages_from_ambiguity
    : Note<"---- SEPARATOR: messages from an ambiguity ----">;
def note_parse_end_messages_from_ambiguity
    : Note<"---- END: messages from an ambiguity ----">;
def err_parse_discard_grouped_parameter_declarator_of_type_name
    : Error<"'%0' is the name of a type, but was used as "
            "a grouped parameter declarator; ambiguity resolution should "
            "pick a different interpretation, so if the end user ever "
            "sees this message then there's a bug in my typechecker">;

def err_undeclared_identifier : Error<"undeclared identifier '%0'">;
def err_redefinition_of_identifier : Error<"redefinition of '%0'">;
def err_unknown_type : Error<"there is no type called '%0'">;
def err_variable_used_as_type : Error<"variable name '%0' used as if it were a type">;
def err_type_used_as_variable : Error<"type name '%0' used as if it were a variable">;
def err_illegal_use_of_typedef : Error<"'%0' is a typedef-name, so cannot be used after '%1'">;
def err_not_struct_class_union : Error<"'%0' is not a struct/class/union">;
def err_no_such : Error<"no such %0: '%1'">;
def err_main_implicit_int : Warning<"obsolete use of implicit int in declaration of main()">;
def err_implicit_return : Error<"obsolete use of implicit int return type">;
def err_break : Error<"'break' not in a 'switch', 'while', 'do', or 'for' statement">;
def err_continue : Error<"'continue' not in a 'while', 'do', or 'for' statement">;

def err_specifier_invalid : Error<"cannot apply const/volatile to type '%0'">;

def err_expr_not_ice : Error<"expression is not an integer constant expression">;
def err_expr_unary_plus
    : Error<"argument to unary + must be of arithmetic, enumeration, or pointer type, not '%0'">;
def err_expr_unary_minus
    : Error<"argument to unary - must be of arithmetic or enumeration type, not '%0'">;
def err_expr_unary_bang
    : Error<"argument to unary ! must be convertible to bool; '%0' is not">;
def err_expr_unary_complement
    : Error<"argument to unary ~ must be of integer or enumeration type, not '%0'">;
def err_expr_update_overloaded
    : Error<"cannot use overloaded function name with '%0'">;
def err_expr_less_than_function
    : Error<"cannot apply '<' to a function; instead, call it or explicitly take its address">;
def err_expr_lhs_arrow_star_not_pointer
    : Error<"the left hand side of '->*' must be a pointer, not '%0'">;
def err_expr_lhs_arrow_star_not_pointer_to_class
    : Error<"the left hand side of '->*' must be a pointer to a class, not '%0'">;
def err_expr_lhs_dot_star_not_class
    : Error<"the left hand side of '.*' must be a class or reference to a class, not '%0'">;
def err_expr_rhs_dot_star_not_pointer_to_member
    : Error<"the right hand side of '.*' must be a pointer-to-member, not '%0'">;
def err_expr_lhs_dot_arrow__star_not_correct_class
    : Error<"the left hand side of %select{'->*'|'.*'}0 has type '%1'"
            ", but this is not equal to or derived from '%2'"
            ", the class whose members the right side can point at">;
def err_expr_lhs_dot_arrow__star_ambiguous
    : Error<"the left hand side of %select{'->*'|'.*'}0 has type '%1'"
            ", but this is derived from '%2'"
            " ambiguously (in more than one way)">;
def err_expr_address_of_non_lvalue
    : Error<"cannot take address of non-lvalue '%0'">;
def err_expr_dereference_void_star
    : Error<"cannot dereference type '%0'">;
def err_expr_dereference_non_pointer
    : Error<"cannot dereference non-pointer type '%0'">;
def err_expr_conditional_bool_conversion
    : Error<"cannot convert '%0' to bool for conditional of '?:'">;
def err_expr_conditional_void_throw
    : Error<"void-typed expression in '?:' must be a throw-expression">;
def err_expr_conditional_class_conversion_ambiguous
    : Error<"class-valued arguments to '?:' are ambiguously inter-convertible">;
def err_expr_conditional_class_conversion_second_to_third_ambiguous
    : Error<"in '?:', conversion of the second argument to type of the third is ambiguous">;
def err_expr_conditional_class_conversion_third_to_second_ambiguous
    : Error<"in '?:', conversion of the third argument to type of the second is ambiguous">;
def err_expr_conditional_class_incompatable_arguments
    : Error<"incompatible '?:' argument types '%0' and '%1'">;
def err_expr_sizeof_type_definition
    : Error<"cannot define types in a 'sizeof' expression">;
def err_expr_assignment_to_non_lvalue
    : Error<"the left operand of an assignment must be an lvalue">;
def warn_expr_implicit_function
    : Warning<"implicit declaration of '%0'">;

def err_namespace_unknown : Error<"namespace '%0' does not exist">;
def err_namespace_alias_redefinition
    : Error<"redefinition of namespace alias '%0' is not allowed "
            "because the new definition isn't the same as the old">;

def err_using_needs_qualified_name : Error<"a using declaration requires a qualified name">;
def err_using_template
    : Error<"you can't use a template-id (template name + template arguments) "
            "in a using-declaration">;

def err_initializer_arguments
    : Error<"expected initializer of '%0' to have 1 argument, not %1">;
def err_constructor_style_initializer_arguments
    : Error<"expected constructor-style initializer of '%0' to have 1 argument, not %1">;

def err_template_in_function_or_local_class
    : Error<"template declaration in a function or local class">;
def err_template_more_than_one_declarator
    : Error<"there can be at most one declarator in a template declaration">;
def err_template_duplicate_parameter
    : Error<"duplicate template parameter '%0'">;
def err_template_explicit_instantiation
    : Error<"explicit instantiation (without declarator) is only for class instantiations">;
def err_template_explicit_instantiation_without_class
    : Error<"explicit instantiation (without declarator) requires \"class ...\"">;
def err_template_explicit_instantiation_too_many_declarators
    : Error<"too many declarators in explicit instantiation">;
def note_template_from_template
    : Note<"from template; would be suppressed in permissive mode">;
def err_template_typename_on_qualified_name_only
    : Error<"the 'typename' keyword can only be used with a qualified name">;
def err_template_typename_required_on_qualified_name
    : Error<"dependent name '%0' used as a type, but the 'typename' keyword was not supplied">;
def err_template_complete_specialization_requires_args
    : Error<"complete specialization ('<>') requires template args">;
def err_template_unions_are_not_allowed
    : Error<"template unions are not allowed">;
def err_template_specialize_non_template
    : Error<"attempt to specialize a non-template">;
def err_template_already_implicitly_instantiated
    : Error<"'%0' has already been implicitly instantiated, "
            "so it's too late to provide an explicit specialization">;
def err_template_function_partial_specialization
    : Error<"function template partial specialization is not allowed">;
def err_template_class_declaration_must_not_have_declarators
    : Error<"template class declaration must not have declarators">;
def err_template_primary_cannot_have_template_args
    : Error<"template primary cannot have template args">;
def err_template_constant_expression
    : Error<"cannot evaluate '%0' as a template constant integer argument">;
def err_template_arguments_to_non_template_function
    : Error<"explicit template arguments were provided after '%0', "
            "but that is not the name of a template function">;
def err_template_parameter_inferred_argument
    : Error<"for parameter '%0', inferred argument '%1'"
            " does not match supplied argument '%2'">;
def err_template_parameter_incompatable_with_argument
    : Error<"during function template argument deduction: "
            "argument '%0' is incompatible with parameter '%1'">;
def err_template_arguments_do_not_bind_parameter
    : Error<"arguments do not bind template parameter '%0'">;
def err_template_too_many_arguments
    : Error<"too many template arguments to '%0'">;

def err_stdconv_cv_flag_mismatch
    : Error<"cannot convert '%0' to '%1': the source has some cv flag that the destination does not">;
def err_stdconv_cv_flag_changed_below_pointer
    : Error<"cannot convert '%0' to '%1': changed cv flags below non-const pointer">;
def err_stdconv_type_incomplete
    : Error<"cannot convert '%0' to '%1': type must be complete to strip '&'">;
def err_stdconv_rvalue_to_non_const_reference
    : Error<"cannot convert '%0' to '%1': attempt to bind an rvalue to a non-const reference">;
def err_stdconv_rvalue_to_lvalue
    : Error<"cannot convert '%0' to '%1': cannot convert rvalue to lvalue">;
def err_stdconv_different_type_constructors
    : Error<"cannot convert '%0' to '%1': different type constructors">;
def err_stdconv_unequal_function_types
    : Error<"cannot convert '%0' to '%1': unequal function types">;
def err_stdconv_unequal_array_types
    : Error<"cannot convert '%0' to '%1': unequal array types">;
def err_stdconv_not_unambiguous_base
    : Error<"cannot convert '%0' to '%1': source member's class is not an unambiguous "
            "base of destination member's class">;
def err_stdconv_virtual_base
    : Error<"cannot convert '%0' to '%1': source member's class is a virtual "
            "base of destination member's class">;
def err_stdconv_variance_not_allowed
    : Error<"cannot convert '%0' to '%1': unequal member classes in pointer-to-member that "
            "is not the topmost type">;
def err_stdconv_different_type_constructors_vs
    : Error<"cannot convert '%0' to '%1': different type constructors, %2 versus %3">;
def err_stdconv_ambiguous_base
    : Error<"cannot convert '%0' to '%1': base class is ambiguous">;
def err_stdconv_incompatable_atomic_types
    : Error<"cannot convert '%0' to '%1': incompatible atomic types">;

def err_class_memberless_initialization : Error<"can't initialize memberless aggregate '%0'">;
def err_class_no_member : Error<"'%0' has no member named '%1'">;
def err_class_member_storage_class
    : Error<"members cannot be marked 'auto', 'extern', or 'register'">;
def err_class_not_constructor_or_member
    : Error<"%0 are only valid for class member functions (constructors in particular)">;
def err_class_not_constructor
    : Error<"%0 are only valid for class constructors">;
def err_class_initialize_static_data_or_member_function
    : Error<"you can't initialize static data nor member functions in a ctor member init list">;
def err_class_initialize_non_base_class
    : Error<"'%0' is not a base class%select{|, nor a data member,}1"
            " so it cannot be initialized here">;
def err_class_direct_and_indirect_base
    : Error<"'%0' is both a direct non-virtual base, "
            "and an indirect virtual base; therefore the initializer "
            "is ambiguous (there's no quick fix--you have to change "
            "your inheritance hierarchy or forego initialization)">;
def err_class_define_class_in_friend_definition
    : Error<"you cannot define a class in a friend definition">;
def err_class_friend_declaration_must_appear_in_class_scope
    : Error<"friend declaration must appear in class scope">;
def err_class_specifier_name_with_template_arguments
    : Error<"class specifier name can have template arguments only in a templatized definition">;
def err_class_redefinition
    : Error<"there is already a %0, but here you're defining a %1 %2">;
def err_class_already_defined
    : Error<"%0 has already been defined">;
def err_class_not_found
    : Error<"no class called '%0' was found">;
def err_class_not_base_class
    : Error<"'%0' is not a class or struct or union, so it cannot be used as a base class">;
def err_class_define_non_static_data_member
    : Error<"cannot define non-static data member '%0'">;
def err_class_destructor_must_be_class_member
    : Error<"destructors must be class members">;
def err_class_constructor_must_be_class_member
    : Error<"constructors must be class members (and implicit int is not supported)">;
def err_class_destructor_name_must_match_class_name
    : Error<"destructor name '%0' must match the class name '%1'">;
def err_class_constructor_name_must_match_class_name
    : Error<"constructor name '%0' must match the class name '%1'">;
def err_class_conversion_operator_arguments
    : Error<"conversion operator cannot accept arguments">;
def err_class_bitfield_size_must_be_constant
    : Error<"bitfield size must be a constant">;
def err_class_this_only_in_non_static_method
    : Error<"can only use 'this' in a nonstatic method">;
def err_class_destructor_arguments
    : Error<"call to destructor must have no arguments">;
def err_class_used_as_function
    : Error<"object of type '%0' used as a function, but it has no operator() declared">;
def err_class_receiver_argument
    : Error<"cannot convert argument type '%0' to receiver parameter type '%1'">;
def err_class_field_qualifier_class_or_namespace
    : Error<"in '%0', when '%1' is found in the current scope, it must be a class or namespace, not '%2'">;
def err_class_field_qualifier_class
    : Error<"in '%0', when '%1' is found in the class of '%2', it must be a class, not '%3'">;
def err_class_field_qualifier_class_two_lookups
    : Error<"in '%0', '%1' was found in the current scope as '%2', and also in the class of '%3'"
            " as '%4', but they must be the same">;
def err_class_field_unqualified_lookup
    : Error<"in '%0', when '%1' is found in the current scope, it must be a class, not '%3'">;
def error_class_field_not_member
    : Error<"field '%0' is not a class member">;
def error_class_field_wrong_member
    : Error<"field '%0' is a member of '%1', which is not a base class of '%2'">;
def error_class_field_ambiguous
    : Error<"field '%0' ambiguously refers to elements of multiple base class sub-objects">;
def error_class_field_typedef
    : Error<"field '%0' is a typedef">;

def err_enum_not_defined
    : Error<"there is no enum called '%0'">;
def err_enum_is_not_an_enum
    : Error<"'%0' is not an enum">;

def err_array_of_references
    : Error<"cannot create an array of references">;
def err_array_of_void
    : Error<"cannot create an array of void">;
def err_array_of_functions
    : Error<"cannot create an array of functions">;
def err_array_new_size_required
    : Error<"new[] must have an array size specified">;
def err_array_size_must_be_positive
    : Error<"array size must be positive (it is %0)">;
def err_array_size_must_be_non_negative
    : Error<"array size must be non-negative (it is %0)">;
def err_array_size_must_have_integral_type
    : Error<"array size must have integral type">;

def err_operator_must_be_function
    : Error<"operators must be functions">;
def err_operator_static_member_function
    : Error<"operator member functions (other than new/delete) cannot be static">;
def err_operator_must_be_member_function
    : Error<"%0 must be a member function">;
def err_operator_parameters
    : Error<"%0 must have %select{one parameter|two parameters|one or two parameters}1">;
def note_including_receiver_object
    : Note<"(including receiver object)">;
def err_operator_incdec_argument_must_be_integer
    : Error<"%select{second |}0parameter of %1 must have type 'int', not '%2', if it is present">;
def err_operator_cannot_have_default_arguments
    : Error<"%0 cannot have default arguments">;
def err_operator_pseudo_destructor
    : Error<"the right hand side of '.' or '->' must be of the form '~ identifier'"
            " if the left hand side is not a class">;
def note_operator_left_type : Note<"the type of the left hand side is '%0'">;
def err_operator_pseudo_destructor_templatized_qualifier
    : Error<"cannot have templatized qualifier as the second-to-last element "
            " of the right hand side of '.' or '->' when the left hand side is not a class">;
def err_operator_pseudo_destructor_qualified
    : Error<"in a '.' or '->' expression, when the left hand side is not a class, "
            " a qualified right hand side must be of the form Q :: t1 :: ~t2 where "
            " t1 and t2 are the same type, but t1 is '%0' and t2 is '%1'">;
def err_operator_pseudo_destructor_type_mismatch
    : Error<"in a '.' or '->' expression, when the left hand side is not a class, "
            "its type must be the same (modulo cv qualifiers) as the right hand side "
            "but the right hasnd side type is %0">;

def err_function_wrong_type
    : Error<"you can't use an expression of type '%0' as a function">;
def err_function_const_volatile
    : Error<"cannot have const/volatile on nonmember functions">;
def err_function_void_parameter
    : Error<"cannot have parameter of type 'void', unless it is "
            "the only parameter, has no parameter name, and has "
            "no default value">;
def err_function_return_value_in_void_function
    : Error<"returning a value in a 'void' function">;
def warn_function_return_value_in_void_function
    : Warning<"returning a value in a 'void' function">;
def err_function_no_return_value
    : Error<"no return value for a non-'void' function">;
def err_function_argument_to_parameter_conversion
    : Error<"cannot convert argument type '%0' to parameter %1 type '%2'">;
def err_function_argument_missing
    : Error<"no argument supplied for parameter %0">;
def err_function_too_many_arguments
    : Error<"too many arguments supplied">;
def err_function_no_function
    : Error<"there is no function called '%0'">;
def note_function_dependent_lookup
    : Note<"due to nondependent lookup, did not search bases: %0">;
def err_function_call_site_lookup
    : Error<"call site name lookup for '%0' failed to yield any candidates">;
def note_function_non_template_given_template_arguments
    : Note<"non-template given template arguments">;
def note_function_incompatible_explicit_template_arguments
    : Note<"incompatible explicit template arguments">;
def note_function_incompatible_call_site_arguments
    : Note<"incompatible call site arguments">;
def note_function_could_not_deduce_template_parameters
    : Note<"could not deduce all template parameters">;

def err_label_duplicate : Error<"'%0' is a duplicate label">;
def note_label_duplicate : Note<"the duplicated label:">;
def err_label_not_found : Error<"label '%0' does not exist">;

def err_overload_not_function
    : Error<"the name '%0' is overloaded, but the type '%1' isn't even a function;"
            " it must be a function and match one of the overloadings">;
def err_overload_does_not_match
    : Error<"the name '%0' is overloaded, but the type '%1' doesn't match any of the"
            " %2 declared overloaded instances">;
def err_overload_conversion_operator
    : Error<"cannot find conversion operator yielding '%0'">;

def err_reference_pointer_to_reference
    : Error<"cannot create a pointer to a reference ('%0')">;
def err_reference_to_reference
    : Error<"cannot create a reference to a reference ('%0')">;
def err_reference_pointer_to_member_to_reference
    : Error<"cannot create a pointer-to-member to a reference">;
def err_reference_pointer_to_member_to_void
    : Error<"cannot create a pointer-to-member to 'void'">;

def error_pointer_to_member_type
    : Error<"cannot find type '%0' for pointer-to-member">;
def error_pointer_to_member_type_does_not_refer_to_a_class
    : Error<"in pointer-to-member, '%0' does not refer to a class nor is a type variable">;

def err_constant_expr_ambiguous
    : Error<"ambiguous expression being constant evaluated">;
def err_constant_expr_failed_type_checking
    : Error<"constant expression failed type checking">;
def err_constant_expr_constructor_integer_only
    : Error<"can only constant evaluate constructors for integer types">;
def err_constant_expr_keyword_cast
    : Error<"cannot constant evaluate a keyword_cast">;
def err_constant_expr_can_not_evaluate
    : Error<"can not evaluate constant expression">;
def err_constant_expr_non_const_variable
    : Error<"can not evaluate non-constant variable">;
def err_constant_expr_dynamic_cast
    : Error<"can not evaluate dynamic_cast">;
def err_constant_expr_unhandled
    : Error<"can not evaluate unhandled case in constEvalAddr">;
def err_constant_expr_bad_cast
    : Error<"in constant expression, can only cast to arithmetic or pointer types">;
def err_constant_expr_tilda_float
    : Error<"cannot apply '~' to float types">;
def err_constant_expr_binary
    : Error<"can not evaluate binary operator">;
def err_constant_expr_division_by_zero
    : Error<"division by zero">;
def err_constant_expr_mod_by_zero
    : Error<"mod by zero">;
def err_constant_expr_mod_float
    : Error<"cannot apply '%' to float types">;
def err_constant_expr_shift_float
    : Error<"cannot shift float types">;
def err_constant_expr_and_float
    : Error<"cannot apply '&' to float types">;
def err_constant_expr_xor_float
    : Error<"cannot apply '^' to float types">;
def err_constant_expr_or_float
    : Error<"cannot apply '|' to float types">;

def err_asm_wide_string_literal : Error<"wide string literal in 'asm'">;

def err_escape_hex_escape_sequence
    : Error<"hex escape sequence must begin with hex digit">;
def err_escape_universal_escape_sequence
    : Error<"universal escape sequence must have %0 hex digits">;
def warn_escape_unknown
    : Warning<"unrecognized escape sequence: '\\%0' treating as same as '%0'">;

def err_char_literal_with_no_characters
    : Error<"character literal with no characters">;
def warn_char_literal_multi_character
    : Warning<"multi-character literals not properly implemented">;
def err_char_literal_no_wchar_t
    : Error<"you must #include <stddef.h> before using wchar_t">;

def err_cast_argument_count
    : Error<"function-style cast to '%0' must have zere or one argument (not %1)">;
def err_cast_type_definition
    : Error<"cannot define types in a cast">;
def err_cast_type_definition_keyword
    : Error<"cannot define types in a '%0'">;

def err_delete_ambiguous
    : Error<"attempt to apply 'delete' to object of type '%0'"
            ", but it has more than one conversion "
            "operator yielding a pointer type">;
def err_delete_no_pointer_conversion
    : Error<"attempt to apply 'delete' to object of type '%0'"
            ", but it has no conversion operator yielding a pointer type">;

def err_initializer_conversion
    : Error<"cannot convert initializer type '%0' to type '%1'">;

def err_test_checktype
    : Error<"checkType: '%0' != '%1'">;
def err_test_invalid
    : Error<"invalid call to '%0'">;
def err_test_overload_line
    : Error<"expected overload to choose the function on line %0 , but it chose line %1">;
def err_test_overload_not_function
    : Error<"expected overload to choose a function, but it chose a non-function">;
def err_test_function_not_defined
    : Error<"expected to be calling a defined function">;
def err_test_function_line
    : Error<"expected to call the function on line %0 , but it chose line %1">;
def err_test_mtype_arguments
    : Error<"__test_mtype requires at least three arguments">;
def err_test_mtype_third_argument_non_constant
    : Error<"the third argument to __test_mtype must be a constant expression">;
def err_test_mtype_flags
    : Error<"invalid flags value for __test_mtype">;
def err_test_mtype_argument_count
    : Error<"__test_mtype requires either four or an odd number of arguments">;
def err_test_mtype_argument_string
    : Error<"__test_mtype argument %0 must be a string literal">;
def err_test_mtype_not_bound
    : Error<"__test_mtype: '%0' is not bound">;
def err_test_mtype_unexpected_binding_kind
    : Error<"__test_mtype: unexpected binding kind '%0'">;
def err_test_mtype_unexpected_binding
    : Error<"__test_mtype: expected '%0' to be bound to '%1'"
            " but it was actually bound to '%2'">;
def err_test_mtype_int_binding
    : Error<"__test_mtype: '%0' was bound to an int, but the provided "
            "expression is not a constant">;
def err_test_mtype_bindings
    : Error<"__test_mtype: call site supplied %0 bindings%select{|s}1, "
            " but match yielded %2">;
def err_test_mtype_succeeded
    : Error<"__test_mtype: succeeded, but failure was expected">;
def err_test_mtype_failed
    : Error<"__test_mtype: failed, but success was expected">;
def err_test_stdconv_getstandardconversion_error
    : Error<"getStandardConversion(%0, %1, %2) yielded '%3', but I expected '%4'">;
def err_test_stdconv_getstandardconversion_warning
    : Error<"getStandardConversion(%0, %1, %2) yielded '%3'">;
}
