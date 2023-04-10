/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fonts.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CMD_SIZE 50
#define ARR_CNT 5

#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85

// Interrupt Pin for "Need check"
#define BUTTON_GPIO_PORT GPIO_PIN_5
#define BUTTON_GPIO_PIN GPIOB

// For HC sensor
#define TRIG_PIN GPIO_PIN_5
#define TRIG_PORT GPIOA

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
struct dht11 {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint8_t data[5];    /* data from sensor store here */
};


struct dht11 dhtInfo;
char snum[5];


char temp[100];
char temp2[100];
char ampm[2][3] = {"AM", "PM"};
int clockFlag=0 ;

char weather[100];
char weather2[100];
int weather_state = 0;
int button_state = 0;
int motor_state = 0;
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;
uint8_t Distance  = 0;

char sendData[50]={0};
char recvData[50]={0};
uint8_t RxBuffer[100];
char userData[50]={0};


extern char rx3Data[50];
extern char rx2Data;
extern volatile unsigned char rx2Flag;
extern volatile unsigned char rx3Flag;
extern volatile unsigned long systick_sec;            //1sec
extern volatile int systick_secFlag;
extern volatile char uart2_rxdata[5][100];

extern volatile unsigned long systick_count;
extern volatile char rx2Buffer[50];
extern volatile unsigned char bufferFlage;
extern volatile unsigned char completeFlag;

int value = 10; //for motor
volatile unsigned long pre_sec;

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;



char snum[5];

void DHT11_Init(struct dht11 dh);
uint8_t DHT_Read(struct dht11 *dht);
void Delay(const uint32_t Count);
void Delay_us(const uint32_t Count);

int set_min;
int set_sec;
int shift;

int bcd_set_min = 0;
int bcd_set_sec = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	(void)HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 100);
	return ch;
}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	while (__HAL_TIM_GET_COUNTER (&htim3) < time);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
	{
		if (Is_First_Captured==0) // if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (Is_First_Captured==1)   // if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}

			Distance = Difference * .034/2;
			Is_First_Captured = 0; // set it back to false

			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC1);
		}
	}
}

void HCSR04_Read (void)
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1);
}


//Exti Interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BUTTON_GPIO_PORT)
		button_state = 1;
	else if(GPIO_Pin == GPIO_PIN_15)
		weather_state = 1;
	else if (GPIO_Pin == GPIO_PIN_10)
		motor_state = 1;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int i;
	dhtInfo.GPIOx = GPIOC;
	dhtInfo.GPIO_Pin = GPIO_PIN_3;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_UART5_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

	printf("start main()\r\n");
	// servo control
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	//for wifi
//	WIFI_init();
//	sprintf(sendData,"[""food"":""PASSWD""]");
//	WIFI_send(sendData);

	bufferFlage = 1;


	// for HC sensor
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

	// for DH sensor
	DHT11_Init(dhtInfo);

	// for oled
	SSD1306_Init();



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint32_t clock_time = HAL_GetTick();
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD); // GetTime to sTime in BCD format
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD); // GetDate to sDate in BCD format

	motor_control(10);
	while (1)
	{
		if (HAL_GetTick() - clock_time >= 1000) {
			clock_time = HAL_GetTick();
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD); // GetTime to sTime in BCD format
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD); // GetDate to sDate in BCD format
			current_time();
		}

		recvData[0] = 0;
		if (completeFlag) {
			completeFlag = 0;
			if (!strncmp((char*) rx2Buffer, "+IPD", 4)) {
				strcpy(recvData, (char*) (rx2Buffer + 8));
				recvData[strlen((char*) (rx2Buffer + 8))] = 0;
				printf("%s\n\r", recvData);
			}
		}
		if (recvData[0] != 0) {
			Serial2_Event();
			Alarm_Event();
			recvData[0] = 0;
		}

//		bcd_set_min = DecimalToBCD(set_min);
//		bcd_set_sec = DecimalToBCD(set_sec);
//		printf("setting : %02x,%02x\r\n", bcd_set_min, bcd_set_sec);
//		HAL_Delay(1000);

		for (int shift = 0; set_min > 0; shift++, set_min /= 10) {
			bcd_set_min |= set_min % 10 << (shift << 2);
		}
		for (int shift = 0; set_sec > 0; shift++, set_sec /= 10) {
			bcd_set_sec |= set_sec % 10 << (shift << 2);
		}

		if ((sTime.Hours == bcd_set_min) && (sTime.Minutes == bcd_set_sec)) {
			printf("Alarm\r\n");
			feed_one_time();
		}

		// for clock

		HCSR04_Read();
		//printf("%d cm\r\n",Distance);
		HAL_Delay(200);

		Oled_Event();
		if ((button_state == 1) && (Distance >= 7)) {
			Oled_Exit_Event();
		}
		if (weather_state == 1) {
			Weather_Event();
			printf("weather\r\n");
		}

		if (motor_state == 1) {
			feed_one_time();
			printf("motor\r\n");
			motor_state = 0;
		}


	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//void DecimalToBCD(int a)
