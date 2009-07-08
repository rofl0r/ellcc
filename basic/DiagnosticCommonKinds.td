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

def note_definition : Note<"definition of '%0':">;
def note_previous_definition : Note<"previous definition is here:">;
def err_deprecated_error : Error<"deprecated error %0:%1: %2">;
def err_deprecated_warning : Warning<"deprecated warning %0:%1: %2">;
def err_parse_error : Error<"parse error (state %0) at %1">;
def note_parse_error_token_info : Note<"expected one of these tokens: %0">;
def note_instantiated_from : Note<"instantiated from:">;
def note_previous_declaration : Note<"previous declaration is here:">;
def note_conflicting_declaration : Note<"conflicting declaration is here:">;
/// note_matching - this is used as a continuation of a previous diagnostic,
/// e.g. to specify the '(' when we expected a ')'.
def note_matching : Note<"to match this '%0'">;
def note_candidate : Note<"candidate '%0'">;
def ext_longlong : Extension<
  "'long long' is an extension when C99 mode is not enabled">;
def warn_integer_too_large : Warning<
  "integer constant is too large for its type">;
def warn_integer_too_large_for_signed : Warning<
  "integer constant is so large that it is unsigned">;
def err_expected_colon : Error<"expected ':'">;
def note_parameter_declaration : Note<"parameter declaration is here:">;
def note_further_errors : Note<"this may be the cause of further errors below">;

}
