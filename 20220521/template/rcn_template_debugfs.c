// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt)	KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>

#include "rcn_template.h"

static int rcn_template_val_get(void *data, u64 *val)
{
	struct template_drvdata *drvdata = data;

	*val = drvdata->val;

	return 0;
}
DEFINE_DEBUGFS_ATTRIBUTE(rcn_template_val_fops,
		rcn_template_val_get, NULL, "%llu\n");

int rcn_template_debugfs_create(struct builder *bd)
{
	struct template_drvdata *drvdata =
			container_of(bd, struct template_drvdata, bd);
	struct device *dev = bd->dev;
	char *name;

	name = devm_kasprintf(dev, GFP_KERNEL, "rcn_template-%s", drvdata->str);
	if (!name)
		return -ENOMEM;

	drvdata->dbgfs = debugfs_create_file(name, 0440,
			NULL, drvdata, &rcn_template_val_fops);

	return 0;
}

void rcn_template_debugfs_remove(struct builder *bd)
{
	struct template_drvdata *drvdata =
			container_of(bd, struct template_drvdata, bd);

	debugfs_remove(drvdata->dbgfs);
}
