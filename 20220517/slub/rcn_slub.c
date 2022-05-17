// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt)	KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <linux/samsung/builder_pattern.h>

struct slub_drvdata {
	struct builder bd;
};

static int __slub_alloc_test(struct builder *bd)
{
	struct device *dev = bd->dev;
	size_t size[] = { 128, 256, 512, 1024, 2048, 4096, 8192 };
	size_t i;

	for (i = 0; i < ARRAY_SIZE(size); i++) {
		void *buf = kmalloc(size[i], GFP_KERNEL);

		dev_info(dev, "kmalloc-%zu : %px\n", size[i], buf);

		kfree(buf);
	}

	return 0;
}

static int __slub_probe_epilog(struct builder *bd)
{
	struct slub_drvdata *drvdata =
			container_of(bd, struct slub_drvdata, bd);
	struct device *dev = bd->dev;

	dev_set_drvdata(dev, drvdata);

	return 0;
}

static int __slub_probe(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct device *dev = &pdev->dev;
	struct slub_drvdata *drvdata;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
	return -ENOMEM;

	drvdata->bd.dev = dev;

	return sec_director_probe_dev(&drvdata->bd, builder, n);
}

static int __slub_remove(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct slub_drvdata *drvdata = platform_get_drvdata(pdev);

	sec_director_destruct_dev(&drvdata->bd, builder, n, n);

	return 0;
}

static const struct dev_builder __slub_dev_builder[] = {
	DEVICE_BUILDER(__slub_alloc_test, NULL),
	DEVICE_BUILDER(__slub_probe_epilog, NULL),
};

static int rcn_slub_probe(struct platform_device *pdev)
{
	return __slub_probe(pdev, __slub_dev_builder,
			ARRAY_SIZE(__slub_dev_builder));
}

static int rcn_slub_remove(struct platform_device *pdev)
{
	return __slub_remove(pdev, __slub_dev_builder,
			ARRAY_SIZE(__slub_dev_builder));
}

static const struct of_device_id rcn_slub_match_table[] = {
	{ .compatible = "raccoon,slub", },
	{},
};
MODULE_DEVICE_TABLE(of, rcn_slub_match_table);

static struct platform_driver rcn_slub_driver = {
	.driver = {
		.name = "raccoon,slub",
		.of_match_table = of_match_ptr(rcn_slub_match_table),
	},
	.probe = rcn_slub_probe,
	.remove = rcn_slub_remove,
};

int rcn_slub_init(void)
{
	return platform_driver_register(&rcn_slub_driver);
}
module_init(rcn_slub_init);

void rcn_slub_exit(void)
{
	platform_driver_unregister(&rcn_slub_driver);
}
module_exit(rcn_slub_exit);

MODULE_AUTHOR("YOUNGJIN JOO <neoelec@gmail.com>");
MODULE_LICENSE("GPL v2");
