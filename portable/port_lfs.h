#include "lfs.h"

int pBlock_Device_Read(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, void *buffer, lfs_size_t size);
int pBlock_Device_Prog(const struct lfs_config *c, lfs_block_t block,
                       lfs_off_t off, const void *buffer, lfs_size_t size);
int pBlock_Device_Erase(const struct lfs_config *c, lfs_block_t block);
int pBlock_Device_Sync(const struct lfs_config *c);