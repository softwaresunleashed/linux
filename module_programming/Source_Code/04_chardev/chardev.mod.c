#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd0b96c58, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xec7095ba, "__register_chrdev" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x9629486a, "per_cpu__cpu_number" },
	{ 0xb72397d5, "printk" },
	{ 0xacdeb154, "__tracepoint_module_get" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb4390f9a, "mcount" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x2de71793, "module_put" },
	{ 0x7ecb001b, "__per_cpu_offset" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C8C2B547A1AC8328A231BCA");
