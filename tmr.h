#ifndef TMR_H_
#define TMRH_

void TCA_init( uint16_t period );

volatile uint8_t TCB0_int_flag;
void TCB_init_compare_mode( uint16_t period );
int8_t TCB_init_time_measure_mode();

#endif /* TMR_H_ */