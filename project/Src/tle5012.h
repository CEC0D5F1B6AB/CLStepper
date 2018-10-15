#include <stdint.h>
#include "stm32f0xx_hal.h"

extern SPI_HandleTypeDef hspi1;

//void update_cur(void);
void TLE5012_INIT(void);
uint16_t TLE5012_GetAngle(void);
float TLE5012_GET_TEMPER(void);
int16_t TLE5012_GET_AREV(void);
int32_t TLE5012_GET_POS(void);

