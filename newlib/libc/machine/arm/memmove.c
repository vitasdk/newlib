#if defined (__vita__)
# include <psp2/kernel/clib.h>
void* memmove (void *dest, const void *source, size_t n){
   return sceClibMemmove(dest, source, n);
}
#else
# include "../../string/memmove.c"
#endif
