#include <avr/wdt.h>
#include "main.h"
#include "usart.h"

uint16_t testVal;
uint8_t testValA, testValB;

uint8_t USART_TX_que ( void ) {
	//	override RX ACK, all frames are always sent.
	
	//	Set "T" frame flag to transmit.
	UART_Tx_req_tab [ T_val_q ] = 1;
	//	Set "H" frame flag to transmit.
	UART_Tx_req_tab [ H_val_q ] = 1;
	//	Set "L" frame flag to transmit.
	UART_Tx_req_tab [ L_val_q ] = 1;

	while ((UART_Tx_req_tab == 0) && (UART_frame_que_cnt < sizeof (UART_Tx_req_tab))) {
		UART_frame_que_cnt++;
	}
	if (UART_Tx_req_tab [UART_frame_que_cnt] == 1) {
		switch (UART_frame_que_cnt) {
			case T_val_q:
				USART_GHC_frame_enc ( FRAME_TYPE_CTRL_REQ, id_T_val, sensor_data.T_val);
				break;
			case H_val_q:
				USART_GHC_frame_enc ( FRAME_TYPE_CTRL_REQ, id_H_val, sensor_data.H_val);
				break;
			case L_val_q:
				USART_GHC_frame_enc ( FRAME_TYPE_CTRL_REQ, id_L_val, sensor_data.L_val);
				break;
		}
	}
	PORTA.DIR |= PIN6_bm;
	PORTA.OUT |= PIN6_bm;
	if (UART_frame_que_cnt < sizeof (UART_Tx_req_tab)) 
		UART_frame_que_cnt++; 
	else 
		UART_frame_que_cnt = 0;
	return 0;
}
uint16_t USART_GHC_frame_enc ( volatile uint8_t frame_type, volatile uint16_t register_code, volatile uint16_t data_to_send) {
	USART_Transmit( 0x00 );    //  synchronization
	USART_Transmit( 0x00 );
	
	//  ---- frame type
	USART_Transmit( frame_type );
	
	//  ---- source Id 3B + 2B
	#define ID1 0b00000001  //  0x01 -closed area controllers
	#define ID2 0b00000000  //  0x00 -PASV (read only)
	#define ID3 0b00001000  //  0x08 -sensor T,H,L (R2)
	USART_Transmit( ID1 );
	USART_Transmit( ID2 );
	USART_Transmit( ID3 );
	
	//  ---- 2 * 1B	serial ID
	USART_Transmit( SIGROW_SERNUM0 );
	USART_Transmit( SIGROW_SERNUM1 );

	//	---- dest ID 3B + 2B
	#define DEST_ID1 0b00000001 //  0x01
	#define DEST_ID2 0b00001000 //  0x08
	#define DEST_ID3 0b00000010 //  0x02
	USART_Transmit( DEST_ID1 );
	USART_Transmit( DEST_ID2 );
	USART_Transmit( DEST_ID3 );
	//  ---- 2 * 1B	serial ID
	USART_Transmit( 0x00 );    //  0x00	received remote Id
	USART_Transmit( 0x00 );    //  0x00 received remote Id
	
	//	---- requested DATA ID
	uint8_t frame_crc = 0;
	//  ---- frame size
	USART_Transmit( 0x05 );
	//  ---- command
	USART_Transmit( 0x12 );
	
	//  ---- data register
	testVal = 0;
	testValA = 0;
	testVal  = register_code;
	testValA = (testVal >> 8);
	testValB = testVal & 0xff;

	frame_crc += testValA;
	frame_crc += testValB;

	USART_Transmit( testValA );
	USART_Transmit( testValB );
	
	testVal = 0;
	testValA = 0;
	testVal  = data_to_send;
	
	testValA = (testVal >> 8);
	testValB = testVal & 0xff;;

	frame_crc += testValA;
	frame_crc += testValB;

	USART_Transmit( testValA );
	USART_Transmit( testValB );
	
	USART_Transmit( frame_crc );
	
	return register_code;
}
void USART_init( void ) {
	//	Set pin 6 as a TX line
	PORTA.DIR |= PIN6_bm;
	PORTA.OUT |= PIN6_bm;
	
	//	Set pin 7 as a RX line
	PORTA.DIR &= ~PIN7_bm;

	//	USART 9600 8 N 1
	USART0.BAUD =  (uint16_t)USART0_BAUD_RATE(9600);

	USART0.CTRLA = 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
	| 0 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
	| 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
	| USART_RS485_OFF_gc /* RS485 Mode disabled */
	| 0 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
	| 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
	| 0 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */

	USART0.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
	| 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
	| 0 << USART_RXEN_bp     /* Receiver enable: disabled */
	| USART_RXMODE_NORMAL_gc /* Normal mode */
	| 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
	| 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */
}

void USART_Transmit(char c)
{
	while (!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = c;
}/**
* \brief Get recieved data from USART_0
*
* \return Data register from USART_0 module
*/
uint8_t USART_0_get_data()
{
	return USART0.RXDATAL;
}

/**
* \brief Check if the USART has received data
*
* \return The status of USART RX data ready check
* \retval true The USART has received data
* \retval false The USART has not received data
*/
uint8_t USART_0_is_rx_ready()
{
	return (USART0.STATUS & USART_RXCIF_bm);
}

/**
* \brief Check if USART_0 data is transmitted
*
* \return Receiver ready status
* \retval true  Data is not completely shifted out of the shift register
* \retval false Data completely shifted out if the USART shift register
*/
uint8_t USART_0_is_tx_busy()
{
	return (!(USART0.STATUS & USART_TXCIF_bm));
}

/**
* \brief Read one character from USART_0
*
* Function will block if a character is not available.
*
* \return Data read from the USART_0 module
*/
uint8_t USART_0_read()
{
	while (!(USART0.STATUS & USART_RXCIF_bm));
	return USART0.RXDATAL;
}
