/**
 ******************************************************************************
 * @file    usmart_config.c
 * @brief   USMART调试.
 ******************************************************************************
 * 
 *-----------------------------------------------------------------------------
 * @brief   1.ug参数列表
 * @param   my_func_list
 *  param   预设: "minnie", "yuqi", "shuhua", "reset".
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * @brief   2.快捷执行函数
 * @param   ug
 *  param   idle: 匹配字符串, 会到my_func_list中查找匹配.
 *                (不支持改变参数)
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * @brief   3.函数列表
 * @param   usmart_nametab
 *  param   模板: (void*)func_name,"retval func_name(arg, ...)",
 *-----------------------------------------------------------------------------
 * 
 */


#include "usmart.h"
#include "usmart_str.h"
#include "delay.h"	 	
#include "sys.h"
#include "code.h"
 

// USMART (G)I-DLE
#if 1
char *my_func_list[] = {
	"minnie",
	"yuqi",
	"shuhua", 
	"reset",

	"multi",
	"led"
};
int my_func_len = sizeof(my_func_list) / sizeof(my_func_list[0]);


int usmart_minnie(void){
	int result = 1;
	/* list all */
	printf("my_func_list[%d]: {", my_func_len);
	for(int i = 0; i < my_func_len ;i++){
		printf("\"%s\"", my_func_list[i]);
		if(i != my_func_len - 1) printf(", ");
	}
	printf("}\r\n");
	return result;
}

int usmart_yuqi(void){
	int result = 2;

	return result;
}

int usmart_shuhua(void){
	int result = 3;
	
	return result;
}

int ug(char *idle){
	printf("\r\nit is GIDLE!\r\n");
	int result = 0;
	int match;
	for(match = 0; match < my_func_len + 1 ;match++){
		if(match == my_func_len) return result;
		if(0 == strcmp(idle, my_func_list[match])) break;
	}

	printf("it is %s!\r\n", my_func_list[match]);
	switch(match){
		case 0:
			result = usmart_minnie();
			break;
		case 1:
			result = usmart_yuqi();
			break;
		case 2:
			result = usmart_shuhua();
			break;
		case 3:
			printf("正在重启...\r\n");
			HAL_NVIC_SystemReset();
			break;

		case 4:
			muti_printf_self_test();
			break;
		case 5:
			my_led_self_test();
			break;
		default:
			printf("%s[%d]: 啊?这...\r\n", __FILE__, __LINE__);
			break;
	}

	printf("end of it\r\n");
	return result;
}

#endif
// USMART (G)I-DLE


// 函数列表 & 管理器
#if 1
////////////////////////////用户配置区///////////////////////////////////////////////			 
// extern void led_set(u8 sta);
// extern void test_fun(void(*ledset)(u8),u8 sta);
extern int my_test(void);
//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(u32 nus)",	 
	
	
	
	(void*)ug,"int ug(char *idle)",
	(void*)my_test,"int my_test(void)",

};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   

#endif
// 函数列表 & 管理器

