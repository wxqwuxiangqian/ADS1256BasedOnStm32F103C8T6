
#include "stm32f10x.h"

void TIM3_Int_Init(u16 arr,u16 psc);


#define CLI()      __set_PRIMASK(1)		/* �ر����ж� */  
#define SEI()      __set_PRIMASK(0)				/* �������ж� */ 
