/* The structure of the following #if #else #endif conditional chain
   must match the chain in memcmp.S.  */

#include "arm-acle-compat.h"

#if __ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A' \
    && defined (__ARM_FEATURE_UNALIGNED)
  /* Defined in memcmp-armv7a.S.  */

#else
# include "../../string/memcmp.c"
#endif
