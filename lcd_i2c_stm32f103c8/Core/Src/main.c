/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include "buzz.h"
#include <stdio.h>
#include "joystick.h"
#include "finger.h"
#include "string.h"
#include "ds1307_for_stm32_hal.h"
#include "stdlib.h"
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char cc1[] = {0x00, 0x00, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00};  // smiley
char cc2[] = {0x0E, 0x0E, 0x04, 0x0E, 0x15, 0x04, 0x0A, 0x0A};  // nguoi
char cc3[] = {0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08, 0x00};  // arrow
char cc4[] = {0x00, 0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x04, 0x00};  // bell
char cc5[] = {0x00, 0x00, 0x0A, 0x15, 0x11, 0x0E, 0x04, 0x00};  // Heart
char cc6[] = {0x00, 0x0E, 0x11, 0x11, 0x11, 0x0A, 0x1B, 0x00};  // omega
char cc7[] = {0x0E, 0x10, 0x17, 0x12, 0x12, 0x12, 0x10, 0x0E};  // CT
char cc8[] = {0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x1F, 0x00};  // +-

char functions[5][20] = {
    "CHAM CONG",
    "THEM VAN TAY",
    "XOA TOAN BO",
		"XEM GIO",
		"KHOI TAO"
};
char* datetime_str = "";

int IDFunc = 0;
int IDFinger =0;
int pID;
char fileContent[1000] = "";  

FATFS fs;
FIL fil;

typedef struct {
  int id;
  char ten[50];
} Employee;
Employee *nv;

typedef struct {
  Employee emloyee;
	bool isGioVao;
  char giovao[20];
	char giora[20];
} Job;
Job* job;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void xemgio(void)
{
	while(1)
	{
		if(get_joystick_position() ==0)
		{
			// Hi?n th? ch?c nang hi?n t?i lên LCD
				sendlcd(functions[IDFunc]);
         datetime_str= get_current_date_time(&hi2c2);
        lcd_put_cur(1,0);
				lcd_send_string(datetime_str);
        // Gi?i phóng b? nh? dã c?p phát cho chu?i th?i gian
        free(datetime_str);
				HAL_Delay(1000);
		}

		else
		{
			beep(500,1);
			break;
		}
			
	}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
void writesdcard(char* filename, char* data)
{
    FRESULT res;
    UINT bytes_written;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        sendlcd("Mount error");
        HAL_Delay(1000);
        return;
    }

    res = f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        sendlcd("Open error");
        HAL_Delay(1000);
        return;
    }

    f_lseek(&fil, fil.fsize);
    res = f_write(&fil, data, strlen(data), &bytes_written);
    if (res != FR_OK) {
        sendlcd("Write error");
        HAL_Delay(1000);
        return;
    }

    res = f_close(&fil);
    if (res != FR_OK) {
        sendlcd("Close error");
        HAL_Delay(1000);
        return;
    }

    sendlcd("Write succeeded");
    HAL_Delay(1000);
}


void readsdcard(const char* fileName)
{
  FIL fil;
  FRESULT res;
  char buffer[100];
// Tang kích thu?c fileContent d? có th? luu du?c nhi?u hon

  /* Mount SD Card */
  if(f_mount(&fs, "", 1) != FR_OK)
  {
    sendlcd("SD Card Mount Failed");
    HAL_Delay(1000);
    return;
  }

  /* Open file to read */
  res = f_open(&fil, fileName, FA_READ);
  if (res != FR_OK)
  {
    sendlcd("Open error");
    HAL_Delay(1000);
    f_close(&fil);
    return;
  }

  /* Reads line by line until the end */
  while(f_gets(buffer, sizeof(buffer), &fil))
  {
    if (f_error(&fil)) {
      sendlcd("Read error");
      HAL_Delay(1000);
      f_close(&fil);
      return;
    }

    /* Concatenate file content to fileContent */
    strcat(fileContent, buffer);
  }

  /* Close file */
  res = f_close(&fil);
  if (res != FR_OK)
  {
    sendlcd("Close error");
    HAL_Delay(1000);
    return;
  }

  /* Unmount SD Card */
  f_mount(NULL, "", 1);
  sendlcd(fileContent);
  HAL_Delay(1500);
}

void writeEmployeeToFile(Employee* emp, const char* filename) {
    FIL fp;
    FRESULT res;
    UINT bytes_written;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return;
    }

    res = f_open(&fp, filename, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        return;
    }

    f_lseek(&fp, f_size(&fp));
    res = f_write(&fp, emp, sizeof(Employee), &bytes_written);
    if (res != FR_OK) {
        return;
    }
		
		res = f_sync(&fp);
    if (res != FR_OK) {
        return;
    }

    res = f_close(&fp);
    if (res != FR_OK) {
        return;
    }
}

