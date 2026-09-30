#include "stm32f10x.h"

/* =========================================================
 * CẤU HÌNH
 * ========================================================= */

/*
 * ADC1_IN0
 * PA0 -> chân giữa của biến trở WH148
 *
 * Hai chân ngoài của biến trở:
 *
 * Một chân -> 3.3V
 * Một chân -> GND
 *
 * Chân giữa -> PA0
 */


/* =========================================================
 * UART1
 *
 * PA9  -> TX
 * PA10 -> RX
 *
 * 115200 8N1
 * ========================================================= */


/* =========================================================
 * HÀM GỬI 1 KÝ TỰ QUA UART
 * ========================================================= */

static void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(USART1, (uint16_t)c);
}


/* =========================================================
 * HÀM GỬI CHUỖI QUA UART
 * ========================================================= */

static void USART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}


/* =========================================================
 * HÀM GỬI SỐ NGUYÊN
 * ========================================================= */

static void USART1_SendUInt(uint32_t number)
{
    char buffer[10];
    uint8_t i = 0;

    if (number == 0)
    {
        USART1_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    while (i > 0)
    {
        i--;
        USART1_SendChar(buffer[i]);
    }
}


/* =========================================================
 * CẤU HÌNH UART1
 * ========================================================= */

static void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /*
     * Bật clock GPIOA
     * Bật clock USART1
     */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );


    /*
     * PA9 = USART1_TX
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /*
     * PA10 = USART1_RX
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /*
     * Cấu hình USART
     */
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
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);


    /*
     * Bật USART1
     */
    USART_Cmd(USART1, ENABLE);
}


/* =========================================================
 * CẤU HÌNH ADC1
 *
 * PA0 = ADC1_IN0
 * ========================================================= */

static void ADC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;


    /*
     * Bật clock GPIOA
     */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA,
        ENABLE
    );


    /*
     * Bật clock ADC1
     */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_ADC1,
        ENABLE
    );


    /*
     * ADC clock:
     *
     * PCLK2 = 72 MHz
     *
     * ADC clock = 72 / 6 = 12 MHz
     *
     * Không được vượt quá giới hạn ADC của STM32F103.
     */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);


    /*
     * PA0 = Analog Input
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /*
     * Cấu hình ADC1
     */
    ADC_InitStructure.ADC_Mode =
        ADC_Mode_Independent;

    ADC_InitStructure.ADC_ScanConvMode =
        DISABLE;

    ADC_InitStructure.ADC_ContinuousConvMode =
        DISABLE;

    ADC_InitStructure.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;

    ADC_InitStructure.ADC_DataAlign =
        ADC_DataAlign_Right;

    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &ADC_InitStructure);


    /*
     * Cấu hình kênh ADC1_IN0
     *
     * Sample time = 55.5 cycles
     */
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_55Cycles5
    );


    /*
     * Bật ADC1
     */
    ADC_Cmd(ADC1, ENABLE);


    /*
     * Reset calibration
     */
    ADC_ResetCalibration(ADC1);

    while (ADC_GetResetCalibrationStatus(ADC1))
    {
    }


    /*
     * Calibration
     */
    ADC_StartCalibration(ADC1);

    while (ADC_GetCalibrationStatus(ADC1))
    {
    }
}


/* =========================================================
 * ĐỌC ADC
 * ========================================================= */

static uint16_t ADC1_Read(void)
{
    /*
     * Bắt đầu chuyển đổi
     */
    ADC_SoftwareStartConvCmd(
        ADC1,
        ENABLE
    );


    /*
     * Chờ chuyển đổi hoàn thành
     */
    while (ADC_GetFlagStatus(
               ADC1,
               ADC_FLAG_EOC) == RESET)
    {
    }


    /*
     * Đọc kết quả
     *
     * 0 -> 4095
     */
    return ADC_GetConversionValue(ADC1);
}


/* =========================================================
 * DELAY
 *
 * Delay đơn giản dùng vòng lặp.
 *
 * Không dùng cho thời gian chính xác.
 * Chỉ dùng để gửi dữ liệu khoảng mỗi 1 giây.
 * ========================================================= */

static void Delay_ms(uint32_t ms)
{
    uint32_t i;

    while (ms--)
    {
        /*
         * Với CPU 72 MHz,
         * giá trị này chỉ mang tính gần đúng.
         */
        for (i = 0; i < 7200; i++)
        {
            __NOP();
        }
    }
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    uint16_t adc_value;

    uint32_t voltage_mV;


    /*
     * Cấu hình UART
     */
    USART1_Config();


    /*
     * Cấu hình ADC
     */
    ADC1_Config();


    /*
     * Thông báo khởi động
     */
    USART1_SendString(
        "STM32 ADC START\r\n"
    );


    while (1)
    {
        /*
         * Đọc ADC
         */
        adc_value = ADC1_Read();


        /*
         * Tính điện áp theo mV
         *
         * V = ADC / 4095 * 3.3
         *
         * Để tránh số thực:
         *
         * mV = ADC * 3300 / 4095
         */
        voltage_mV =
            ((uint32_t)adc_value * 3300) / 4095;


        /*
         * Gửi ADC
         */
        USART1_SendString("ADC = ");

        USART1_SendUInt(adc_value);


        /*
         * Gửi điện áp
         */
        USART1_SendString(
            " | Voltage = "
        );


        /*
         * Phần nguyên
         */
        USART1_SendUInt(
            voltage_mV / 1000
        );


        USART1_SendChar('.');


        /*
         * Ba chữ số phần thập phân
         */
        if ((voltage_mV % 1000) < 100)
        {
            USART1_SendChar('0');
        }

        if ((voltage_mV % 1000) < 10)
        {
            USART1_SendChar('0');
        }

        USART1_SendUInt(
            voltage_mV % 1000
        );


        USART1_SendString(
            " V\r\n"
        );


        /*
         * Khoảng 1 giây đọc/gửi một lần
         */
        Delay_ms(1000);
    }
}
