/* The structure of the following #if #else #endif conditional chain
   must match the chain in memcmp-stub.c.  */

#include "arm-acle-compat.h"

#if __ARM_ARCH >= 7 && __ARM_ARCH_PROFILE == 'A' \
    && defined (__ARM_FEATURE_UNALIGNED)
#include "memcmp-armv7a.S"

#else
  /* Defined in memcmp-stub.c.  */

#endif
