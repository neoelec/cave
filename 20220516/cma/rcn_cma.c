// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt)	KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>

#include <linux/samsung/builder_pattern.h>

struct cma_drvdata {
	struct builder bd;
	void *virt;
	dma_addr_t phys;
};

static int __cma_parse_dt_memory_region(struct builder *bd,
		struct device_node *np)
{
	struct device *dev = bd->dev;
	struct device_node *mem_np;
	int err;

	mem_np = of_parse_phandle(np, "memory-region", 0);
	if (!mem_np)
		return -ENOENT;

	err = of_reserved_mem_device_init_by_idx(dev, np, 0);
	if (err) {
		dev_err(dev, "failed to initialize reserved mem (%d)\n", err);
		return err;
	}

	return 0;
}

static const struct dt_builder __cma_dt_builder[] = {
	DT_BUILDER(__cma_parse_dt_memory_region),
};

static int __cma_parse_dt(struct builder *bd)
{
	return sec_director_parse_dt(bd, __cma_dt_builder,
			ARRAY_SIZE(__cma_dt_builder));
}

static int __cma_alloc_1_page(struct builder *bd)
{
	struct cma_drvdata *drvdata =
			container_of(bd, struct cma_drvdata, bd);
	struct device *dev = bd->dev;
	void *virt;
	dma_addr_t phys;

	virt = dmam_alloc_coherent(dev, PAGE_SIZE, &phys, GFP_KERNEL);
	dev_info(dev, "%p (%zx)\n", virt, (size_t)phys);

	drvdata->virt = virt;
	drvdata->phys = phys;

	return 0;
}

static int __cma_probe_epilog(struct builder *bd)
{
	struct cma_drvdata *drvdata =
			container_of(bd, struct cma_drvdata, bd);
	struct device *dev = bd->dev;

	dev_set_drvdata(dev, drvdata);

	return 0;
}

static int __cma_probe(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct device *dev = &pdev->dev;
	struct cma_drvdata *drvdata;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
	return -ENOMEM;

	drvdata->bd.dev = dev;

	return sec_director_probe_dev(&drvdata->bd, builder, n);
}

static int __cma_remove(struct platform_device *pdev,
		const struct dev_builder *builder, ssize_t n)
{
	struct cma_drvdata *drvdata = platform_get_drvdata(pdev);

	sec_director_destruct_dev(&drvdata->bd, builder, n, n);

	return 0;
}

static const struct dev_builder __cma_dev_builder[] = {
	DEVICE_BUILDER(__cma_parse_dt, NULL),
	DEVICE_BUILDER(__cma_probe_epilog, NULL),
	DEVICE_BUILDER(__cma_alloc_1_page, NULL),
};

static int rcn_cma_probe(struct platform_device *pdev)
{
	return __cma_probe(pdev, __cma_dev_builder,
			ARRAY_SIZE(__cma_dev_builder));
}

static int rcn_cma_remove(struct platform_device *pdev)
{
	return __cma_remove(pdev, __cma_dev_builder,
			ARRAY_SIZE(__cma_dev_builder));
}

static const struct of_device_id rcn_cma_match_table[] = {
	{ .compatible = "raccoon,cma", },
	{},
};
MODULE_DEVICE_TABLE(of, rcn_cma_match_table);

static struct platform_driver rcn_cma_driver = {
	.driver = {
		.name = "raccoon,cma",
		.of_match_table = of_match_ptr(rcn_cma_match_table),
	},
	.probe = rcn_cma_probe,
	.remove = rcn_cma_remove,
};

int rcn_cma_init(void)
{
	return platform_driver_register(&rcn_cma_driver);
}
module_init(rcn_cma_init);

void rcn_cma_exit(void)
{
	platform_driver_unregister(&rcn_cma_driver);
}
module_exit(rcn_cma_exit);

MODULE_AUTHOR("YOUNGJIN JOO <neoelec@gmail.com>");
MODULE_LICENSE("GPL v2");
