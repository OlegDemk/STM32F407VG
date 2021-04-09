/*
 * keyboard.c
 *
 *  Created on: Apr 8, 2021
 *      Author: odemki
 */

#include "main.h"

char keyboard_test(void)
{
	char num;
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
	{
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			num = '1';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			num = '4';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			num = '7';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			num = '*';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
	}
	////////////////////////////////////////////////////////////////
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)	// Entered 2 or 5 or 8 or 0
	{
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			num = '2';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			num = '5';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			num = '8';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			num = '0';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
	}

	////////////////////////////////////////////////////////////////
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 3 or 6 or 9 or №
	{
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			num = '3';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			num = '6';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			num = '9';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			num = '#';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
	}
	return num;
}


//void keyboard_test(void)
//{
//	char num;
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '1';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '4';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '7';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '*';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
//	////////////////////////////////////////////////////////////////
//
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '2';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '3';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '8';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '0';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
//
//	////////////////////////////////////////////////////////////////
//
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '3';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '6';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '9';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
//	if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
//	{
//		num = '#';
//	}
//	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
//
//
//}

