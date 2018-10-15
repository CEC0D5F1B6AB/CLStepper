#include <stdint.h>
#include <math.h>
#include "stm32f0xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

void A4950_EN(uint8_t status);
void A4950_INIT(void);
void A4950_SET_CURRENT(int32_t a, int32_t b);
void A4950_SET_DIRECTION(int32_t a, int32_t b);
int A4950_STEP(int16_t dir);
