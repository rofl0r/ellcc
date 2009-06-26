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

def err_undeclared_identifier : Error<"undeclared identifier '%0'">;
def err_redefinition_of_identifier : Error<"redefinition of '%0'">;
def err_unknown_type : Error<"there is no type called '%0'">;
def err_variable_used_as_type : Error<"variable name '%0' used as if it were a type">;

def err_specifier_invalid : Error<"cannot apply const/volatile to type '%0'">;

def err_expr_not_ice : Error<"expression is not an integer constant expression">;

def err_namespace_unknown : Error<"namespace '%0' does not exist">;

def err_using_needs_qualified_name : Error<"a using declaration requires a qualified name">;

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
def err_class_specifier_name_with_template_arguments
    : Error<"class specifier name can have template arguments only in a templatized definition">;

}
