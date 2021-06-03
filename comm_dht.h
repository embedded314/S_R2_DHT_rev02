#ifndef COMM_DHT_H_
#define COMM_DHT_H_

//#define OW_DEBUG
#ifdef OW_DEBUG
volatile uint16_t DHT_DBG_tab [40];
volatile uint8_t DHT_DBG_cnt;
#endif


//	Clear sensor response flag HAL_sensor_presence_flag
volatile uint8_t HAL_sensor_dht_err_cnt;
volatile uint8_t HAL_sensor_dht_err_flag;


//	Reserve 16b + 16b + 8b = 8b + 8b + 8b +8b + 8b = 40b RAM buffer
//	for a DHT22 sensor incoming frame.
volatile uint8_t DHT_frame_buffer [5];

void DHT_read( void );
void DHT_err ( void );

#endif /* COMM_DHT_H_ */