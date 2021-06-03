
#ifndef WATCHDOG_TIMER_H_INCLUDED
#define WATCHDOG_TIMER_H_INCLUDED

#include <compiler.h>
#include <wdt.h>
#include <ccp.h>

#ifdef __cplusplus
extern "C" {
	#endif

	int8_t WDT_0_init();

	#ifdef __cplusplus
}
#endif

#endif /* WATCHDOG_TIMER_H_INCLUDED */
