#include "stm32f10x.h"

/*
 * ==========================================
 * BÀI 2 - SYSTICK
 *
 * SysTick interrupt: 1 ms
 *
 * LED1: 0.1 Hz
 * LED2: 1 Hz
 * LED3: 10 Hz
 *
 * ==========================================
 */


/* ==========================================
 * GPIO LED
 *
 * LED1 -> PC13
 * LED2 -> PA1
 * LED3 -> PA2
 *
 * Nếu mạch của bạn dùng chân khác,
 * chỉ cần thay các chân này.
 * ==========================================
 */

#define LED1_PORT      GPIOC
#define LED1_PIN       GPIO_Pin_13

#define LED2_PORT      GPIOA
#define LED2_PIN       GPIO_Pin_1

#define LED3_PORT      GPIOA
#define LED3_PIN       GPIO_Pin_2


/* ==========================================
 * Bộ đếm thời gian
 * ==========================================
 */

volatile uint32_t led1_counter = 0;
volatile uint32_t led2_counter = 0;
volatile uint32_t led3_counter = 0;


/* ==========================================
 * CẤU HÌNH GPIO
 * ==========================================
 */

static void LED_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    /* Bật clock GPIOA và GPIOC */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_GPIOC,
        ENABLE
    );


    /* =========================
     * LED1 - PC13
     * =========================
     */

    GPIO_InitStructure.GPIO_Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(LED1_PORT, &GPIO_InitStructure);


    /* =========================
     * LED2 - PA1
     * =========================
     */

    GPIO_InitStructure.GPIO_Pin = LED2_PIN;

    GPIO_Init(LED2_PORT, &GPIO_InitStructure);


    /* =========================
     * LED3 - PA2
     * =========================
     */

    GPIO_InitStructure.GPIO_Pin = LED3_PIN;

    GPIO_Init(LED3_PORT, &GPIO_InitStructure);


    /* =========================
     * Trạng thái ban đầu
     *
     * PC13 trên Blue Pill thường
     * là LED active LOW.
     *
     * PA1, PA2 giả sử active HIGH.
     * =========================
     */

    GPIO_SetBits(LED1_PORT, LED1_PIN);

    GPIO_ResetBits(LED2_PORT, LED2_PIN);
    GPIO_ResetBits(LED3_PORT, LED3_PIN);
}


/* ==========================================
 * CẤU HÌNH SYSTICK
 *
 * SystemCoreClock = 72 MHz
 *
 * SysTick_Config(72000000 / 1000)
 *
 * = SysTick interrupt mỗi 1 ms
 * ==========================================
 */

static void SysTick_Config_1ms(void)
{
    /*
     * SystemCoreClock chứa tần số CPU.
     *
     * Với STM32F103:
     *
     * SystemCoreClock = 72 MHz
     *
     * 72,000,000 / 1000
     * = 72,000
     *
     * => ngắt mỗi 1 ms
     */

    SysTick_Config(SystemCoreClock / 1000);
}


/* ==========================================
 * NGẮT SYSTICK
 * ==========================================
 */

void SysTick_Handler(void)
{
    /* =====================================
     * LED1 = 0.1 Hz
     *
     * Chu kỳ = 10 giây
     *
     * Đảo trạng thái mỗi 5 giây
     *
     * 5000 lần ngắt × 1 ms
     * = 5000 ms
     * = 5 giây
     * =====================================
     */

    led1_counter++;

    if (led1_counter >= 5000)
    {
        led1_counter = 0;

        GPIO_WriteBit(
            LED1_PORT,
            LED1_PIN,
            (BitAction)(1 - GPIO_ReadOutputDataBit(
                LED1_PORT,
                LED1_PIN
            ))
        );
    }


    /* =====================================
     * LED2 = 1 Hz
     *
     * Chu kỳ = 1 giây
     *
     * Đảo trạng thái mỗi 500 ms
     * =====================================
     */

    led2_counter++;

    if (led2_counter >= 500)
    {
        led2_counter = 0;

        GPIO_WriteBit(
            LED2_PORT,
            LED2_PIN,
            (BitAction)(1 - GPIO_ReadOutputDataBit(
                LED2_PORT,
                LED2_PIN
            ))
        );
    }


    /* =====================================
     * LED3 = 10 Hz
     *
     * Chu kỳ = 100 ms
     *
     * Đảo trạng thái mỗi 50 ms
     * =====================================
     */

    led3_counter++;

    if (led3_counter >= 50)
    {
        led3_counter = 0;

        GPIO_WriteBit(
            LED3_PORT,
            LED3_PIN,
            (BitAction)(1 - GPIO_ReadOutputDataBit(
                LED3_PORT,
                LED3_PIN
            ))
        );
    }
}


/* ==========================================
 * MAIN
 * ==========================================
 */

int main(void)
{
    /* Cấu hình GPIO */
    LED_GPIO_Config();

    /* Cấu hình SysTick 1 ms */
    SysTick_Config_1ms();

    /*
     * Không cần xử lý gì trong main.
     *
     * LED được điều khiển hoàn toàn
     * bởi ngắt SysTick.
     */

    while (1)
    {
    }
}
