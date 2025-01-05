// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Andriy Zinchenko <andriyzinchenko@gmail.com>");
MODULE_DESCRIPTION("Modified Test Data Kernel Module");
MODULE_LICENSE("Dual BSD/GPL");

struct node_data {
	struct list_head node;
	ktime_t time_stamp;
};

static LIST_HEAD(node_list);

static unsigned int print_count = 1;
module_param(print_count, uint, S_IRUGO);
MODULE_PARM_DESC(print_count, "How many times to print 'Test data'");

static void list_cleanup(void)
{
	struct node_data* entry, * tmp;

	list_for_each_entry_safe(entry, tmp, &node_list, node) {
		list_del(&entry->node);
		kfree(entry);
	}
	pr_info("List cleaned up.\n");
}

static void display_list(void)
{
	struct node_data* entry;

	list_for_each_entry(entry, &node_list, node) {
		pr_info("Recorded time: %lld ns\n", ktime_to_ns(entry->time_stamp));
	}
}

static int __init mod_startup(void)
{
	struct node_data* new_node;
	int idx;

	if (print_count == 0 || (print_count >= 3 && print_count <= 7)) {
		pr_warn("Notice: print_count is 0 or in the range 3 to 7.\n");
	}

	if (print_count > 7) {
		pr_err("Error: print_count is greater than 7. Cannot proceed.\n");
		return -EINVAL;
	}

	for (idx = 0; idx < print_count; idx++) {
		new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
		if (!new_node) {
			pr_err("Memory allocation failed for node.\n");
			return -ENOMEM;
		}

		new_node->time_stamp = ktime_get();
		list_add_tail(&new_node->node, &node_list);
		pr_info("Test data [%d]\n", idx + 1);
	}

	return 0;
}

static void __exit mod_cleanup(void)
{
	display_list();
	list_cleanup();
	pr_info("Module successfully unloaded.\n");
}

module_init(mod_startup);
module_exit(mod_cleanup);
