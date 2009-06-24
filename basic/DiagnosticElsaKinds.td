//==--- DiagnosticElsaKinds.td - libelsa diagnostics ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

let Component = "Elsa" in {

def err_confused : Error<"confused by earlier errors, bailing out">, DefaultFatal;

def err_undeclared_identifier : Error<"undeclared identifier '%0'">;
def err_redefinition_of_identifier : Error<"redefinition of '%0'">;

def err_expr_not_ice : Error<"expression is not an integer constant expression">;

def err_typecheck_no_member : Error<"'%0' has no member named '%1'">;

def err_unknown_namespace : Error<"namespace '%0' does not exist">;

def err_using_needs_qualified_name : Error<"a using declaration requires a qualified name">;

def err_memberless_aggregate_initialization : Error<"can't initialize memberless aggregate '%0'">;

def err_rejected_alternative_modified_environment
    : Error<"a rejected alternative modified this environment">;
def err_begin_messages_from_ambiguity
    : Error<"---- BEGIN: messages from an ambiguity ----">;
def err_separator_messages_from_ambiguity
    : Error<"---- SEPARATOR: messages from an ambiguity ----">;
def err_end_messages_from_ambiguity
    : Error<"---- END: messages from an ambiguity ----">;

def err_member_storage_class
    : Error<"members cannot be marked 'auto', 'extern', or 'register'">;

def err_template_explicit_instantiation
    : Error<"explicit instantiation (without declarator) is only for class instantiations">;
def err_template_explicit_instantiation_without_class
    : Error<"explicit instantiation (without declarator) requires \"class ...\"">;
def err_template_explicit_instantiation_too_many_declarators
    : Error<"too many declarators in explicit instantiation">;

def err_class_not_constructor_or_member
    : Error<"%0 are only valid for class member functions (constructors in particular)">;
def err_class_not_constructor
    : Error<"%0 are only valid for class constructors">;
def err_class_initialize_static_data_or_member_function
    : Error<"you can't initialize static data nor member functions in a ctor member init list">;
def err_class_initialize_non_base_class
    : Error<"'%0' is not a base class%select{|, nor a data member,}1"
            " so it cannot be initialized here">;

}
