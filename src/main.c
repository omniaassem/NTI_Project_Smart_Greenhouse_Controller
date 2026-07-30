int main(void)
{
    /*================ MCAL =================*/
    GPIO_Init();

    ADC_ConfigType adcConfig = {0};
    ADC_Init(&adcConfig);

    Timer_ConfigType timerConfig =
    {
        .mode         = TIMER_MODE_CTC,
        .prescaler    = TIMER_CLOCK_DIV_1024,   // كما في الـ README
        .compareValue = 77                      // 10 ms @ 8 MHz
    };
    Timer_Init(&timerConfig);

    UART_ConfigType uartConfig = {0};
    UART_Init(&uartConfig);

    I2C_MasterInit();

    EXTI_ConfigType extiConfig = {0};
    EXTI_Init(&extiConfig);

    /*================ HAL =================*/

    SEN_Init();
    BTN_Init();
    LCD_Init();
    ACT_Init();

    /*================ APP =================*/

    CON_Init();
    RPT_Init();
    CTRL_Init();      // لو موجودة
    FSM_Init();

    sei();

    uint16_t lastTick = 0;
    uint16_t tickCounter = 0;

    while(1)
    {
        uint16_t currentTick;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            currentTick = g_tick;      // متغير volatile يزيد داخل ISR
        }

        if(currentTick == lastTick)
            continue;

        lastTick = currentTick;
        tickCounter++;

        /*========== 10 ms ==========*/
        BTN_Poll();
        FSM_Run();

        /*==========20 ms===========*/
        if((tickCounter % 2U) == 0U)
            CON_Process();

        /*==========100 ms==========*/
        if((tickCounter % 10U) == 1U)
            SEN_Scan();

        /*==========200 ms==========*/
        if((tickCounter % 20U) == 3U)
            CTRL_Update();

        /*==========500 ms==========*/
        if((tickCounter % 50U) == 5U)
            LCD_Refresh();

        /*==========5 sec===========*/
        if((tickCounter % 500U) == 7U)
            RPT_SendStatus();

        /* EEPROM Task محذوفة */
    }
}