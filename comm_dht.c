#include <avr/io.h>
#include <avr/wdt.h>

#include "main.h"
#include "tmr.h"
#include "comm_dht.h"

const uint16_t TCB_period = 0x5000;	//	1ms period

uint8_t DHT_frame_byte_cnt;
void DHT_read( void ) {
	//	Clear received bits counter HAL_frame_bit_mask.
	
	//	Add volatile if you want to see result.
	uint16_t HAL_frame_bit_mask = 0;
	//	Clear received bytes counter DHT_frame_byte_cnt.
	uint8_t DHT_frame_byte_cnt = 0;

#ifdef OW_DEBUG
	for (uint8_t i = 0; i< 40; i++){
		DHT_DBG_tab [ i ] = 0;
	}
#endif
	
	//	----	TIMER B configuration to send initial 1000ms signal into 1W pin
	
	//	Set 1W pin to Lo state
 	PORTA.DIR |= PIN1_bm;
 	PORTA.OUT &= ~PIN1_bm;

	//	1ms initial time period
	TCB_init_compare_mode( TCB_period );
	
  	while (!(TCB0.INTFLAGS & TCB0_INTFLAGS));
	TCB0.INTFLAGS = TCB_CAPT_bm;
	
	//	Set 1W pin as input.
  	PORTA.DIR &= ~PIN1_bm;

	//	TCB event configuration
	EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_PORTA_PIN1_gc; /* Asynchronous Event from Pin PA1 */
	EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH0_gc; /* Asynchronous Event Channel 0 */

	//	Set TCB in time measurement mode.
	TCB_init_time_measure_mode();
#ifdef OW_DEBUG
	DHT_DBG_cnt = 0;
#endif
	//	Ignore first two falling edges: transmission start timeout and sensor ACK.
	for (uint8_t i=0;i<2;i++) {
		//	Wait for a interrupt generated on falling edge.
 		while (!(TCB0.INTFLAGS & TCB0_INTFLAGS)) {
			//	no sensor timeout protection
  			if (TCB0.CNT > 5000) {
  				DHT_frame_byte_cnt = 5;
  				HAL_frame_bit_mask = 0;
				TCB0.INTFLAGS = TCB_CAPT_bm;
  				break;
  			}
 		};
		TCB0.INTFLAGS = TCB_CAPT_bm;
	}
	//	read sensor data
	uint8_t data_buffer;
	for (DHT_frame_byte_cnt = 0; DHT_frame_byte_cnt < 5; DHT_frame_byte_cnt++) {
		//	MSB order
		//	Reset bit mask
		HAL_frame_bit_mask = 128;
		
		//	reset value on input buffer 
		*(DHT_frame_buffer + DHT_frame_byte_cnt) = 0;
		
		while (HAL_frame_bit_mask > 0){
			//	Wait for a interrupt generated on falling edge
 			while (!(TCB0.INTFLAGS & TCB0_INTFLAGS)) {
 				if (TCB0.CNT > 5000) {
 					//	End of transmission
 					DHT_frame_byte_cnt = 5;
 					HAL_frame_bit_mask = 0;
					TCB0.INTFLAGS = TCB_CAPT_bm;
 					break;
 				}
 			};
			TCB0.INTFLAGS = TCB_CAPT_bm;

			if (TCB0.CCMP > 1000) {
				//	zaktualizuj bit na pozycji HAL_frame_bit_mask w bajcie DHT_frame_byte_cnt
				data_buffer = *(DHT_frame_buffer + DHT_frame_byte_cnt);
				*(DHT_frame_buffer + DHT_frame_byte_cnt) = HAL_frame_bit_mask | data_buffer;
				
			}
#ifdef OW_DEBUG
			DHT_DBG_tab [DHT_DBG_cnt] = TCB0.CCMP;
			//DHT_DBG_tab [DHT_DBG_cnt] = HAL_frame_bit_mask;
			DHT_DBG_cnt++;
#endif
			//	decrease bit mask
			HAL_frame_bit_mask /= 2;
		} ;
	}
	//	End of transmission
	
	
	uint8_t dht_CRC = 0;
	dht_CRC = *(DHT_frame_buffer) + *(DHT_frame_buffer + 1) + *(DHT_frame_buffer + 2) + *(DHT_frame_buffer + 3);
	if (*(DHT_frame_buffer + 4) == dht_CRC) {
		//	Update sensor data structure if CRC value is correct and the read values are within the range.
		uint16_t H,T;
		H = (*(DHT_frame_buffer) << 8) | ( *(DHT_frame_buffer + 1) & 0xff);
		T = (*(DHT_frame_buffer + 2) << 8) | ( *(DHT_frame_buffer + 3) & 0xff);
		wdt_reset();

		if ((H > 0) && (H < 1000)) sensor_data.H_val = H;

		if ((T > 100) && (T < 500)) sensor_data.T_val = T;
		
		if (HAL_sensor_dht_err_cnt > 0) HAL_sensor_dht_err_cnt--;
		if (HAL_sensor_dht_err_cnt < 5) HAL_sensor_dht_err_flag = 0;
	} else {
		DHT_err();
	}
	
	//	turn Off TCA
	TCB0.CTRLA = 0 << TCB_ENABLE_bp;
}
void DHT_err ( void ) {
	if (HAL_sensor_dht_err_cnt < 100) HAL_sensor_dht_err_cnt++;
	if (HAL_sensor_dht_err_cnt == 100) HAL_sensor_dht_err_flag = 1;
}
