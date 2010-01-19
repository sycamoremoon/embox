/**
 * @file init.c
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 */

#include <types.h>
#include <common.h>
#include <hal/irq_ctrl.h>
#include <kernel/uart.h>
#include <kernel/timers.h>
#include <net/net.h>
#include <fs/rootfs.h>
#include <asm/cache.h>
#include <kernel/init.h>

int hardware_init_hook(void) {
	extern init_descriptor_t *__init_handlers_start, *__init_handlers_end;
	init_descriptor_t ** p_init_desc = &__init_handlers_start;
	//int i, total = (int) (&__init_handlers_end - &__init_handlers_start);
	int level;
	const char *init_name;
	const char *default_init_name = "???";

	/*
	 * Zero level initialization (+ uart)
	 */
	cache_data_enable();
	cache_instr_enable();
	irq_init_handlers();
	uart_init();
	sys_timers_init();

	TRACE("\nStarting Monitor...\n");

	for (level = 0; level <= INIT_MAX_LEVEL; level++) {
		TRACE("\n********* Init level %d:\n", level);
		for (p_init_desc = &__init_handlers_start; p_init_desc
				< &__init_handlers_end; p_init_desc++) {
			init_name = default_init_name;
			if (NULL == (*p_init_desc)) {
				TRACE("Missing init descriptor\n");
				continue;
			}
			if ((*p_init_desc)->level != level) {
				continue;
			}
			if (NULL != (*p_init_desc)->name) {
				init_name = ((*p_init_desc)->name);
			}
			if (NULL == ((*p_init_desc)->init)) {
				TRACE("%s has a broken init handler descriptor.\n",
						init_name);
				continue;
			}
			TRACE("*** Initializing %s\n", (*p_init_desc)->name);
			(*p_init_desc)->init();
		}
	}
	return 0;
}
