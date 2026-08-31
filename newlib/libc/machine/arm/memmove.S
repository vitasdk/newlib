/* The structure of the following #if #else #endif conditional chain
   must match the chain in memmove-stub.c.  */

#include "arm-acle-compat.h"

#if __ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A' \
    && defined (__ARM_FEATURE_UNALIGNED)
#include "memmove-armv7a.S"

#else
  /* Defined in memmove-stub.c.  */

#endif
