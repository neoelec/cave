#ifndef __INTERNAL__RCN_TEMPLATE_H__
#define __INTERNAL__RCN_TEMPLATE_H__

#include <linux/debugfs.h>

#include <linux/samsung/builder_pattern.h>

struct template_drvdata {
	struct builder bd;
	const char *str;
	u32 val;
#if IS_ENABLED(CONFIG_DEBUG_FS)
	struct dentry *dbgfs;
#endif
};

/* rcn_template_debugfs.c */
extern int rcn_template_debugfs_create(struct builder *bd);
extern void rcn_template_debugfs_remove(struct builder *bd);

#endif /* __INTERNAL__RCN_TEMPLATE_H__ */
