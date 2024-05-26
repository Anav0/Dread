#ifndef ALLOC_H
#define ALLOC_H

#pragma once

#include <string>

#define MemoryCopy(dst, src, size) memmove((dst), (src), (size))
#define MemorySet(dst, byte, size) memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)  memcmp((a), (b), (size))
#define MemoryStrlen(ptr)          strlen(ptr)

#define MemoryCopyStruct(d,s)  MemoryCopy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   MemoryCopy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),sizeof(*(d))*(c))

#define MemoryZero(s,z)       memset((s),0,(z))
#define MemoryZeroStruct(s)   MemoryZero((s),sizeof(*(s)))
#define MemoryZeroArray(a)    MemoryZero((a),sizeof(a))
#define MemoryZeroTyped(m,c)  MemoryZero((m),sizeof(*(m))*(c))

#define MemoryMatch(a,b,z)     (MemoryCompare((a),(b),(z)) == 0)
#define MemoryMatchStruct(a,b)  MemoryMatch((a),(b),sizeof(*(a)))
#define MemoryMatchArray(a,b)   MemoryMatch((a),(b),sizeof(a))

#define place(a,T,c) (T*)arena_push((a), sizeof(T)*(c))

#define ARENA_HEADER_SIZE 128

#ifndef ARENA_RESERVE_SIZE
# define ARENA_RESERVE_SIZE MB(64)
#endif
#ifndef ARENA_COMMIT_SIZE
# define ARENA_COMMIT_SIZE KB(64)
#endif

struct Arena
{
	Arena *ptr;
	u64 pos;
	u64 commited;
	u64 reserved;
	u64 align;
};

Arena *alloc(u64 reserve_size, u64 commit_size);
void   dealloc(Arena *arena);

i32   os_commit(void *ptr, u64 size);
void *os_reserve(u64 size);
void  os_release(void *ptr);
u64   os_page_size();

void *arena_push(Arena *arena, u64 size);

#endif
