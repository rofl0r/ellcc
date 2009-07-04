//==--- DiagnosticCommonKinds.td - common diagnostics ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Common Helpers
//===----------------------------------------------------------------------===//

let Component = "Common" in {

// RICH: start of used
def note_definition : Note<"definition of '%0':">;
def note_previous_definition : Note<"previous definition is here:">;
def err_deprecated_error : Error<"deprecated error %0:%1: %2">;
def err_deprecated_warning : Warning<"deprecated warning %0:%1: %2">;
def err_parse_error : Error<"parse error (state %0) at %1">;
def note_parse_error_token_info : Note<"expected one of these tokens: %0">;
def note_instantiated_from : Note<"instantiated from:">;
// RICH: end of used

def note_previous_declaration : Note<"previous declaration is here:">;
def note_parameter_declaration : Note<"parameter declaration is here:">;
def note_previous_implicit_declaration : Note<
  "previous implicit declaration is here:">;
def note_previous_use : Note<"previous use is here:">;
def note_duplicate_case_prev : Note<"previous case defined here:">;
def note_forward_declaration : Note<"forward declaration of %0">;
def note_type_being_defined : Note<
  "definition of %0 is not complete until the closing '}'">;
/// note_matching - this is used as a continuation of a previous diagnostic,
/// e.g. to specify the '(' when we expected a ')'.
def note_matching : Note<"to match this '%0'">;
def note_candidate : Note<"candidate '%0'">;

def note_using_decl : Note<"using">;
def note_also_found_decl : Note<"also found">;

// Parse && Lex
def err_expected_colon : Error<"expected ':'">;

// Parse && Sema
def err_no_declarators : Error<"declaration does not declare anything">;
def err_param_redefinition : Error<"redefinition of parameter %0">;
def err_invalid_storage_class_in_func_decl : Error<
  "invalid storage class specifier in function declarator">;
def err_expected_namespace_name : Error<"expected namespace name">;

// Sema && Lex
def ext_longlong : Extension<
  "'long long' is an extension when C99 mode is not enabled">;
def warn_integer_too_large : Warning<
  "integer constant is too large for its type">;
def warn_integer_too_large_for_signed : Warning<
  "integer constant is so large that it is unsigned">;

// Sema && AST
def note_invalid_subexpr_in_ice : Note<
  "subexpression not valid in an integer constant expression">;

// clang-cc
def err_pp_I_dash_not_supported : Error<
  "-I- not supported, please use -iquote instead">;

}
