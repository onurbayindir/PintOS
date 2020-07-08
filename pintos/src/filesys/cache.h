#ifndef CACHE_H
#define CACHE_H

#include "devices/block.h"
#include "threads/synch.h"

void cache_init (void);
void cache_read (struct block *block, block_sector_t sector, void *buffer);
void cache_write (struct block *block, block_sector_t sector, const void *buffer);
void cache_read_at(struct block *block, block_sector_t sector, void *buffer, off_t offset, size_t len);
void cache_write_at(struct block *block, block_sector_t sector, void *buffer, off_t offset, size_t len);
void cache_flush (void);

struct list lru_cache;
struct lock cache_lock; 

struct cache_entry
{
	bool dirty;
    bool valid;
    struct block *cache_block;
    block_sector_t sector_num;

    char buffer[512];

	struct list_elem elem;
    struct lock entry_lock;
};

#endif