Employee* readEmployeeFromFile(const char* filename, int id) {
    FIL fp;
    FRESULT res;
    UINT bytes_read;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return NULL;
    }

    res = f_open(&fp, filename, FA_READ);
    if (res != FR_OK) {
        return NULL;
    }

    Employee* emp = (Employee*) malloc(sizeof(Employee));
    while (f_read(&fp, emp, sizeof(Employee), &bytes_read) == FR_OK && bytes_read == sizeof(Employee)) {
        if (emp->id == id) {
            f_close(&fp);
            return emp;
        }
    }

    f_close(&fp);
    free(emp);
    return NULL;
}

void updateEmployeeInFile(Employee* emp, const char* filename) {
    FIL fp;
    FRESULT res;
    UINT bytes_written;
    UINT bytes_read;
    Employee* cur_emp;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return;
    }

    res = f_open(&fp, filename, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    if (res != FR_OK) {
        return;
    }

    while (f_read(&fp, cur_emp, sizeof(Employee), &bytes_read) == FR_OK && bytes_read == sizeof(Employee)) {
        if (cur_emp->id == emp->id) {
            f_lseek(&fp, fp.fptr - sizeof(Employee)); // Di chuy?n con tr? d?n d?u dòng c?a Employee c?n c?p nh?t
            res = f_write(&fp, emp, sizeof(Employee), &bytes_written); // Ghi dè thông tin Employee
            if (res != FR_OK) {
                return;
            }
            break; // Ðã c?p nh?t xong, thoát kh?i vòng l?p while
        }
    }

    res = f_close(&fp);
    if (res != FR_OK) {
        return;
    }
}

int countEmployees(const char* filename) {
    FIL fp;
    FRESULT res;
    UINT bytes_read;
    int count = 0;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return -1;
    }

    res = f_open(&fp, filename, FA_READ);
    if (res != FR_OK) {
        return -1;
    }

    Employee* emp = (Employee*) malloc(sizeof(Employee));
    while (f_read(&fp, emp, sizeof(Employee), &bytes_read) == FR_OK && bytes_read == sizeof(Employee)) {
        count++;
    }

    f_close(&fp);
    free(emp);
    return count;
}


void writeJobToFile(Job* job, const char* filename) {
    FIL fp;
    FRESULT res;
    UINT bytes_written;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return;
    }

    res = f_open(&fp, filename, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        return;
    }

    f_lseek(&fp, f_size(&fp));
    res = f_write(&fp, job, sizeof(Job), &bytes_written);
    if (res != FR_OK) {
        return;
    }

    res = f_close(&fp);
    if (res != FR_OK) {
        return;
    }
}


Job* readJobFromFile(const char* filename, int id) {
    FIL fp;
    FRESULT res;
    UINT bytes_read;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return NULL;
    }

    res = f_open(&fp, filename, FA_READ);
    if (res != FR_OK) {
        return NULL;
    }

    Job* job = (Job*) malloc(sizeof(Job));
    while (f_read(&fp, job, sizeof(Job), &bytes_read) == FR_OK && bytes_read == sizeof(Job)) {
        if (job->emloyee.id==id) {
            f_close(&fp);
            return job;
        }
    }

    f_close(&fp);
    free(job);
    return NULL;
}

void updateJobInFile(Job* job, const char* filename) {
    FIL fp;
    FRESULT res;
    UINT bytes_written;
    UINT bytes_read;
    Job* cur_job;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return;
    }

    res = f_open(&fp, filename, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    if (res != FR_OK) {
        return;
    }

    while (f_read(&fp, cur_job, sizeof(Job), &bytes_read) == FR_OK && bytes_read == sizeof(Job)) {
        if (cur_job->emloyee.id == job->emloyee.id) {
            f_lseek(&fp, fp.fptr - sizeof(Job)); // Di chuy?n con tr? d?n d?u dòng c?a Job c?n c?p nh?t
            res = f_write(&fp, job, sizeof(Job), &bytes_written); // Ghi dè thông tin Job
            if (res != FR_OK) {
                return;
            }
            break; // Ðã c?p nh?t xong, thoát kh?i vòng l?p while
        }
    }

    res = f_close(&fp);
    if (res != FR_OK) {
        return;
    }
}