//{
//
//	for (int shift = 0; a > 0; shift++, a /= 10)
//	{
//		bcd_pArray |= a % 10 << (shift << 2);
//	}
//
//	return bcd_pArray;
//}


void Serial2_Event()
{
	int i=0;
	int num = 0;
	char * pToken;
	char * pArray[ARR_CNT]={0};
	char recvBuf[CMD_SIZE]={0};

	strcpy(recvBuf,recvData);
	i=0;
	Serial3_Send_String(recvBuf);
	Serial3_Send_String("\n\r");

	printf("%s\r\n",recvBuf);

	Serial2_Send_String(recvBuf);
	Serial2_Send_String("\r\n");


	pToken = strtok(recvBuf,"[@]");

	while(pToken != NULL)
	{
		pArray[i] =  pToken;
		if(++i >= ARR_CNT)
			break;
		pToken = strtok(NULL,"[@]");
	}


	// user control from APP
	if (!strcmp(pArray[1], "set_time"))
	{
		if (pArray[3] != NULL)
			num = atoi(pArray[3]);
		else
			num = 0;
		// motor control
		printf(value);

		Serial2_Send_String("ok"); // 배급?�� ?��료되?��?��?�� ?���??????????????????????
		Serial2_Send_String("\r\n");
	}
	else if (!strncmp(pArray[1], " New conn", 8)) {
		return;
	}
	else if (!strncmp(pArray[1], " Already log", 8)) {
		connectWiFi();
		sprintf(sendData, "[" "6" ":" "PASSWD" "]");
		WIFI_send(sendData);
		return;
	}else
		return;

	sprintf(sendData,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
	WIFI_send(sendData);
}

void Alarm_Event()
{
	int i=0;
	int num = 0;
	char * pToken;
	char * pArray[ARR_CNT]={0};
	char recvBuf[CMD_SIZE]={0};

	strcpy(recvBuf,recvData);
	i=0;
	Serial3_Send_String(recvBuf);
	Serial3_Send_String("\n\r");

	printf("%s\r\n",recvBuf);

	Serial2_Send_String(recvBuf);
	Serial2_Send_String("\r\n");


	pToken = strtok(recvBuf,"[@]");

	while(pToken != NULL)
	{
		pArray[i] =  pToken;
		if(++i >= ARR_CNT)
			break;
		pToken = strtok(NULL,"[@]");
	}

	printf("%s\r\n", pArray[1]);
	if (!strcmp(pArray[1], "set")) {
		if (pArray[3] != NULL)
			num = atoi(pArray[3]);
		else
			num = 0;
		printf("%s\r\n", pArray[2]);
		printf("%s\r\n", pArray[3]);
		set_min = atoi(pArray[2]); // to int type
		set_sec = atoi(pArray[3]); // to int type

	} else if (!strncmp(pArray[1], " New conn", 8)) {
		return;
	}
	else if (!strncmp(pArray[1], " Already log", 8)) {
		connectWiFi();
		sprintf(sendData, "[" "6" ":" "PASSWD" "]");
		WIFI_send(sendData);
		return;
	}else
		return;
}


void motor_control(uint8_t value)
{

	if(value > 130)
		TIM2->CCR1 = 130;
	else if (value >= 10)
		TIM2->CCR1 = value ;
	else if ( value <10)
		TIM2->CCR1 = 10 ;

}
void feed_one_time()
{
	static uint8_t motor_control_value = 10;
	motor_control_value += 30;
	if (motor_control_value < 10)
		motor_control_value = 10;
	if (motor_control_value > 130)
		motor_control_value = 130;
	motor_control(motor_control_value);
}
void Weather_Event()
{
	if (DHT_Read(&dhtInfo))
	{
		sprintf(weather, "Temp : %d.%d ", dhtInfo.data[0], dhtInfo.data[1]);
		sprintf(weather2, "Humi : %d.%d", dhtInfo.data[2], dhtInfo.data[3]);

		SSD1306_Clear();
		SSD1306_GotoXY(3, 10);
		SSD1306_Puts(" Today's weather", &Font_7x10, 1);
		SSD1306_GotoXY(20, 30);
		SSD1306_Puts(weather, &Font_7x10, 1);
		SSD1306_GotoXY(20, 50);
		SSD1306_Puts(weather2, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		HAL_Delay(2000);
		SSD1306_Clear();
	}
	weather_state=0;
}

void current_time()
{
	sprintf(temp, "20%02x-%02x-%02x", sDate.Year, sDate.Month, sDate.Date);
	sprintf(temp2, "%s %02x:%02x:%02x", ampm[sTime.TimeFormat], sTime.Hours,sTime.Minutes, sTime.Seconds);
	//	HAL_UART_Transmit(&huart3, (uint8_t*) temp, strlen(temp), 10);
	//	HAL_UART_Transmit(&huart3, (uint8_t*) temp2, strlen(temp), 10);

	SSD1306_GotoXY(3, 10);
	SSD1306_Puts("  Current Time", &Font_7x10, 1);
	SSD1306_GotoXY(20, 30);
	SSD1306_Puts(temp, &Font_7x10, 1);
	SSD1306_GotoXY(20, 50);
	SSD1306_Puts(temp2, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}

void Oled_Event()
{
	if (Distance < 7) {
		clockFlag = 1;
		if ((Distance < 7) && (clockFlag = 1)) {
			clockFlag =0;
			value = 40;
			TIM2->CCR1 = value;
			SSD1306_Clear();
			SSD1306_GotoXY(0, 12);
			SSD1306_Puts("   FEED  ", &Font_11x18, 1);
			SSD1306_GotoXY(0, 30);
			SSD1306_Puts("   TIME  ", &Font_11x18, 1);
			SSD1306_UpdateScreen();
			HAL_Delay(2000);
			value = 80;
			TIM2->CCR1 = value;
			SSD1306_Clear();
		}
	}
}

void Oled_Exit_Event()
{
	pre_sec = systick_sec;
	if (systick_sec - pre_sec <= 4)
	{
		button_state = 0;
		SSD1306_Clear();
		SSD1306_GotoXY(3, 12);
		SSD1306_Puts("   Need  ", &Font_11x18, 1);
		SSD1306_GotoXY(0, 30);
		SSD1306_Puts("   Check  ", &Font_11x18, 1);
		SSD1306_UpdateScreen();
		HAL_Delay(2000);
		SSD1306_Clear();
		sprintf(userData,"[udb]food@%d\n",5);
		send_wifi_simple(userData);

	} else if (systick_sec - pre_sec > 4) {
		pre_sec = systick_sec;
	}
}

// For DHT sensor
void DHT11_Init(struct dht11 dh)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = dh.GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(dh.GPIOx, &GPIO_InitStruct);
}

uint8_t DHT_Read(struct dht11 *dht)
{
	uint8_t tmp;
	uint8_t sum = 0;
	uint8_t j = 0, i;
	uint8_t last_state = 1;
	uint16_t counter = 0;
	/*
	 * Pull the pin 1 and wait 250 milliseconds
	 */

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = dht->GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct);
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, 1);
	Delay(250);

	//   dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;
	//data배열 0?���???????? 초기?��
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, 0);
	Delay(20);
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, 1);
	Delay_us(30);                   					//40 ?��?�� ?��컨드 ?��?��?��
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pin = dht->GPIO_Pin;
	HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct);

	/* Read the timings */
	for (i = 0; i < DHT_MAXTIMINGS; i++) {             //DHT_MAXTIMINGS = 85
		counter = 0;
		while (1) {
			tmp = HAL_GPIO_ReadPin(dht->GPIOx,dht->GPIO_Pin);
			Delay_us(3);

			if (tmp != last_state)    //last state = 1
				break;

			counter++;
			Delay_us(1);

			if (counter == 255)  //85 times
				break;
		}

		last_state = HAL_GPIO_ReadPin(dht->GPIOx,dht->GPIO_Pin);  //0000 0x00

		if (counter == 255)  //85 times
			break;

		/* Ignore first 3 transitions */
		if ((i >= 4) && (i % 2 == 0)) {
			/* Shove each bit into the storage bytes */
			dht->data[j/8] <<= 1;
			if (counter > DHT_COUNT)  //DHT_COUNT 6
				dht->data[j/8] |= 1;
			j++;
		}
	}

	sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];
	//sum = check sum

	if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))  //data[4]?? check sum
		return 1;
	return 0;
}

void Delay(const uint32_t Count)
{
	__IO uint32_t index = 0;
	for(index = (16800 * Count); index != 0; index--);
}

void Delay_us(const uint32_t Count)
{
	__IO uint32_t index = 0;
	for(index = (13 * Count); index != 0; index--);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
