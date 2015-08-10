/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __MBED_ALLOC_H
#define __MBED_ALLOC_H

#if defined(__ARMCC_VERSION)
#   include <rt_sys.h>
#   define PREFIX(x)    _sys##x
#   define OPEN_MAX     _SYS_OPEN
#   ifdef __MICROLIB
#       pragma import(__use_full_stdio)
#   endif

#elif defined(__ICCARM__)
#   include <yfuns.h>
#   define PREFIX(x)        _##x
#   define OPEN_MAX         16

#   define STDIN_FILENO     0
#   define STDOUT_FILENO    1
#   define STDERR_FILENO    2

#else
#   include <sys/syslimits.h>
#   define PREFIX(x)    x
#endif

#ifndef pid_t
 typedef int pid_t;
#endif
#ifndef caddr_t
 typedef char * caddr_t;
#endif

#ifndef SBRK_ALIGN
#define SBRK_ALIGN 4U
#endif
#if (SBRK_ALIGN & (SBRK_ALIGN-1))
#error SBRK_ALIGN must be a power of 2
#endif

#ifndef SBRK_INC_MIN
#define SBRK_INC_MIN (SBRK_ALIGN)
#endif

#ifndef KRBS_ALIGN
#define KRBS_ALIGN 4U
#endif
#if (KRBS_ALIGN & (KRBS_ALIGN-1))
#error KRBS_ALIGN must be a power of 2
#endif

#ifndef KRBS_INC_MIN
#define KRBS_INC_MIN (KRBS_ALIGN)
#endif

#ifndef MBED_HEAP_SIZE
extern uint32_t __heap_size;
// Extract linker heap size parameter
#define MBED_HEAP_SIZE ((ptrdiff_t) &__heap_size)
#endif

#ifdef __ARMCC_VERSION
	#define __mbed_sbrk_start (Image$$ARM_LIB_HEAP$$Base)
	#define __mbed_krbs_start (Image$$ARM_LIB_HEAP$$ZI$$Limit)
	#define __heap_size (Image$$ARM_LIB_HEAP$$ZI$$Length)
	extern unsigned int __mbed_sbrk_start;
	extern unsigned int __mbed_krbs_start;
	extern unsigned int __heap_size;
	#pragma import(__use_two_region_memory)
#else
	extern uint32_t __mbed_krbs_start;
	extern uint32_t __mbed_sbrk_start;
#endif

#ifdef __cplusplus
extern "C" {
#endif
void * mbed_sbrk(ptrdiff_t size);
void * mbed_krbs(const ptrdiff_t size);
void * mbed_krbs_ex(const ptrdiff_t size, ptrdiff_t *actual);
#ifdef __cplusplus
}
#endif


#endif // __MBED_ALLOC_H
