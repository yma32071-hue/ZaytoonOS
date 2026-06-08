#include <stdint.h>
#include "kernel/mm/pmm.h"
#include "kernel/kernel/printk.h"
#include "kernel/mm/mm.h"

#define PMM_MAX_FRAMES 1024

static void *free_frames[PMM_MAX_FRAMES];
static unsigned int free_frame_count;

void pmm_init(void)
{
    uintptr_t frame = 0x01000000u;
    uintptr_t limit = 0x01800000u;

    free_frame_count = 0;
    while (frame + 0x1000u <= limit && free_frame_count < PMM_MAX_FRAMES) {
        free_frames[free_frame_count++] = (void *)frame;
        frame += 0x1000u;
    }

    printk("[pmm] %u free frames initialized", free_frame_count);
}

void *pmm_alloc_frame(void)
{
    if (free_frame_count == 0) {
        return NULL;
    }
    return free_frames[--free_frame_count];
}

void pmm_free_frame(void *frame)
{
    if (free_frame_count < PMM_MAX_FRAMES) {
        free_frames[free_frame_count++] = frame;
    }
}
