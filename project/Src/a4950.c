#include "a4950.h"

#define MICRO_STEP 1024
#define MICRO_CURRENT 1024
#define MOTOR_CURRENT 1024

int16_t table[MICRO_STEP];
uint32_t MAX_CURRENT = MOTOR_CURRENT; //ma

void A4950_EN(uint8_t status){// ma
	if(status){
		MAX_CURRENT = MOTOR_CURRENT;
		A4950_SET_CURRENT(MOTOR_CURRENT/2, MOTOR_CURRENT/2);
	}else{
		//MAX_CURRENT = 0;
		A4950_SET_CURRENT(0, 0);
	}
	//
	
}

void A4950_SET_CURRENT(int32_t a, int32_t b){// ma
	TIM3->CCR1 = (a > 0 ? a : -a) * MAX_CURRENT / MICRO_CURRENT;
	TIM3->CCR2 = (b > 0 ? b : -b) * MAX_CURRENT / MICRO_CURRENT;
}

void A4950_SET_DIRECTION(int32_t a, int32_t b){// ma
	TIM1->CCR1 = TIM1->CCR2 = a + MICRO_CURRENT;
	TIM1->CCR3 = TIM1->CCR4 = b + MICRO_CURRENT;
}

void A4950_INIT(void){
	//gen table
	for(int i=0;i<MICRO_STEP;i++){
		table[i] = MICRO_CURRENT * sin((float)i/MICRO_STEP*6.283185307);
	}
	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	//start current dac
	
	A4950_SET_CURRENT(100, 100);
	A4950_SET_DIRECTION(+MICRO_CURRENT, 0);
	HAL_Delay(100);
}

int A4950_STEP(int16_t dir){
	static uint32_t step = 0;
	
	if(dir > 0){
		step = (step + dir) % MICRO_STEP;
	}else if(dir < 0){
		step = ((step + MICRO_STEP) + dir) % MICRO_STEP;
	}else{
		return 0;
	}
	
	int32_t a = table[step];
	int32_t b = table[(step + MICRO_STEP/4) % MICRO_STEP];
	A4950_SET_DIRECTION(a, b);
	A4950_SET_CURRENT(a, b);
	
	return 1;//(200*64/4096)
}
