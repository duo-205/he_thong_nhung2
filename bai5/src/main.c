#include <stdlib.h>
#include "stm32f10x.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>


/* =========================================================
 * CẤU HÌNH
 * ========================================================= */

/* PWM:
 * TIM3_CH1 -> PA6
 *
 * Timer clock = 72 MHz
 * PSC = 71  -> 1 MHz
 * ARR = 999 -> 1 kHz PWM
 */

#define PWM_PERIOD     999

/* UART command buffer */
#define CMD_BUFFER_SIZE 64


/* =========================================================
 * BIẾN TOÀN CỤC
 * ========================================================= */

/* Buffer nhận UART */
volatile char uart_buffer[CMD_BUFFER_SIZE];
volatile uint8_t uart_index = 0;

/* Trạng thái LED */
volatile uint8_t led_state = 0;
/*
 * 0 = OFF
 * 1 = ON
 */

/* PWM được cấu hình gần nhất */
volatile uint8_t pwm_percent = 50;


/* =========================================================
 * DELAY
 * ========================================================= */

static void Delay_ms(volatile uint32_t ms)
{
    volatile uint32_t i;

    while (ms--)
    {
        for (i = 0; i < 7200; i++)
        {
            __asm volatile ("nop");
        }
    }
}


/* =========================================================
 * UART
 * ========================================================= */

static void UART1_SendChar(char c)
{
    USART_SendData(USART1, (uint16_t)c);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}


static void UART1_SendString(const char *str)
{
    while (*str)
    {
        UART1_SendChar(*str++);
    }
}


/* =========================================================
 * PWM
 * ========================================================= */

static void PWM_SetDuty(uint8_t percent)
{
    uint16_t compare;

    if (percent > 100)
    {
        percent = 100;
    }

    compare = ((uint32_t)percent * (PWM_PERIOD + 1)) / 100;

    if (compare > PWM_PERIOD)
    {
        compare = PWM_PERIOD;
    }

    TIM_SetCompare1(TIM3, compare);
}


/* =========================================================
 * PWM CONFIG
 * ========================================================= */

static void PWM_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* Enable GPIOA + AFIO + TIM3 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_AFIO,
        ENABLE
    );

    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_TIM3,
        ENABLE
    );


    /* -----------------------------------------------------
     * PA6 = TIM3_CH1
     * Alternate Function Push-Pull
     * ----------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* -----------------------------------------------------
     * TIM3
     *
     * Timer clock = 72 MHz
     *
     * 72 MHz / (71 + 1)
     * = 1 MHz
     *
     * 1 MHz / (999 + 1)
     * = 1 kHz
     * ----------------------------------------------------- */

    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);


    /* -----------------------------------------------------
     * PWM Channel 1
     * ----------------------------------------------------- */

    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);


    /* Bắt đầu OFF */
    TIM_SetCompare1(TIM3, 0);

    TIM_Cmd(TIM3, ENABLE);
}


/* =========================================================
 * UART CONFIG
 * ========================================================= */

static void UART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    /* Enable GPIOA + USART1 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );


    /* -----------------------------------------------------
     * PA9 = USART1_TX
     * ----------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* -----------------------------------------------------
     * PA10 = USART1_RX
     * ----------------------------------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* -----------------------------------------------------
     * USART1
     * ----------------------------------------------------- */

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Rx |
        USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);


    /* -----------------------------------------------------
     * USART1 RX interrupt
     * ----------------------------------------------------- */

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );


    /* -----------------------------------------------------
     * NVIC USART1
     * ----------------------------------------------------- */

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);


    USART_Cmd(USART1, ENABLE);
}


/* =========================================================
 * XỬ LÝ LỆNH
 * ========================================================= */

