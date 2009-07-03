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

def err_namespace_unknown : Error<"namespace '%0' does not exist">;

def err_using_needs_qualified_name : Error<"a using declaration requires a qualified name">;

def err_initializer_arguments
    : Error<"expected initializer of '%0' to have 1 argument, not %1">;
def err_constructor_style_initializer_arguments
    : Error<"expected constructor-style initializer of '%0' to have 1 argument, not %1">;

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
}
