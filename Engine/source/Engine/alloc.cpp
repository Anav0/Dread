#include "base.h"
#include "alloc.h"

#include <windows.h>
#include <memoryapi.h>
#include <sysinfoapi.h>
#include <stdio.h>
#include <string.h>
#include <cassert>

i32 os_commit(void *ptr, u64 size)
{
	i32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
	return (result);
}

void *os_reserve(u64 size)
{
	void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
	return (result);
}

void os_release(void *ptr)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
}

u64 os_page_size()
{
	SYSTEM_INFO sysinfo = {0};
	GetSystemInfo(&sysinfo);
	return (sysinfo.dwPageSize);
}

Arena *alloc(u64 reserve_size, u64 commit_size)
{
	u64 page_size = os_page_size();
	reserve_size  = AlignPow2(reserve_size, page_size);
	commit_size   = AlignPow2(commit_size, page_size);
	void *memory  = os_reserve(reserve_size);

	if (!os_commit(memory, commit_size))
	{
		printf("Failed to reserve memory");
		assert(false);
	}

	assert(memory);

	Arena *arena    = (Arena*)memory;
	arena->ptr      = arena;
	arena->commited = commit_size;
	arena->reserved = reserve_size;
	arena->align    = 8;

	return arena;
}

void dealloc(Arena *arena)
{
	os_release(arena);
}

void *arena_push(Arena *arena, u64 size)
{
	Arena *current = arena->ptr;
	u64 pos_mem    = AlignPow2(current->pos, arena->align);
	u64 pos_new    = pos_mem + size;

	void *memory = 0;
	if (current->commited < pos_new)
	{
		u64 cmt_new_aligned, cmt_new_clamped, cmt_new_size;
		i32 is_cmt_ok;

		cmt_new_aligned = AlignPow2(pos_new, ARENA_COMMIT_SIZE);
		cmt_new_clamped = ClampTop(cmt_new_aligned, current->reserved);
		cmt_new_size    = cmt_new_clamped - current->commited;
		is_cmt_ok       = os_commit((u8 *)current + current->commited, cmt_new_size);

		assert(is_cmt_ok);

		if (is_cmt_ok)
		{
			current->commited = cmt_new_clamped;
		}
	}
	else
	{
		memory = (u8 *)current + pos_mem;
		current->pos = pos_new;
		// AsanUnpoisonMemoryRegion(memory, size);
	}

	return memory;
}

