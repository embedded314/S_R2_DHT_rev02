#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "main.h"
#include "tmr.h"
#include "ADC.h"
#include "rtc.h"
#include "usart.h"
#include "comm_dht.h"

const uint16_t TCA_period = 0x8FFF;
const uint16_t RTC_period = 0x0064;

int main(void){
	cli();

	CPU_init ();

	WDT_init ();

	ADC_init();
	
	USART_init();
	
	TCA_init( TCA_period );

	RTC_init( RTC_period );

	sei();
	
	while (1) {
		if (TCA_IRQ_flag == 1) {
			//	DHT sensor reading
			DHT_read ();

			//	LSENS reading
			sensor_data.L_val = ADC_L_read();

			TCA_IRQ_flag = 0;
		}

		if (RTC_IRQ_flag == 1) {
			//	Reset UART frame pos to avoid unsychronized reading.
			//if (GW_UART2_frame_decoder_busy_flag == 0) GW_UART2_frame_byte_pos = 0;
			USART_TX_que();

			RTC_IRQ_flag = 0;
		}

		wdt_reset();
	}
}

void CPU_init ( void ) {
	//	set CPU speed to 20Mhz
	//	use Configuration Change Protection (CCP)
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLB = 0;
}

void WDT_init ( void ) {
	//	WDT initialization
	CPU_CCP = 0xD8;
	WDT.CTRLA = WDT_PERIOD_8KCLK_gc;	/* 8K cycles (8.2s) */
	CPU_CCP = 0xD8;
}
