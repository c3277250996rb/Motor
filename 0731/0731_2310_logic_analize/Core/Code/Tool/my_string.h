#ifndef __MY_STRING_H
#define __MY_STRING_H

#include "code.h"


// multi printf
#if 1
    char* multi_printf_arrays(int num_arrays, const char* delimiter, const char* format, ...);

#endif
// multi printf


// USART6
#define USE_DEBUG_PRINTF_VIEWER_USART6
void init_debug_printf_usart6(void);
void ITM_SendString(char *str);


#endif
