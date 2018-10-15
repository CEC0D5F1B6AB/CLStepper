#include <stdint.h>
#include <math.h>
#include "stm32f0xx_hal.h"

extern ADC_HandleTypeDef hadc;

uint16_t adc_buf[3];

void UTILS_INIT(){
	HAL_ADCEx_Calibration_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc, (uint32_t*)&adc_buf, 3);
}

float GetTemp1(){
	const float Rp = 10000.0; //10K
	const float T2 = (273.15+25.0);; //T2
	const float Bx = 3950.0; //B
	const float Ka = 273.15;
	
	float Rt = (adc_buf[0]*10000/(0xfff-adc_buf[0]));
	float temp;
	
	temp = Rt/Rp;
	temp = log(temp);//ln(Rt/Rp)
	temp /= Bx;//ln(Rt/Rp)/B
	temp += (1/T2);
	temp = 1/(temp);
	temp -= Ka;
	return temp;
}

float GetTemp2(){
	//return (1430-adc_buf[1]*3300/0xfff)/4.3+25;
	return (1430-adc_buf[1]*1200/adc_buf[2])/4.3+25;
}
