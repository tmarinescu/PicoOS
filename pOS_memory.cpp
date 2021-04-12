#include "pOS_memory.hpp"

pOS_memory_block::pOS_memory_block()
{
	id = 0;
	location = 0;
	size = 0;
}

pOS_memory_block::~pOS_memory_block()
{
	
}

uint8_t pOS_memory::_pool[MEMORY_POOL];

pOS_memory_block pOS_memory::_pool_meta[MEMORY_META_POOL];

bool pOS_memory::initialize()
{
	for (uint32_t i = 0; i < MEMORY_POOL; i++)
	{
		_pool[i] = i;
	}
	
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		_pool_meta[i].id = 0;
		_pool_meta[i].location = 0;
		_pool_meta[i].size = 0;
	}
	
	return true;
}

bool pOS_memory::is_memory_chunk_taken(uint32_t offset, uint32_t ignore_id)
{
	void* ptr = &_pool[offset];
	void* start = 0;
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].id != ignore_id && _pool_meta[i].id != 0)
		{
			for (uint32_t j = 0; j < _pool_meta[i].size; j++)
			{
				if (&((uint8_t*)_pool_meta[i].location)[j] == (uint8_t*)ptr)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}
	
void* pOS_memory::allocate(uint32_t id, uint32_t size)
{
	pOS_memory_block* mem = 0;
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].id == 0)
		{
			mem = &_pool_meta[i];
		}
	}
	
	if (mem == 0)
	{
		return 0;
	}
	else
	{
		uint32_t track_size = 0;
		uint32_t track_offset = 0;
		uint32_t start_offset = 0;
		uint8_t found_chunk = 0;
		for (track_offset = 0; track_offset < MEMORY_POOL; track_offset++)
		{
			if (is_memory_chunk_taken(track_offset, id))
			{
				track_size = 0;
				start_offset = track_offset + 1;
			}
			else
			{
				track_size++;
			}
			
			if (track_size == size)
			{
				found_chunk = 1;
				break;
			}
		}
		
		if (found_chunk == 1)
		{
			mem->location = &_pool[start_offset];
			mem->id = id;
			mem->size = size;
			return mem->location;
		}
		else
		{
			mem->id = 0;
			mem->size = 0;
			mem->location = 0;
			return 0;
		}
	}
}
	
void pOS_memory::deallocate(uint32_t id)
{
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].id == id)
		{
			_pool_meta[i].id = 0;
			_pool_meta[i].location = 0;
			_pool_meta[i].size = 0;
			break;
		}
	}
}
	
void pOS_memory::deallocate(void* ptr)
{
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].location == ptr)
		{
			_pool_meta[i].id = 0;
			_pool_meta[i].location = 0;
			_pool_meta[i].size = 0;
			break;
		}
	}
}
	
void* pOS_memory::get_pointer(uint32_t id)
{
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].id == id)
		{
			return _pool_meta[i].location;
		}
	}
	
	return 0;
}
	
bool pOS_memory::zero(void* ptr)
{
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].location == ptr)
		{
			for (uint32_t j = 0; j < _pool_meta[i].size; j++)
			{
				((uint8_t*)_pool_meta[i].location)[j] = 0;
			}
			return true;
		}
	}
	
	return false;
}
	
bool pOS_memory::zero(uint32_t id)
{
	for (uint32_t i = 0; i < MEMORY_META_POOL; i++)
	{
		if (_pool_meta[i].id == id)
		{
			for (uint32_t j = 0; j < _pool_meta[i].size; j++)
			{
				((uint8_t*)_pool_meta[i].location)[j] = 0;
			}
			return true;
		}
	}
	
	return false;
}
	