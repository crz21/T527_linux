#include <linux/configfs.h>
#include <linux/of.h>
#include <linux/of_property.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/list.h>

/* 表示一个设备树节点的 configfs 实例 */
struct of_configfs_node {
	struct config_item item;
	struct device_node *dn;       // 关联的设备树节点
	char name[OF_NAME_LEN];       // 节点名称（如 "node@1"）
	struct list_head props;       // 节点属性列表
	struct mutex lock;            // 保护节点操作的互斥锁
};

/* 表示设备树节点的一个属性 */
struct of_configfs_prop {
	struct config_item item;
	struct property *prop;        // 关联的设备树属性
	struct list_head node_entry;  // 挂入节点的属性链表
};

/* 类型转换宏（从 config_item 到自定义结构体） */
#define to_of_node(item) container_of(item, struct of_configfs_node, item)
#define to_of_prop(item) container_of(item, struct of_configfs_prop, item)

/* 设备树节点的 configfs 操作方法（简化示例） */
static int of_node_configfs_validate(struct config_item *item)
{
	// 验证节点名称、属性等合法性
	return 0;
}

static void of_node_configfs_release(struct config_item *item)
{
	struct of_configfs_node *node = to_of_node(item);
	// 释放节点资源（如属性、设备树节点引用等）
	mutex_destroy(&node->lock);
	kfree(node);
}

/* 定义节点的 configfs 操作集 */
static struct configfs_item_operations of_node_ops = {
	.validate = of_node_configfs_validate,
	.release = of_node_configfs_release,
	// 可添加属性读写、重命名等操作
};

/* 根目录的 configfs 类型定义 */
static struct config_item_type of_configfs_root_type = {
	.ct_item_ops = &of_node_ops,
	.ct_owner = THIS_MODULE,
};

/* configfs 子系统根目录 */
static struct configfs_subsystem of_configfs_subsys = {
	.su_group = {
		.cg_item = {
            .ci_namebuf = "device-tree",
			.ci_type = &of_configfs_root_type,
		},
	},
	.su_mutex = __MUTEX_INITIALIZER(of_configfs_subsys.su_mutex),
};

/* 模块初始化 */
static int __init of_configfs_init(void)
{
	int ret;
	// 初始化子系统并注册到 configfs
	ret = configfs_register_subsystem(&of_configfs_subsys);
	if (ret)
		pr_err("Failed to register OF configfs subsystem\n");
	return ret;
}

/* 模块退出 */
static void __exit of_configfs_exit(void)
{
	configfs_unregister_subsystem(&of_configfs_subsys);
}

module_init(of_configfs_init);
module_exit(of_configfs_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("OF configfs support for 6.1 kernel");
MODULE_AUTHOR("Your Name");