void clearFile(const char* filename) {
    FIL fp;
    FRESULT res;

    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        return;
    }

    res = f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        return;
    }

    res = f_truncate(&fp); // Xóa n?i dung c?a file
    if (res != FR_OK) {
        return;
    }

    res = f_close(&fp);
    if (res != FR_OK) {
        return;
    }
}
void displayEmployeeJobInfo(Employee* emp, Job* job) {
    char display_str[50]; // khai báo m?t m?ng char d? l?n d? ch?a chu?i

sprintf(display_str, "Ten:%s", emp->ten); // ghép chu?i "Ten: " và tên c?a nhân viên
sendlcd(display_str); // hi?n th? chu?i lên LCD
	beep(100,1);
HAL_Delay(2000);

sprintf(display_str, "Gio vao:  %s", job->giovao); // ghép chu?i "Gio vao: " và gi? vào c?a công vi?c
sendlcd(display_str); // hi?n th? chu?i lên LCD
	beep(100,1);
HAL_Delay(2000);

sprintf(display_str, "Gio ra:  %s", job->giora); // ghép chu?i "Gio ra: " và gi? ra c?a công vi?c
sendlcd(display_str); // hi?n th? chu?i lên LCD
	beep(100,1);
HAL_Delay(2000);
NVIC_SystemReset();
}
 void khoitaodoituong(void)
 {
		Employee emp1 = {0, "Phan Hoang Khai"};
		Employee emp2 = {1, "Nguyen Hoang Thien Bao"};
		Employee emp3 = {2, "Nguyen Hong Son"};

		writeEmployeeToFile(&emp1, "employees.bin");
		writeEmployeeToFile(&emp2, "employees.bin");
		writeEmployeeToFile(&emp3, "employees.bin");
			
		Job Job1 = {emp1, true, "", ""};
		Job Job2= {emp2, true, "", ""};
		Job Job3 = {emp3, true, "", ""};

		writeJobToFile(&Job1, "jobs.bin");
		writeJobToFile(&Job2, "jobs.bin");
		writeJobToFile(&Job3, "jobs.bin");
 }
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	  lcd_init();
	joystick_init(&hadc1);
	


	while (1)
			{
				sendlcd(functions[IDFunc]); // hi?n th? ch?c nang hi?n t?i lên LCD
				HAL_Delay(200);

				uint8_t joystick_position = get_joystick_position();
				switch (joystick_position) {
					case 1: // Nh?n joystick d? ch?n ch?c nang
						beep(100, 1);
						switch (IDFunc) {
							case 0: // Ch?n ch?c nang CH?M CÔNG
								beep(300, 1);
								pID=verify_fingerprint();
								if(pID ==-1)
									break;
								nv=readEmployeeFromFile("employees.bin",pID);
							job=readJobFromFile("jobs.bin",nv->id);	
							if(job->isGioVao==true) {
									strcpy(job->giovao, get_current_date_time(&hi2c2));
									strcpy(job->giora, "  ");
									job->isGioVao = false; // Ðánh d?u dã ghi nh?n gi? vào
							} else {
									strcpy(job->giora, get_current_date_time(&hi2c2));
									job->isGioVao = true; // Ðánh d?u dã ghi nh?n gi? ra
							}
							updateJobInFile(job,"jobs.bin");
							updateEmployeeInFile(nv,"employees.bin");
								//nv=readEmployeeFromFile("employees.bin",pID);
								//job=readJobFromFile("jobs.bin",nv->id);
								displayEmployeeJobInfo(nv,job);
								break;
							case 1: // Ch?n ch?c nang THÊM VÂN TAY
								beep(300, 1);
								int countNV=countEmployees("employees.bin");
								if(countNV==0)
								{			
									addFinger(IDFinger);
									IDFinger++;
									break;
								}
								else
									addFinger(countNV);						
								break;
							case 2: // Ch?n ch?c nang XÓA TOÀN B?
								beep(300, 1);
								deleteAllFinger();
								clearFile("employees.bin");
								clearFile("jobs.bin");
								break;
							case 3: // Ch?n ch?c nang XEM GI?
								beep(300, 1);
								xemgio();
							break;
							case 4:
								beep(300, 1);
								khoitaodoituong();
								sendlcd("Khoi tao 3 doi tuong");
								HAL_Delay(1000);
								break;
							default:
								break;
						}
						break;
					case 2: // Nh?n joystick sang trái d? chuy?n sang ch?c nang tru?c dó
						beep(100, 1);
						IDFunc--;
						if (IDFunc < 0) {
							IDFunc = 4;
						}
						break;
					case 3: // Nh?n joystick sang ph?i d? chuy?n sang ch?c nang k? ti?p
						beep(100, 1);
						IDFunc++;
						if (IDFunc >4) {
							IDFunc = 0;
						}
						break;
					default:
						break;
				}
			}

  /* USER CODE END 2 */

			
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
