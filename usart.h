#ifndef UART_H_
#define UART_H_

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define FRAME_TYPE_CTRL_REQ 0x0A    //  request to read data from remote register

#define FRAME_QUE_SIZE	3	//	maximum number of frames to be sent.

#define T_val_q		0
#define H_val_q		1
#define L_val_q		2

#define id_T_val    0x0011
#define id_H_val    0x0021
#define id_L_val    0x0049

volatile uint8_t USART_irq_flag;
volatile uint8_t UART_Tx_req_tab [ FRAME_QUE_SIZE ];
volatile uint8_t UART_frame_que_cnt;
volatile uint8_t GW_UART2_frame_decoder_busy_flag, GW_UART2_frame_byte_pos;

void USART_init( void );
void USART_Transmit(char c);
void USART_ISR_Interrupt(void);
uint8_t USART_TX_que ( void );

uint16_t USART_GHC_frame_enc ( volatile uint8_t frame_type, volatile uint16_t register_code, volatile uint16_t data_to_send);


#endif /* UART_H_ */