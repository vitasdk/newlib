/* The structure of the following #if #else #endif conditional chain
   must match the chain in memmove.S.  */

#include "arm-acle-compat.h"

#if __ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A' \
    && defined (__ARM_FEATURE_UNALIGNED)
  /* Defined in memmove-armv7a.S.  */

#else
# include "../../string/memmove.c"
#endif
