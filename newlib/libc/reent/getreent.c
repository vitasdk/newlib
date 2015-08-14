/* default reentrant pointer when multithread enabled */

#include <_ansi.h>
#include <reent.h>

#ifdef __getreent
#undef __getreent
#endif

#if 0
struct _reent *
_DEFUN_VOID(__getreent)
{
  return _impure_ptr;
}
#endif