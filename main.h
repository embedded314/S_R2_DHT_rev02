#ifndef MAIN_H_
#define MAIN_H_

//	Required sensor data structure.
volatile struct {
	uint16_t H_val;
	uint16_t T_val;
	uint16_t L_val;
	uint16_t H_prev;
	uint16_t T_prev;
	uint16_t L_prev;
	uint8_t CRC_val;
} sensor_data;

volatile uint8_t RTC_IRQ_flag;
volatile uint8_t TCA_IRQ_flag;

void CPU_init ( void );
void WDT_init ( void );

#endif /* MAIN_H_ */