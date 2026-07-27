#include "my_string.h"


// multi printf
#if 1
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define mpa_MAX_STR_LEN 1024  // 结果字符串最大长度


/**
 * @brief  Concatenate array(s).
 * @param  num_arrays: Number of array(s).
 * @param  delimiter: Separator.
 * @param  format: Item format.
 *         @arg "%d": Dec
 *         @arg "%f": Float
 *         @arg "%x": Hex
 *         @arg "%s": String
 *         @arg ...
 * @retval Char*
 */
char* multi_printf_arrays(int num_arrays, const char* delimiter, const char* format, ...) {
    static char result[mpa_MAX_STR_LEN];  // 静态缓冲区存储结果
    result[0] = '\0';  // 初始化为空字符串

    va_list args;
    va_start(args, format);

    char buffer[32];  // 临时存储格式化后的数据
    int first = 1;    // 用于避免首个元素前的分隔符

    for (int i = 0; i < num_arrays; i++) {
        
        if (strcmp(format, "%f") == 0) {
            float* array = va_arg(args, float*);  // 处理浮点数数组
            int size = va_arg(args, int);

            for (int j = 0; j < size; j++) {
                if (!first) {
                    strcat(result, delimiter);
                }
                first = 0;
                snprintf(buffer, sizeof(buffer), format, array[j]);  // 处理浮点数
                strcat(result, buffer);
            }
        } else {
            int* array = va_arg(args, int*);  // 处理整数数组
            int size = va_arg(args, int);

            for (int j = 0; j < size; j++) {
                if (!first) {
                    strcat(result, delimiter);
                }
                first = 0;
                snprintf(buffer, sizeof(buffer), format, array[j]);  // 处理整数
                strcat(result, buffer);
            }
        }
    }

    va_end(args);
    return result;
}


/**
 * @brief  Concatenate array(s) (direction).
 * @param  num_arrays: Number of array(s).
 * @param  delimiter: Separator.
 * @param  format: Item format.
 *         @arg "%d": Dec
 *         @arg "%f": Float
 *         @arg "%x": Hex
 *         @arg "%s": String
 *         @arg ...
 * @retval Char**
 */
char** multi_printf_arrays_direct(int num_arrays, const char* delimiter, const char* format, ...) {
    static char result[mpa_MAX_STR_LEN];  // 静态缓冲区存储结果
    result[0] = '\0';  // 初始化为空字符串

    va_list args;
    va_start(args, format);

    char buffer[32];  // 临时存储格式化后的数据
    int first = 1;    // 用于避免首个元素前的分隔符

    for (int i = 0; i < num_arrays; i++) {
        
        if (strcmp(format, "%f") == 0) {
            float* array = va_arg(args, float*);  // 处理浮点数数组
            int size = va_arg(args, int);

            for (int j = 0; j < size; j++) {
                if (!first) {
                    strcat(result, delimiter);
                }
                first = 0;
                snprintf(buffer, sizeof(buffer), format, array[j]);  // 处理浮点数
                strcat(result, buffer);
            }
        } else {
            int* array = va_arg(args, int*);  // 处理整数数组
            int size = va_arg(args, int);

            for (int j = 0; j < size; j++) {
                if (!first) {
                    strcat(result, delimiter);
                }
                first = 0;
                snprintf(buffer, sizeof(buffer), format, array[j]);  // 处理整数
                strcat(result, buffer);
            }
        }
    }

    va_end(args);
    return (char **) result;
}

int muti_printf_self_test() {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5};
    int arr3[] = {16, 17, 18, 19};
    float arr4[] = {1.2f, 2.3f};

    // 示例：使用逗号作为分隔符，数字以 %d 格式输出
    char* result = multi_printf_arrays(3, ",", "%d", arr1, 3, arr2, 2, arr3, 4);
    printf("拼接结果: %s\r\n", result);

    // 示例：使用空格作为分隔符，数字以十六进制格式输出
    printf("拼接结果 (十六进制): %s\r\n", multi_printf_arrays_direct(3, " ", "0x%02X", arr1, 3, arr2, 2, arr3, 4));

    // 示例：使用逗号作为分隔符，数字以 %.3f 格式输出
    /* 有bug */
        // result = multi_printf_arrays(1, ",", "%.3f", arr4, 2);
    /* ---- */
    result = multi_printf_arrays(1, ",", "%f", arr4, 2);
    printf("拼接结果 (float数组):%s\r\n", result);

    for(int i = 0; i < 1000*100; i++);
    return 0;
}


#endif
// multi printf