static void Process_Command(char *cmd)
{
    char response[80];

    /* -----------------------------------------------------
     * ON
     * ----------------------------------------------------- */

    if (strcmp(cmd, "ON") == 0)
    {
        led_state = 1;

        /* Khôi phục PWM gần nhất */
        PWM_SetDuty(pwm_percent);

        UART1_SendString("LED ON\r\n");
    }


    /* -----------------------------------------------------
     * OFF
     * ----------------------------------------------------- */

    else if (strcmp(cmd, "OFF") == 0)
    {
        led_state = 0;

        /* Tắt PWM thực tế */
        PWM_SetDuty(0);

        UART1_SendString("LED OFF\r\n");
    }


    /* -----------------------------------------------------
     * STATUS
     * ----------------------------------------------------- */

    else if (strcmp(cmd, "Status") == 0 ||
             strcmp(cmd, "STATUS") == 0 ||
             strcmp(cmd, "status") == 0)
    {
        sprintf(
            response,
            "LED=%s, PWM=%d%%\r\n",
            led_state ? "ON" : "OFF",
            pwm_percent
        );

        UART1_SendString(response);
    }


    /* -----------------------------------------------------
     * PWM:Percent%
     *
     * Ví dụ:
     *
     * PWM:10%
     * PWM:50%
     * PWM:100%
     * ----------------------------------------------------- */

    else if (strncmp(cmd, "PWM:", 4) == 0)
    {
        char *value_string;
        int value;

        value_string = cmd + 4;

        value = atoi(value_string);

        if (value >= 0 && value <= 100)
        {
            /*
             * Luôn lưu mức PWM mới
             */
            pwm_percent = (uint8_t)value;

            /*
             * Chỉ thay đổi PWM thực tế
             * nếu LED đang ON.
             */
            if (led_state)
            {
                PWM_SetDuty(pwm_percent);
            }

            sprintf(
                response,
                "PWM CONFIG=%d%%\r\n",
                pwm_percent
            );

            UART1_SendString(response);
        }
        else
        {
            UART1_SendString(
                "ERROR: PWM must be 0-100%\r\n"
            );
        }
    }


    /* -----------------------------------------------------
     * LỆNH KHÔNG HỢP LỆ
     * ----------------------------------------------------- */

    else
    {
        UART1_SendString("ERROR: Unknown command\r\n");
    }
}


/* =========================================================
 * USART1 INTERRUPT HANDLER
 * ========================================================= */

void USART1_IRQHandler(void)
{
    char received_char;

    if (USART_GetITStatus(
            USART1,
            USART_IT_RXNE
        ) != RESET)
    {
        received_char =
            (char)USART_ReceiveData(USART1);


        /* -------------------------------------------------
         * Khi nhận dấu !
         * -> kết thúc một lệnh
         * ------------------------------------------------- */

        if (received_char == '!')
        {
            uart_buffer[uart_index] = '\0';

            Process_Command((char *)uart_buffer);

            /* Reset buffer */
            uart_index = 0;
        }

        else
        {
            /* -------------------------------------------------
             * Không lưu CR/LF
             * ------------------------------------------------- */

            if (received_char != '\r' &&
                received_char != '\n')
            {
                if (uart_index < CMD_BUFFER_SIZE - 1)
                {
                    uart_buffer[uart_index++] =
                        received_char;
                }
                else
                {
                    /* Buffer overflow */
                    uart_index = 0;

                    UART1_SendString(
                        "ERROR: Command too long\r\n"
                    );
                }
            }
        }


        USART_ClearITPendingBit(
            USART1,
            USART_IT_RXNE
        );
    }
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    /* PWM */
    PWM_Config();

    /* UART */
    UART1_Config();


    UART1_SendString(
        "\r\nSTM32F103 PWM UART READY\r\n"
    );

    UART1_SendString(
        "Commands: ON! OFF! PWM:Percent%! Status!\r\n"
    );

    UART1_SendString(
        "Initial state: OFF, PWM=50%\r\n"
    );


    while (1)
    {
        /*
         * UART được xử lý hoàn toàn bằng ngắt.
         *
         * Không cần polling UART ở đây.
         */
    }
}
