/*===---- unwind.h - Stack unwinding ----------------------------------------===
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */

/* NOTE: This file is incomplete and only contains the declarations
 * necessary to make libcxx and libcxxabi compiler. The underlying
 * unwind library has not been implemented.
 */
#ifndef _UNWIND_H_
#define _UNWIND_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned _Unwind_Word __attribute__((__mode__(__word__)));
typedef unsigned _Unwind_Ptr __attribute__((__mode__(__pointer__)));

typedef unsigned _Unwind_Exception_Class __attribute__((__mode__(__DI__)));

typedef enum
{
  _URC_NO_REASON = 0,
  _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
  _URC_FATAL_PHASE2_ERROR = 2,
  _URC_FATAL_PHASE1_ERROR = 3,
  _URC_NORMAL_STOP = 4,
  _URC_END_OF_STACK = 5,
  _URC_HANDLER_FOUND = 6,
  _URC_INSTALL_CONTEXT = 7,
  _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

class _Unwind_Exception;

typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code,
                                             struct _Unwind_Exception *);
class _Unwind_Exception {
public:
    _Unwind_Exception_Class exception_class;
    _Unwind_Exception_Cleanup_Fn exception_cleanup;
};

extern _Unwind_Reason_Code _Unwind_SjLj_RaiseException(struct _Unwind_Exception *);
extern _Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception *);
extern void _Unwind_DeleteException(struct _Unwind_Exception *);

struct _Unwind_Context;

extern void _Unwind_SetGR(struct _Unwind_Context *, int, _Unwind_Word);
extern void _Unwind_SetIP(struct _Unwind_Context *, _Unwind_Ptr);
extern _Unwind_Ptr _Unwind_GetIP(struct _Unwind_Context *);
extern void *_Unwind_GetLanguageSpecificData(struct _Unwind_Context *);
extern _Unwind_Ptr _Unwind_GetRegionStart(struct _Unwind_Context *);

typedef int _Unwind_Action;
#define _UA_SEARCH_PHASE  0x01
#define _UA_CLEANUP_PHASE 0x02
#define _UA_HANDLER_FRAME 0x04
#define _UA_FORCE_UNWIND  0x08
#define _UA_END_OF_STACK  0x10

#ifdef __cplusplus
}
#endif

#endif
