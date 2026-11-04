#ifndef ARM_ACLE_INTRINS_H
#define ARM_ACLE_INTRINS_H

#include <stdint.h>
#if defined(_MSC_VER) && !defined(__clang__)
#  include <intrin.h>
#elif defined(HAVE_ARM_ACLE_H)
#  include <arm_acle.h>
#endif

#ifdef ARM_CRC32
#if defined(ARCH_ARM) && defined(ARCH_64BIT)
#  define Z_TARGET_CRC Z_TARGET("+crc")
#elif defined(ARCH_ARM) && defined(ARCH_32BIT) && defined(__clang__) && !defined(__ARM_FEATURE_CRC32)
/* 32-bit ARM with an ARMv7-A baseline (e.g. Android's armeabi-v7a): clang does not expose the CRC32
 * intrinsics from arm_acle.h and its integrated assembler rejects the instructions unless the CRC feature
 * is enabled. Enable it per function rather than per module (this keeps LTO happy) and map the intrinsics
 * to the compiler builtins. The caller is expected to have checked HWCAP2_CRC32 at runtime. */
#  define Z_TARGET_CRC Z_TARGET("armv8-a,crc")
#  define ARM_CRC32_CLANG_BUILTINS
#else
#  define Z_TARGET_CRC
#endif
#ifdef ARM_PMULL_EOR3
#  define Z_TARGET_PMULL_EOR3 Z_TARGET("+crc+crypto+sha3")
#else
#  define Z_TARGET_PMULL_EOR3
#endif

#if defined(ARM_CRC32_CLANG_BUILTINS)
#  define __crc32b __builtin_arm_crc32b
#  define __crc32h __builtin_arm_crc32h
#  define __crc32w __builtin_arm_crc32w
#  define __crc32d __builtin_arm_crc32d
#elif !defined(ARM_CRC32_INTRIN) && !defined(_MSC_VER) && \
      !(defined(HAVE_ARM_ACLE_H) && defined(__ARM_FEATURE_CRC32))
#if defined(ARCH_ARM) && defined(ARCH_64BIT)
static inline uint32_t __crc32b(uint32_t __a, uint8_t __b) {
    uint32_t __c;
    __asm__("crc32b %w0, %w1, %w2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32h(uint32_t __a, uint16_t __b) {
    uint32_t __c;
    __asm__("crc32h %w0, %w1, %w2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32w(uint32_t __a, uint32_t __b) {
    uint32_t __c;
    __asm__("crc32w %w0, %w1, %w2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32d(uint32_t __a, uint64_t __b) {
    uint32_t __c;
    __asm__("crc32x %w0, %w1, %x2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}
#else
static inline uint32_t __crc32b(uint32_t __a, uint8_t __b) {
    uint32_t __c;
    __asm__("crc32b %0, %1, %2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32h(uint32_t __a, uint16_t __b) {
    uint32_t __c;
    __asm__("crc32h %0, %1, %2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32w(uint32_t __a, uint32_t __b) {
    uint32_t __c;
    __asm__("crc32w %0, %1, %2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}

static inline uint32_t __crc32d(uint32_t __a, uint64_t __b) {
    return __crc32w (__crc32w (__a, __b & 0xffffffffULL), __b >> 32);
}
#endif
#endif
#endif

#ifdef ARM_SIMD
#ifdef _MSC_VER
typedef uint32_t uint16x2_t;

#define __uqsub16 _arm_uqsub16
#elif !defined(ARM_SIMD_INTRIN)
typedef uint32_t uint16x2_t;

static inline uint16x2_t __uqsub16(uint16x2_t __a, uint16x2_t __b) {
    uint16x2_t __c;
    __asm__("uqsub16 %0, %1, %2" : "=r" (__c) : "r"(__a), "r"(__b));
    return __c;
}
#endif
#endif

#endif // include guard ARM_ACLE_INTRINS_H
