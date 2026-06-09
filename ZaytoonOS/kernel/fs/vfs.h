#ifndef ZAYTOONOS_KERNEL_FS_VFS_H
#define ZAYTOONOS_KERNEL_FS_VFS_H

#include "kernel/types.h"

void vfs_init(void);
void vfs_list_dir(const char *path, void (*writer)(const char *line));
bool vfs_exists(const char *path);
const char *vfs_cwd(void);
bool vfs_chdir(const char *path);

#endif /* ZAYTOONOS_KERNEL_FS_VFS_H */
