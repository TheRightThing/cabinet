struct CabinetDirectoryEntry
{
	int is_dir;
	char * name;
};

typedef void (*dir_selected_fn_t)(struct CabinetDirectoryEntry const *);

struct CabinetDirView {
	struct CabinetDirectoryEntry const * entries;
	unsigned selected;
	unsigned count;

	unsigned start;

	dir_selected_fn_t on_selected;
};

void cabinet_dir_view_activate(struct CabinetDirView * ctx);
