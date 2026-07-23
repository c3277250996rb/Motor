/*金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮金米妮*/
#include "code.h"


#if 723

extern __IO uint16_t IC2Value;
extern __IO uint16_t IC1Value;
extern __IO float DutyCycle;
extern __IO float Frequency;
int my_test(void){
    
		HAL_Delay(500);
		printf("IC1Value = %d  IC2Value = %d ",IC1Value,IC2Value);
		printf("占空比：%0.2f%%   频率：%0.2fHz\r\n",DutyCycle,Frequency);	
}
#endif

