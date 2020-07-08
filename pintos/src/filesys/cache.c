#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "devices/block.h"
#include "filesys/cache.h"
#include "threads/synch.h"
#include <list.h>
#include <stdlib.h>
#include "threads/malloc.h"
#include "threads/thread.h"

const int BUFFER_CACHE_CAPACITY = 64;

void cache_init (void) 
{
    /* Initialize the global lock and cache list */
    list_init (&lru_cache);
    lock_init (&cache_lock);
    for (int i = 0; i < BUFFER_CACHE_CAPACITY; ++i) {
        struct cache_entry *entry = (struct cache_entry*) malloc (sizeof(struct cache_entry));
        lock_init(&entry->entry_lock);
        entry->valid = entry->dirty = 0;
        list_push_front (&lru_cache, &(entry->elem));
    }
}

/* Find the entry corresponding to sector if it's in the cache; otherwise return NULL 
   Assumes that the global lock is held when it is called.
*/
static struct cache_entry *find_cache_entry (block_sector_t sector) 
{
    struct list_elem *e;    
    for (e = list_begin (&lru_cache); e != list_end (&lru_cache); e = list_next (e)) 
    {
        struct cache_entry *entry = list_entry (e, struct cache_entry, elem);
        if (entry->valid && entry->sector_num == sector)  // cache hit
        {
            thread_current ()->num_hits++;
            /* Found the corresponding entry */
            return entry;
        } 
    }
    return NULL;
}

/* Move the corresponding entry to the front of the LRU linked list (indicating recent use)
   Assumes the global lock is held when it is called. 
 */
static void move_to_back (struct cache_entry *entry) {
    list_remove (&entry->elem);
    list_push_back (&lru_cache, &entry->elem);
}

/* Locates an invalid block if possible, else evicts/flushes and returns the lru block 
   Assumes the global lock is held when it is called.
   When it returns, the global lock has been released and the entry lock has been acquired.
*/
static struct cache_entry *find_lru_entry (void) {
    struct list_elem *e;    
    struct cache_entry *lru_entry;
    for (e = list_begin (&lru_cache); e != list_end (&lru_cache); e = list_next (e)) 
    {
        lru_entry = list_entry (e, struct cache_entry, elem);
        if (!lru_entry->valid)  // found an invalid entry
        {
            lock_acquire (&lru_entry->entry_lock);
            if (!lru_entry->valid)
                break;
            else {
                lock_release (&lru_entry->entry_lock);
                lru_entry = NULL;
            }
        } else {
            lru_entry = NULL;
        }
    } 

    if (lru_entry == NULL) {
        lru_entry = list_entry (list_begin (&lru_cache), struct cache_entry, elem);
        lock_acquire (&lru_entry->entry_lock);
        if (lru_entry->dirty) {
            block_write (lru_entry->cache_block, lru_entry->sector_num, lru_entry->buffer);
            lru_entry->dirty = false;
        }
    } 
    move_to_back (lru_entry);
    lock_release (&cache_lock); 
    return lru_entry;
}

void cache_read_at (struct block *block, block_sector_t sector, void *buffer, off_t offset, size_t len) 
{
    // acquire global cache lock
    // walk through cache to find entry
    // release global cache lock
    lock_acquire (&cache_lock);
    thread_current ()->num_cache_accesses++;

    struct cache_entry *entry = find_cache_entry (sector);
    
    /* On a cache hit
       The while loop accounts for the fact that the block might change between
       releasing the cache lock and acquiring the entry lock.
     */
    while (entry != NULL)
    {
        lock_release (&cache_lock);
        lock_acquire (&entry->entry_lock);
        if (entry->valid && entry->sector_num == sector && entry->cache_block == block) 
        {
            /* Copy contents of cache block into buffer */
            memcpy(buffer, (void *) entry->buffer + offset, len);
            lock_release (&entry->entry_lock);

            /* Move the entry to the front of the lru list to implement LRU */
            lock_acquire (&cache_lock);
            move_to_back (entry);
            lock_release (&cache_lock);
            return;
        } else {
            lock_release (&entry->entry_lock);
            lock_acquire (&cache_lock);
            entry = find_cache_entry (sector);
        }   
    }
   
    // cache miss
    entry = find_lru_entry ();
    /* After call to find_lru entry, we have released the cache_lock and acquired the entry_lock */
    block_read (block, sector, entry->buffer);
    entry->cache_block = block;
    entry->sector_num = sector;
    entry->dirty = false;
    entry->valid = true;
    memcpy(buffer, (void *) entry->buffer + offset, len);
    lock_release (&entry->entry_lock);

    return;
}

void cache_write_at (struct block *block, block_sector_t sector, void *buffer, off_t offset, size_t len) 
{
    lock_acquire (&cache_lock);
    thread_current ()->num_cache_accesses++;

    struct cache_entry *entry = find_cache_entry(sector);

    while (entry != NULL)
    {
        lock_release (&cache_lock);
        lock_acquire (&entry->entry_lock);
        if (entry->valid && entry->sector_num == sector && entry->cache_block == block) 
        {
            memcpy((void *) entry->buffer + offset, buffer, len);
            entry->dirty = true;
            lock_release (&entry->entry_lock);

            lock_acquire (&cache_lock);
            move_to_back (entry);
            lock_release (&cache_lock);
            return;
        } else 
        {
            lock_release (&entry->entry_lock);
            lock_acquire (&cache_lock);
            entry = find_cache_entry (sector);
        }   
    }
    if (entry == NULL) 
    {
        /* When find_lru_entry exits, the current thread will have released the global lock
           and acquired the per-entry lock */
        entry = find_lru_entry ();
    }
    //entry lock will be acquired here
    /* Copy contents of cache block into buffer */
    memcpy((void *) entry->buffer + offset, buffer, len);
    entry->cache_block = block;
    entry->sector_num = sector;
    entry->valid = true;
    entry->dirty = true;
    lock_release (&entry->entry_lock);
}

void cache_read (struct block *block, block_sector_t sector, void *buffer) 
{
    cache_read_at (block, sector, buffer, 0, BLOCK_SECTOR_SIZE);
}

void cache_write (struct block *block, block_sector_t sector, const void *buffer) 
{
    cache_write_at (block, sector, buffer, 0, BLOCK_SECTOR_SIZE);
}

void cache_flush (void) 
{
    lock_acquire (&cache_lock);
    thread_current ()->num_cache_accesses = 0;
    thread_current ()->num_hits = 0;
    
    struct list_elem *e;

    //write dirty entries to disk
    for (e = list_begin (&lru_cache); e != list_end (&lru_cache); e = list_next (e))  
    {
        struct cache_entry *cur = list_entry (e, struct cache_entry, elem);
        lock_acquire (&cur->entry_lock);
        if (cur->valid && cur->dirty) 
        {
            block_write (cur->cache_block, cur->sector_num, cur->buffer);
            cur->dirty = false;
            cur->valid = false;
        }
        lock_release (&cur->entry_lock);
    }

    lock_release (&cache_lock);
}
