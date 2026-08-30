/* The structure of the following #if #else #endif conditional chain
   must match the chain in memset.c.  */

#include "arm-acle-compat.h"

#if __ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A' \
    && defined (__ARM_FEATURE_UNALIGNED)
#include "memset-armv7a.S"

#else
  /* Defined in memset.c.  */

#endif
