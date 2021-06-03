#ifndef ADC_H_
#define ADC_H_

#define READADC()   (((unsigned int)ADC0.RESH)<<8)|(ADC0.RESL)

volatile uint16_t TADC_val_prev, TADC_val;

void ADC_init ( void ) ;
uint16_t ADC_L_read ( void );

#endif /* ADC_H_ */