#include "fs.h"
#include "macros.h"
#include <stddef.h>
#include <stdlib.h>

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_COURCE 200809L
char *mkdtemp(char *template);
#endif

typedef struct Setup {
  const char* root; // Root directory of the filesystem.
  const fs_entry* entries; // Expected entries in the filesystem.
  const unsigned int num_entries; // Number of entries in the filesystem.
} Setup;

typedef int (*fs_test) (Setup*);

int assert_equal_entries(const fs_entry* a, const fs_entry* b);

int list_directory(Setup* setup) {
  fs_entries entries = entries_in(setup->root);
  if (entries.num_entries != setup->num_entries) {
    return 0;
  }
  return assert_equal_entries(entries.entries, setup->entries);
}

int assert_equal_entries(const fs_entry* a, const fs_entry* b) {
  for (unsigned int i = 0;;++i) {
    // Both lists end at the same time, with the same value.
    if (a[i].type == FS_ENTRY_TYPE_SENTINEL) {
      return b[i].type == FS_ENTRY_TYPE_SENTINEL;
    }
    // Both entries have the same type.
    if (a[i].type != b[i].type) {
      return 0;
    }
    // Both entries have the same path.
    if (a[i].path != b[i].path) {
      return 0;
    }
  }
  return 1;
}

int test_with_fs(const fs_test tests[]) {
  (void) tests;
  const char* root = mkdtemp("cabinet_testXXXXXX");
  const unsigned int num_of_files = 100;
  for (unsigned int = 0; i < num_of_files; ++i) {
    const char* filename = mkstemp();
    const char* path = concat(root, filename, i);
  }
  return 0;
}

int filesystem_tests(int *tests)
{
  // Number of tests in this suite.
  *tests = 1;
  return test_with_fs((const fs_test[]){list_directory});
  // // Number of successful tests.
  // return 1;
}
