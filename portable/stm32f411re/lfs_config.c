#include "port_lfs.h"

const struct lfs_config cfg = {
    .read = pBlock_Device_Read,
    .prog = pBlock_Device_Prog,
    .erase = pBlock_Device_Erase,
    .sync = pBlock_Device_Sync,
    .read_size = 256,     // Minimal  size of reading in bytes.
    .prog_size = 256,     // Minimal size of writing in bytes.
    .block_size = 4096,   // Minimal size of erasing block.
    .block_count = 2048,  // Overall amount of blocks in memory

    .cache_size = 256,
    .lookahead_size = 32,  // Size of buffer for searching free blocks
    .block_cycles = 500,   // Number of wear-leveling cycles
};

int pBlock_Device_Read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size) {}
int pBlock_Device_Prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size);
int pBlock_Device_Erase(const struct lfs_config *c, lfs_block_t block);
int pBlock_Device_Sync(const struct lfs_config *c);