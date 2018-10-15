#include "tle5012.h"

static int32_t last_pos;

//int32_t cur_pos;

//void update_cur(void){
//	if(HAL_GPIO_ReadPin(IFB_GPIO_Port, IFB_Pin) == GPIO_PIN_RESET){
//		cur_pos += 1;
//	}else{
//		cur_pos -= 1;
//	}
//}

uint8_t CRC8(uint8_t *u8_data,uint8_t u8_len)
{
    uint8_t i, j;
    uint8_t u8_crc8;
    uint8_t u8_poly;
    
    u8_crc8 = 0xFF;
    u8_poly = 0x1D;
    
    for (i = 0; i < u8_len; i++)
    {
        u8_crc8 ^= u8_data[i];
        
        for (j = 0; j < 8; j++)
        {
            if (u8_crc8 & 0x80)
            {
                u8_crc8 = (u8_crc8 << 1) ^ u8_poly;
            }
            else
            {
                u8_crc8 <<= 1;
            }
        }
    }
    
    u8_crc8 ^= (uint8_t)0xFF;
    return u8_crc8;
}

uint16_t TLE5012_GetAngle(void){
	uint16_t sbuf = 0x8021, rbuf = 0;
	while(!(HAL_SPI_Transmit(&hspi1, (uint8_t *)&sbuf, 1, 1) == 0 && HAL_SPI_Receive(&hspi1, (uint8_t *)&rbuf, 1, 0xff) == 0));
	return rbuf&0x7FFF;
}

uint16_t TLE5012_GET_IIF_CNT(void){
	uint16_t sbuf = 0x8201, rbuf = 0;
	while(!(HAL_SPI_Transmit(&hspi1, (uint8_t *)&sbuf, 1, 1) == 0 && HAL_SPI_Receive(&hspi1, (uint8_t *)&rbuf, 1, 0xff) == 0));
	return rbuf&0x3FFF;
}

float TLE5012_GET_TEMPER(void){
	uint16_t sbuf = 0x8051, rbuf = 0;
	while(!(HAL_SPI_Transmit(&hspi1, (uint8_t *)&sbuf, 1, 1) == 0 && HAL_SPI_Receive(&hspi1, (uint8_t *)&rbuf, 1, 0xff) == 0));
	int8_t temper_value = rbuf&0xff;
	return (temper_value+152)/2.776;
}

int16_t TLE5012_GET_AREV(void){
	uint16_t sbuf = 0x8041, rbuf[2];
	while(!(HAL_SPI_Transmit(&hspi1, (uint8_t *)&sbuf, 1, 1) == 0 && HAL_SPI_Receive(&hspi1, (uint8_t *)&rbuf, 2, 0xff) == 0));
	return rbuf[1];
	if((rbuf[0]&0x100) == 0x100){
		return -(0x100-(rbuf[0]&0xff));
	}else{
		return (rbuf[0]&0xff);
	}
}

int32_t TLE5012_GET_RAW_POS(void){
	uint16_t buf[10];
	uint8_t crc_buf[16];
	buf[0] = 0x8023;

	while(1){
		if(HAL_SPI_Transmit(&hspi1, (uint8_t *)&buf[0], 1, 0xffff)) continue;
		if(HAL_SPI_Receive(&hspi1, (uint8_t *)&buf[1], 4, 0xffff)) continue;

		//translate
		for(int i=0;i<4;i++){
			crc_buf[i*2] = (uint8_t) (buf[i] >> 8);
			crc_buf[i*2+1] = (uint8_t) (buf[i] & 0xFF);
		}

		//crc
		if(	(buf[4]&0x7000) == 0x7000
				&& (buf[4]&0xff) == CRC8(crc_buf, 8)
		) break;
		
	}

	if((buf[3]&0x100) == 0x100){
		return -((0x100-buf[3]&0xff)<<15) | (buf[1]&0x7FFF);
	}else{
		return ((buf[3]&0xff)<<15)|(buf[1]&0x7FFF);
	}
	
}


int32_t TLE5012_GET_POS(void){
	int32_t pos;
	for(int i=0;i<0xff;i++){
		pos = TLE5012_GET_RAW_POS();
		//limit
		if(pos <= last_pos + 0xff 
			&& pos >= last_pos - 0xff) break;
	}
	
	last_pos = pos;
	
	return pos;
}

void TLE5012_SET_REG(uint16_t cmd, uint16_t value){
	uint16_t buf[3];

	buf[0] = cmd;
	buf[1] = value;
	
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&buf[0], 2, 0xffff);
	HAL_SPI_Receive(&hspi1, (uint8_t *)&buf[2], 1, 0xffff);
}

void TLE5012_INIT(void){
	//restart
	TLE5012_SET_REG(0x5011, 0xFFFF);
	
	//Step/Dir mode
	TLE5012_SET_REG(0x5061, 0x0002);
	
	last_pos = TLE5012_GET_RAW_POS();
}
