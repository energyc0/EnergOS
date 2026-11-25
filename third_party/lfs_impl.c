#include "LittleFS/lfs.h"
#include "portable/kernel_config.h"

#ifdef W25Q64
#include "portable/stm32f411re/w25q64.h"
#endif

int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
             void *buffer, lfs_size_t size);
int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
             const void *buffer, lfs_size_t size);
int lfs_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_sync(const struct lfs_config *c);

static uint8_t lfs_read_buf[STATIC_READ_BUF_SIZE];
static uint8_t lfs_prog_buf[STATIC_PROG_BUF_SIZE];

static uint32_t lfs_lookahead_buf[STATIC_LOOKAHEAD_BUF_SIZE];

const struct lfs_config cfg = {
    .context = 0,
    .read = lfs_read,
    .prog = lfs_prog,
    .erase = lfs_erase,
    .sync = lfs_sync,

    // Memory parameters
    .read_size = PAGESIZE_BYTES,
    .prog_size = PAGESIZE_BYTES,
    .block_size = SECTOR_SIZE_BYTES,
    .block_count = SECTOR_COUNT,
    .cache_size = PAGESIZE_BYTES,
    .lookahead_size = sizeof(lfs_lookahead_buf),
    .block_cycles = WEAR_LEVELING,

    // Static buffers (no malloc)
    .read_buffer = lfs_read_buf,
    .prog_buffer = lfs_prog_buf,
    .lookahead_buffer = lfs_lookahead_buf,
};