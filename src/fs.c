#include <stdlib.h>
#include "fs.h"

const char* get_current_dir() {
	return "";
}

// entries returns the entries of the current directory. For guarantees of the
// return value, see entries_in.
fs_entries entries() {
	const char* current_dir = get_current_dir();
	return entries_in(current_dir);
}

// entries_in returns the entries of the directory at the given path. The
// result is guaranteed to contain at least the FS_ENTRY_TYPE_SENTINEL entry,
// so is never NULL.
fs_entries entries_in(const char* path) {
	(void) path;
	const int n = 420;
	fs_entry* entries = malloc(n * sizeof(struct fs_entry));
	// TODO: Populate entries.
	entries[0] = (fs_entry){
		.path = "some/path",
		.type = FS_ENTRY_TYPE_FILE,
	};
	return (fs_entries){
		.entries = entries,
		.num_entries = n,
	};
}
