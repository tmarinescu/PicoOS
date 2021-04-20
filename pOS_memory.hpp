#ifndef _POS_MEMORY_H_
#define _POS_MEMORY_H_

#include "pOS_main.hpp"

class pOS_memory_block
{
public:
	uint32_t id;			/* 0 = unused */
	uint32_t size;
	volatile void* volatile location;
	pOS_memory_block();
	~pOS_memory_block();
};

class pOS_memory
{
private:
	static volatile uint8_t _pool[MEMORY_POOL];
	static volatile pOS_memory_block _pool_meta[MEMORY_META_POOL];
public:
	static bool initialize();
	static void* allocate(uint32_t id, uint32_t size);
	static void deallocate(uint32_t id);
	static void deallocate(void* ptr);
	static void* get_pointer(uint32_t id);
	static bool zero(void* ptr);
	static bool zero(uint32_t id);
	static void* wait_for_memory_id(uint32_t id);
	static bool is_memory_chunk_taken(uint32_t offset, uint32_t ignore_id);
};

#endif