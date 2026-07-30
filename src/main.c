#include <avr/io.h>
#include <util/delay.h>

/* 1. طبقات الـ MCAL والـ HAL الأساسية */
#include "adc_interface.h"
#include "uart_interface.h"
#include "LCD_Aip31068_i2c.h"

/* 2. وحدات النظام (Micro Layers) */
#include "scheduler.h"
#include "greenhouse_fsm.h"
#include "control.h"
#include "console.h"
#include "report.h"

int main(void) {
    /* الخطوة الأولى: تهيئة الهاردوير الأساسي مع تمرير NULL للاستعدادات الافتراضية */
    ADC_Init(NULL);
    UART_Init(NULL);
    LCD_Aip31068_Init(NULL);

    /* الخطوة الثانية: تهيئة وحدات النظام والـ FSM */
    // (يمكنك إضافة دوال الـ Init الخاصة بالـ FSM و الـ Control هنا إذا لزم الأمر)

    /* الخطوة الثالثة: تشغيل نظام الجدولة التعاوني */
    SCH_Init();
    // إذا كانت دالة البدء أو التوزيع لها اسم مختلف في scheduler.h، يمكنك التأكد منها، 
    // وغالباً دالة التوزيع الأساسية للمجدول تكون SCH_Dispatch أو ما يُعادلها داخل الملف.

    /* الخطوة الرابعة: حلقة التشغيل المستمرة لإدارة المهام */
    while (1) {
        SCH_Dispatch();
    }
}