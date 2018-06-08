//
//  randlibhw.c
//  
//
//  Created by Saman Hashemipour on 6/6/18.
//

#include "randlib.h"
#include <immintrin.h>


extern unsigned long long
hardware_rand64 (void)
{
    unsigned long long int x;
    while (! _rdrand64_step (&x))
        continue;
    return x;
}
