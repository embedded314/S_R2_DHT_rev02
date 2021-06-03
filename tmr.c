#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "tmr.h"

 void TCA_init( uint16_t period ) {
	TCA0.SINGLE.CMP0 = period;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | (1 << TCA_SINGLE_ENABLE_bp);
	TCA0.SINGLE.CTRLB |= (1 << 4);
	TCA0.SINGLE.INTCTRL |= (1 << 4);
}
ISR(TCA0_CMP0_vect){
	TCA0.SINGLE.INTFLAGS |= (1 << 0);	//	Clear interrupt TCA0
	TCA0.SINGLE.CNT = 0;
	TCA_IRQ_flag = 1;
}


int8_t TCB_init_time_measure_mode() {

	// TCB0.CCMP = 0x0; /* Compare or Capture: 0x0 */

	// TCB0.CNT = 0x0; /* Count: 0x0 */

	TCB0.CTRLB = 0 << TCB_ASYNC_bp      /* Asynchronous Enable: disabled */
	| 0 << TCB_CCMPEN_bp   /* Pin Output Enable: disabled */
	| 0 << TCB_CCMPINIT_bp /* Pin Initial State: disabled */
	| TCB_CNTMODE_PW_gc;   /* Input Capture Pulse-Width measurement */

	// TCB0.DBGCTRL = 0 << TCB_DBGRUN_bp; /* Debug Run: disabled */

	TCB0.EVCTRL = 1 << TCB_CAPTEI_bp    /* Event Input Enable: enabled */
	| 0 << TCB_EDGE_bp    /* Event Edge: enabled */
	| 1 << TCB_FILTER_bp; /* Input Capture Noise Cancellation Filter: enabled */

	TCB0.INTCTRL = 1 << TCB_CAPT_bp; /* Capture or Timeout: enabled */

	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc  /* CLK_PER (No Prescaling) */
	| 1 << TCB_ENABLE_bp   /* Enable: enabled */
	| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
	| 0 << TCB_SYNCUPD_bp; /* Synchronize Update: disabled */

	return 0;
}
void TCB_init_compare_mode( uint16_t period ) {
	TCB0.CCMP = period;
	TCB0.CNT = 0x0; /* Count: 0x0 */

	TCB0.CTRLB = 0 << TCB_ASYNC_bp      /* Asynchronous Enable: disabled */
	| 0 << TCB_CCMPEN_bp   /* Pin Output Enable: disabled */
	| 0 << TCB_CCMPINIT_bp /* Pin Initial State: disabled */
	| TCB_CNTMODE_INT_gc;   /* Input Capture Pulse-Width measurement */

	// TCB0.DBGCTRL = 0 << TCB_DBGRUN_bp; /* Debug Run: disabled */

	TCB0.EVCTRL = 0 << TCB_CAPTEI_bp    /* Event Input Enable: disabled */
	| 0 << TCB_EDGE_bp    /* Event Edge: disabled */
	| 0 << TCB_FILTER_bp; /* Input Capture Noise Cancellation Filter: disabled */

	TCB0.INTCTRL = 0 << TCB_CAPT_bp; /* Capture or Timeout: disabled */

	TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc  /* CLK_PER (No Prescaling) */
	| 1 << TCB_ENABLE_bp   /* Enable: enabled */
	| 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
	| 0 << TCB_SYNCUPD_bp; /* Synchronize Update: disabled */
}
