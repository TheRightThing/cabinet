#ifndef FS_H
#define FS_H

typedef enum fs_entry_type {
  FS_ENTRY_TYPE_FILE,
  FS_ENTRY_TYPE_DIR,
  FS_ENTRY_TYPE_SENTINEL,
} fs_entry_type;

typedef struct fs_entry {
  char *path;
  fs_entry_type type;
} fs_entry;

typedef struct fs_entries {
  const fs_entry *entries;
  const unsigned int num_entries;
} fs_entries;

const char *get_current_dir();
fs_entries entries();
fs_entries entries_in(const char *path);

#endif
