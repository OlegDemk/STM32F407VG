/*
 * keyboard.c
 *
 *  Created on: Apr 8, 2021
 *      Author: odemki
 */

#include "main.h"
#include "keyboard.h"

#include "screens/ILI9341_Touchscreen.h"
#include "screens/ILI9341_STM32_Driver.h"
#include "screens/ILI9341_GFX.h"
#include "screens/LCD.h"

#include <stdbool.h>


//----------------------------------------------------------------------------------------
void read_digits(void)
{
	char digith = 0;;

	digith = read_one_digit_from_keyboard();
	if(digith == '#')						// Clean buffer
	{
		memset(keyboard.keyboard_digits_buffer, '\0', sizeof(keyboard.keyboard_digits_buffer));
		keyboard.read_digits_position = 0;
		keyboard.read_one_digit_status = false;
	}
	if(digith == '*')						// If pressed enter
	{
		keyboard.all_digits_was_read = true;
		keyboard.read_one_digit_status = false;
		keyboard.read_digits_position = 0;
	}

	if((digith != '\0') && (digith != '#')&& (digith != '*'))
	{
		if(keyboard.read_digits_position <= keyboard.how_meny_digits_must_be_written)
		{
			keyboard.keyboard_digits_buffer[keyboard.read_digits_position] = digith;
			keyboard.read_digits_position++;
			keyboard.all_digits_was_read = false;
			keyboard.read_one_digit_status = true;
		}
		else								// If entered all digits
		{
			keyboard.all_digits_was_read = true;
			keyboard.read_one_digit_status = false;
			keyboard.read_digits_position = 0;
		}
	}
	else									// If didn't enter any key
	{
		keyboard.read_one_digit_status = false;
	}
}

//----------------------------------------------------------------------------------------
char read_one_digit_from_keyboard(void)
{
	char digit = '\0';
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
			digit = '1';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			digit = '4';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			digit =  '7';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_1_Pin) == GPIO_PIN_RESET)	// Entered 1 or 4 or 7 or *
		{
			digit =  '*';
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
			digit =  '2';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			digit =  '5';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			digit = '8';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOB, KEYBOARD_COLUMN_2_Pin) == GPIO_PIN_RESET)
		{
			digit = '0';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
	}

	////////////////////////////////////////////////////////////////
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOE, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)	// Entered 3 or 6 or 9 or №   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	{
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(GPIOE, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			digit = '3';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOE, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			digit = '6';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOE, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			digit = '9';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOE, KEYBOARD_COLUMN_3_Pin) == GPIO_PIN_RESET)
		{
			digit = '#';
		}
		HAL_GPIO_WritePin(GPIOD, KEYBOARD_ROW_4_Pin, GPIO_PIN_SET);
	}
	return digit;
}
//----------------------------------------------------------------------------------------
