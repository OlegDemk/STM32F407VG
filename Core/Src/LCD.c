/*
 * LCD.c
 *
 *  Created on: Apr 7, 2021
 *      Author: odemki
 */

#include "main.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "snow_tiger.h"

extern RNG_HandleTypeDef hrng;

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
/*
 * Random generate circles
 */
void speed_test_LCD(int number_of_tests)
{
	int i =0;
	while(i <= number_of_tests)
	{
		i ++;
		//ILI9341_Draw_Text("", i, 200, YELLOW, 4, BLACK);
		uint32_t random_num = 0;
		uint16_t xr = 0;
		uint16_t yr = 0;
		uint16_t radiusr = 0;
		uint16_t colourr = 0;

		random_num = HAL_RNG_GetRandomNumber(&hrng);
		xr = random_num;
		random_num = HAL_RNG_GetRandomNumber(&hrng);
		yr = random_num;
		random_num = HAL_RNG_GetRandomNumber(&hrng);
		radiusr = random_num;
		random_num = HAL_RNG_GetRandomNumber(&hrng);
		colourr = random_num;

		xr &= 0x01FF;
		yr &= 0x01FF;
		radiusr &= 0x001F;
		colourr &= 0xFFFF;
		ILI9341_Draw_Filled_Circle(xr, yr, radiusr, colourr);
		//ILI9341_Draw_Pixel(xr, yr, WHITE);
	}

}
//-------------------------------------------------------------------------------------------
/*
 * Print coordinates on LCD
 */
void test_touchsreen(void)
{
	if(TP_Touchpad_Pressed())
	{
		uint16_t x_pos_1 = 0;
		uint16_t y_pos_1 = 0;

		uint16_t x_pos_2 = 0;
		uint16_t y_pos_2 = 0;

		uint16_t position_array[2];
		if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
		{
			x_pos_1 = position_array[0];
			y_pos_1 = position_array[1];

			char x_pos_char_buff[15] = {0};
			char y_pos_char_buff[15] = {0};

			TP_Read_Coordinates(position_array);
			x_pos_2 = position_array[0];
			y_pos_2 = position_array[1];

			if((x_pos_1 != x_pos_2) || (y_pos_1 != y_pos_2))		// If changed position clear lines
			{
				ILI9341_Draw_Text("            ", 0, 0, WHITE, 3, BLACK);
				ILI9341_Draw_Text("            ", 0, 20, WHITE, 3, BLACK);
			}

			if((x_pos_1 == x_pos_2) && (y_pos_1 == y_pos_2))
			{

			}
			else
			{
				sprintf(x_pos_char_buff, "x_pos: %d", x_pos_2);
				ILI9341_Draw_Text(x_pos_char_buff, 0, 0, WHITE, 2, BLACK);
				sprintf(y_pos_char_buff, "y_pos: %d", y_pos_2);
				ILI9341_Draw_Text(y_pos_char_buff, 0, 20, WHITE, 2, BLACK);
			}

		}
	}
	else
	{
		ILI9341_Draw_Text("            ", 0, 0, WHITE, 3, BLACK);
		ILI9341_Draw_Text("            ", 0, 20, WHITE, 3, BLACK);
	}
}
//-------------------------------------------------------------------------------------------
