#include "stm32f10x.h"


/* =========================================================
 * TIM1 PWM
 *
 * TIM1_CH1 -> PA8  -> LED1 -> 10%
 * TIM1_CH2 -> PA9  -> LED2 -> 30%
 * TIM1_CH3 -> PA10 -> LED3 -> 50%
 * TIM1_CH4 -> PA11 -> LED4 -> 70%
 *
 * PWM frequency = 1 kHz
 *
 * Timer clock = 72 MHz
 * Prescaler   = 71
 * Counter     = 1 MHz
 * ARR         = 999
 *
 * PWM frequency:
 *
 * 1 MHz / 1000 = 1 kHz
 * ========================================================= */


/* =========================================================
 * CẤU HÌNH GPIO
 * ========================================================= */

static void PWM_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    /*
     * Bật clock GPIOA
     * Bật clock AFIO
     */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO,
        ENABLE
    );


    /*
     * PA8  = TIM1_CH1
     * PA9  = TIM1_CH2
     * PA10 = TIM1_CH3
     * PA11 = TIM1_CH4
     *
     * Alternate Function Push-Pull
     */
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_8 |
        GPIO_Pin_9 |
        GPIO_Pin_10 |
        GPIO_Pin_11;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );
}


/* =========================================================
 * CẤU HÌNH TIM1 PWM
 * ========================================================= */

static void TIM1_PWM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_OCInitTypeDef TIM_OCInitStructure;


    /*
     * Bật clock TIM1
     */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_TIM1,
        ENABLE
    );


    /* =====================================================
     * CẤU HÌNH TIMER
     * ===================================================== */

    /*
     * Timer clock = 72 MHz
     *
     * PSC = 71
     *
     * 72 MHz / (71 + 1)
     * = 1 MHz
     */
    TIM_TimeBaseStructure.TIM_Prescaler = 71;


    /*
     * Đếm từ 0 -> 999
     *
     * ARR = 999
     *
     * Có 1000 mức đếm
     */
    TIM_TimeBaseStructure.TIM_Period = 999;


    /*
     * Clock division = 1
     */
    TIM_TimeBaseStructure.TIM_ClockDivision =
        TIM_CKD_DIV1;


    /*
     * Đếm lên
     */
    TIM_TimeBaseStructure.TIM_CounterMode =
        TIM_CounterMode_Up;


    /*
     * Repetition counter không sử dụng
     */
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;


    TIM_TimeBaseInit(
        TIM1,
        &TIM_TimeBaseStructure
    );


    /* =====================================================
     * CẤU HÌNH PWM
     * ===================================================== */

    /*
     * PWM Mode 1
     */
    TIM_OCInitStructure.TIM_OCMode =
        TIM_OCMode_PWM1;


    /*
     * Ban đầu disable output
     * Sẽ enable ở từng channel
     */
    TIM_OCInitStructure.TIM_OutputState =
        TIM_OutputState_Enable;


    /*
     * TIM1 là Advanced Timer nên
     * cần Main Output Enable.
     */
    TIM_OCInitStructure.TIM_OutputNState =
        TIM_OutputNState_Disable;


    /*
     * Polarity mức cao
     */
    TIM_OCInitStructure.TIM_OCPolarity =
        TIM_OCPolarity_High;


    /*
     * Polarity complementary không sử dụng
     */
    TIM_OCInitStructure.TIM_OCNPolarity =
        TIM_OCNPolarity_High;


    /*
     * Idle state
     */
    TIM_OCInitStructure.TIM_OCIdleState =
        TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_OCNIdleState =
        TIM_OCNIdleState_Reset;


    /* =====================================================
     * CHANNEL 1
     *
     * PA8
     *
     * Duty = 10%
     *
     * CCR1 = 100
     * ===================================================== */

    TIM_OCInitStructure.TIM_Pulse = 100;

    TIM_OC1Init(
        TIM1,
        &TIM_OCInitStructure
    );

    TIM_OC1PreloadConfig(
        TIM1,
        TIM_OCPreload_Enable
    );


    /* =====================================================
     * CHANNEL 2
     *
     * PA9
     *
     * Duty = 30%
     *
     * CCR2 = 300
     * ===================================================== */

    TIM_OCInitStructure.TIM_Pulse = 300;

    TIM_OC2Init(
        TIM1,
        &TIM_OCInitStructure
    );

    TIM_OC2PreloadConfig(
        TIM1,
        TIM_OCPreload_Enable
    );


    /* =====================================================
     * CHANNEL 3
     *
     * PA10
     *
     * Duty = 50%
     *
     * CCR3 = 500
     * ===================================================== */

    TIM_OCInitStructure.TIM_Pulse = 500;

    TIM_OC3Init(
        TIM1,
        &TIM_OCInitStructure
    );

    TIM_OC3PreloadConfig(
        TIM1,
        TIM_OCPreload_Enable
    );


    /* =====================================================
     * CHANNEL 4
     *
     * PA11
     *
     * Duty = 70%
     *
     * CCR4 = 700
     * ===================================================== */

    TIM_OCInitStructure.TIM_Pulse = 700;

    TIM_OC4Init(
        TIM1,
        &TIM_OCInitStructure
    );

    TIM_OC4PreloadConfig(
        TIM1,
        TIM_OCPreload_Enable
    );


    /*
     * Cho phép preload ARR
     */
    TIM_ARRPreloadConfig(
        TIM1,
        ENABLE
    );


    /*
     * TIM1 là Advanced Timer.
     *
     * Phải bật Main Output Enable
     * thì PWM mới xuất ra chân.
     */
    TIM_CtrlPWMOutputs(
        TIM1,
        ENABLE
    );


    /*
     * Bắt đầu Timer
     */
    TIM_Cmd(
        TIM1,
        ENABLE
    );
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    /*
     * Cấu hình chân PWM
     */
    PWM_GPIO_Config();


    /*
     * Cấu hình TIM1 PWM
     */
    TIM1_PWM_Config();


    /*
     * Không cần xử lý gì thêm.
     *
     * TIM1 tự động tạo PWM bằng phần cứng.
     */
    while (1)
    {
    }
}
