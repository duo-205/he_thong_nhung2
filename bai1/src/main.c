#include "stm32f10x.h"

/* =========================
 * CẤU HÌNH
 * ========================= */

#define RX_BUFFER_SIZE 128

/* Thay 2 dòng này bằng mã lớp và mã nhóm của bạn */
#define MA_LOP  "D23CQDT04"
#define MA_NHOM "07"

/* =========================
 * BIẾN TOÀN CỤC
 * ========================= */

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_index = 0;


/* =========================
 * UART1 GỬI 1 KÝ TỰ
 * ========================= */

static void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(USART1, (uint16_t)c);
}


/* =========================
 * UART1 GỬI CHUỖI
 * ========================= */

static void USART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}


/* =========================
 * CẤU HÌNH UART1
 *
 * PA9  -> TX
 * PA10 -> RX
 * Baudrate: 115200
 * Data: 8 bit
 * Stop: 1 bit
 * Parity: None
 * ========================= */

static void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Bật clock GPIOA và USART1 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* -------------------------
     * PA9 = USART1_TX
     * Alternate Function Push-Pull
     * ------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* -------------------------
     * PA10 = USART1_RX
     * Input Floating
     * ------------------------- */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* -------------------------
     * Cấu hình USART1
     * ------------------------- */

    USART_InitStructure.USART_BaudRate = 115200;

    USART_InitStructure.USART_WordLength =
        USART_WordLength_8b;

    USART_InitStructure.USART_StopBits =
        USART_StopBits_1;

    USART_InitStructure.USART_Parity =
        USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);


    /* -------------------------
     * Cho phép ngắt RX
     * ------------------------- */

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );


    /* -------------------------
     * Cấu hình NVIC
     * ------------------------- */

    NVIC_InitStructure.NVIC_IRQChannel =
        USART1_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStructure.NVIC_IRQChannelCmd =
        ENABLE;

    NVIC_Init(&NVIC_InitStructure);


    /* Bật USART1 */
    USART_Cmd(USART1, ENABLE);
}


/* =========================
 * NGẮT USART1
 * ========================= */

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char c;

        /* Đọc dữ liệu nhận được */
        c = (char)USART_ReceiveData(USART1);

        /* -------------------------
         * Nếu nhận được !
         * ------------------------- */

        if (c == '!')
        {
            /* Kết thúc chuỗi */
            rx_buffer[rx_index] = '\0';

            /* -------------------------
             * Gửi:
             * <Mã lớp><Mã nhóm>: <bản tin>
             * ------------------------- */

            USART1_SendString(MA_LOP);
            USART1_SendString(MA_NHOM);
            USART1_SendString(": ");
            USART1_SendString((const char *)rx_buffer);
            USART1_SendString("\n\r");

            /* Xóa buffer để nhận bản tin mới */
            rx_index = 0;
        }

        /* -------------------------
         * Nếu chưa gặp !
         * lưu vào buffer
         * ------------------------- */

        else
        {
            if (rx_index < RX_BUFFER_SIZE - 1)
            {
                rx_buffer[rx_index] = c;
                rx_index++;
            }
        }
    }
}


/* =========================
 * MAIN
 * ========================= */

int main(void)
{
    USART1_Config();

    while (1)
    {
    }
}
