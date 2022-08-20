#if defined (__vita__)
# include <psp2/kernel/clib.h>
void memset (void *dest, int c, size_t n){
   return sceClibMemset(dest, c, n);
}
#else
# include "../../string/memset.c"
#endif
