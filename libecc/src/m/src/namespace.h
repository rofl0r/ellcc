/* $NetBSD: namespace.h,v 1.3 2010/04/23 19:17:07 drochner Exp $ */

// RICH: #define atan2 _atan2
// RICH: #define atan2f _atan2f
// RICH: #define hypot _hypot
// RICH: #define hypotf _hypotf

// RICH: #define exp _exp
// RICH: #define expf _expf
// RICH: #define log _log
// RICH: #define logf _logf

#if 0 /* not yet - need to review use in machdep code first */
#define sin _sin
#define sinf _sinf
#define cos _cos
#define cosf _cosf
#define finite _finite
#define finitef _finitef
#endif /* notyet */
// RICH: #define sinh _sinh
// RICH: #define sinhf _sinhf
// RICH: #define cosh _cosh
// RICH: #define coshf _coshf
// RICH: #define asin _asin
// RICH: #define asinf _asinf

// RICH: #define casin _casin
// RICH: #define casinf _casinf
// RICH: #define catan _catan
// RICH: #define catanf _catanf

// RICH: #define scalbn _scalbn
// RICH: #define scalbnf _scalbnf
