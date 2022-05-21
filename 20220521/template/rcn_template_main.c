// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt)	KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "rcn_template.h"

static int __template_parse_dt_str(struct builder *bd, struct device_node *np)
{
	struct template_drvdata *drvdata =
			container_of(bd, struct template_drvdata, bd);
	struct device *dev = bd->dev;
	const char *str;
	int err;

	err = of_property_read_string(np, "rcn,str", &str);
	if (err) {
		dev_err(dev, "err = %d\n", err);
		return -EINVAL;
	}

	drvdata->str = str;

	return 0;
}

static int __template_parse_dt_val(struct builder *bd, struct device_node *np)
{
	struct template_drvdata *drvdata =
			container_of(bd, struct template_drvdata, bd);
	struct device *dev = bd->dev;
	u32 val;
	int err;

	err = of_property_read_u32(np, "rcn,val", &val);
	if (err) {
		dev_err(dev, "err = %d\n", err);
		return -EINVAL;
	}

	drvdata->val = val;

	return 0;
}

static const struct dt_builder __template_dt_builder[] = {
        DT_BUILDER(__template_parse_dt_str),
        DT_BUILDER(__template_parse_dt_val),
};

static int __template_parse_dt(struct builder *bd)
{
	return sec_director_parse_dt(bd, __template_dt_builder,
			ARRAY_SIZE(__template_dt_builder));
}

static int __template_probe_epilog(struct builder *bd)
{
	struct template_drvdata *drvdata =
			container_of(bd, struct template_drvdata, bd);
	struct device *dev = bd->dev;

	dev_set_drvdata(dev, drvdata);

	return 0;
}

static void __template_remove_prolog(struct builder *bd)
{
	struct device *dev = bd->dev;

	dev_info(dev, "removing...\n");
}

static int __template_probe(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct device *dev = &pdev->dev;
	struct template_drvdata *drvdata;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
	return -ENOMEM;

	drvdata->bd.dev = dev;

	return sec_director_probe_dev(&drvdata->bd, builder, n);
}

static int __template_remove(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct template_drvdata *drvdata = platform_get_drvdata(pdev);

	sec_director_destruct_dev(&drvdata->bd, builder, n, n);

	return 0;
}

static const struct dev_builder __template_dev_builder[] = {
	DEVICE_BUILDER(__template_parse_dt, NULL),
#if IS_ENABLED(CONFIG_DEBUG_FS)
	DEVICE_BUILDER(rcn_template_debugfs_create, rcn_template_debugfs_remove),
#endif
	DEVICE_BUILDER(__template_probe_epilog, __template_remove_prolog),
};

static int rcn_template_probe(struct platform_device *pdev)
{
	return __template_probe(pdev, __template_dev_builder,
			ARRAY_SIZE(__template_dev_builder));
}

static int rcn_template_remove(struct platform_device *pdev)
{
	return __template_remove(pdev, __template_dev_builder,
			ARRAY_SIZE(__template_dev_builder));
}

static const struct of_device_id rcn_template_match_table[] = {
	{ .compatible = "raccoon,template", },
	{},
};
MODULE_DEVICE_TABLE(of, rcn_template_match_table);

static struct platform_driver rcn_template_driver = {
	.driver = {
		.name = "raccoon,template",
		.of_match_table = of_match_ptr(rcn_template_match_table),
	},
	.probe = rcn_template_probe,
	.remove = rcn_template_remove,
};

int rcn_template_init(void)
{
	return platform_driver_register(&rcn_template_driver);
}
module_init(rcn_template_init);

void rcn_template_exit(void)
{
	platform_driver_unregister(&rcn_template_driver);
}
module_exit(rcn_template_exit);

MODULE_AUTHOR("YOUNGJIN JOO <neoelec@gmail.com>");
MODULE_LICENSE("GPL v2");
