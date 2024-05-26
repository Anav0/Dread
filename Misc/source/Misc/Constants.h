#ifndef MISC_CONSTANTS_H
#define MISC_CONSTANTS_H

#pragma once

#include <cstdint>

typedef int ID;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef float  f32;
typedef double f64;


#define KB(n)  (((u64)(n)) << 10)
#define MB(n)  (((u64)(n)) << 20)
#define GB(n)  (((u64)(n)) << 30)
#define TB(n)  (((u64)(n)) << 40)

#define Min(A,B) ( ((A)<(B))?(A):(B) )
#define Max(A,B) ( ((A)>(B))?(A):(B) )
#define ClampTop(A,X) Min(A,X)
#define AlignPow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))

#endif
