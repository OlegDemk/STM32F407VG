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

void test_touchsreen(void)
{
	if(TP_Touchpad_Pressed())
		 {
			uint16_t x_pos = 0;
			uint16_t y_pos = 0;

			ILI9341_Draw_Text("            ", 0, 0, WHITE, 3, BLACK);
			ILI9341_Draw_Text("            ", 0, 20, WHITE, 3, BLACK);

			uint16_t position_array[2];
			if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
			{
				x_pos = position_array[0];
				y_pos = position_array[1];

				char x_pos_char_buff[15] = {0};
				char y_pos_char_buff[15] = {0};
				sprintf(x_pos_char_buff, "x_pos: %d", x_pos);
				ILI9341_Draw_Text(x_pos_char_buff, 0, 0, WHITE, 2, BLACK);
				sprintf(y_pos_char_buff, "y_pos: %d", y_pos);
				ILI9341_Draw_Text(y_pos_char_buff, 0, 20, WHITE, 2, BLACK);

	//			HAL_Delay(300);
	//			ILI9341_Draw_Text("            ", 0, 60, WHITE, 3, BLACK);
	//			ILI9341_Draw_Text("            ", 0, 90, WHITE, 3, BLACK);
			}
		 }
		 else
		 {
		 	ILI9341_Draw_Text("            ", 0, 0, WHITE, 3, BLACK);
		 	ILI9341_Draw_Text("            ", 0, 20, WHITE, 3, BLACK);
		 }
}
