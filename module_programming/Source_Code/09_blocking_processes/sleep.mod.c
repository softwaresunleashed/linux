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
	{ 0x4e7ebfd2, "per_cpu__current_task" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xd6a21b73, "remove_proc_entry" },
	{ 0x167e7f9d, "__get_user_1" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x9629486a, "per_cpu__cpu_number" },
	{ 0xb72397d5, "printk" },
	{ 0xacdeb154, "__tracepoint_module_get" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb4390f9a, "mcount" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x2de71793, "module_put" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4292364c, "schedule" },
	{ 0xca15a521, "create_proc_entry" },
	{ 0x7ecb001b, "__per_cpu_offset" },
	{ 0x642e54ac, "__wake_up" },
	{ 0x33d92f9a, "prepare_to_wait" },
	{ 0x9ccb2622, "finish_wait" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B716FF5B3E006903DBD6DB5");
