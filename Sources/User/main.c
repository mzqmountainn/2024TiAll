
/*
 * FreeRTOS STC port V1.0.2
 * 
 * STC32G12K128功能简介：
 *    2组 CAN, 3组Lin, USB, 32位 8051, RMB5,
 *    高精准12位ADC, DMA 支持(TFT彩屏, ADC, 4组串口, SPI, I2C )
 *  
 *  各位用户在后续的使用过程中，如果发现有不正确的地方或者
 *  有可改善的地方，请和STC联系，由STC官方进行统一修改并更新版本
 *
 *  若您的代码或文章中需要引用本程序包中的部分移植代码，请在您的
 *  代码或文章中注明参考或使用了STC的FreeRTOS的移植代码
*/

#include "FreeRTOS.h"
#include "task.h"
#include "System_init.h"

// #include "display.h"
// #include "rtc.h"
// #include "ntc.h"
// #include "adckey.h"
// #include "uart2_3.h"
// #include "MatrixKey.h"
// #include "i2c_ps.h"
// #include "pwmb.h"
#include "mzqGlobal.h"
#include "Semphr.h"
#include "STC32G_UART.h"
#include "lcd.h"
#include "lcd_init.h"
void vSystemInit( void );
void motorInit(void);
extern void outputSpeed(void *pvParameters);
extern void PWMupdate(void *pvParameters);
extern QueueHandle_t pwmUpdateSignal;
extern void openMVgetAngle(void *pvParameters);
extern void moudle8266(void *pvParameters);
extern void uart4frame(void *pvParameters);
extern void lcd(void *pvParameters);
void main( void )
{
    /* 系统初始化 */
    vSystemInit();
    motorInit();
    PidInit(&pid1);
    PidInit(&pid2);
    PidInit(&pidLoc);
    pid1.kp = 0.1;
    pid1.ki = 1;
    pid2.kp = 0.1;
    pid2.ki = 1;
    pid2.kd = 0.18;
    pidLoc.kp = 5;
    pwmUpdateSignal = xSemaphoreCreateBinary();
    /* 创建任务 */
    xTaskCreate((TaskFunction_t )outputSpeed,
                (const char*    )"outputTIM",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+2,
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )PWMupdate,
                (const char*    )"PWMupdate",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+3,
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )openMVgetAngle,
                (const char*    )"openMVgetAngle",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+4,
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )moudle8266,
                (const char*    )"moudle8266",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+4,
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )uart4frame,
                (const char*    )"moudle8266",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+4,
                (TaskHandle_t*  )NULL);
    xTaskCreate((TaskFunction_t )lcd,
                (const char*    )"lcd",
                (uint16_t       )configDEFAULT_STACK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )(configDEFAULT_PRIORITIES)+3,
                (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vDisplayTask,
    //             (const char*    )"DISPLAY",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vRtcTask,
    //             (const char*    )"RTC",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES + 1),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vAdcKeyTask,
    //             (const char*    )"ADCKEY",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vNtcTask,
    //             (const char*    )"NTC",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vUart2_3Task,
    //             (const char*    )"UART2_3",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )1000,  //注意pvParameters参数传地址或者立即数时只有低24位是有效位，最高8位编译时会自动填0. 可通过变量或者常量传送32字节数据.
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vMatrixKeyTask,
    //             (const char*    )"MatrixKey",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vI2C_PsTask,
    //             (const char*    )"I2C_PS",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);
    // xTaskCreate((TaskFunction_t )vPWMB_LedTask,
    //             (const char*    )"PWMB_LED",
    //             (uint16_t       )configDEFAULT_STACK_SIZE,
    //             (void*          )NULL,
    //             (UBaseType_t    )(configDEFAULT_PRIORITIES),
    //             (TaskHandle_t*  )NULL);

    /* 启动任务调度器开始任务调度 */
    vTaskStartScheduler();

    /* 正常情况下不会运行到此处 */
    while (1){};
}
void motorInit(void){
  STBY_6612 = 1;
  AIN1_6612 = 0;
  AIN2_6612 = 0;
  BIN1_6612 = 0;
  BIN2_6612 = 0;
}
