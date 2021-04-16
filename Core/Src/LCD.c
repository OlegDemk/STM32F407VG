/*
 * LCD.c
 *
 *  Created on: Apr 7, 2021
 *      Author: odemki
 */

#include "main.h"

#include "LCD.h"

#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "snow_tiger.h"

#include <string.h>

extern RNG_HandleTypeDef hrng;

static uint8_t getPressKey(void);  				// Function for read data from keyboard    // Simulation
static void Generic_Write(const char* Text);    // Function for print text on LCD
static void Level1Item3_Enter(void);// FOR LED

/** This is used when an invalid menu handle is required in
 *  a \ref MENU_ITEM() definition, i.e. to indicate that a
 *  menu has no linked parent, child, next or previous entry.
 */
Menu_Item_t NULL_MENU = {0};

/** \internal
 *  Pointer to the generic menu text display function
 *  callback, to display the configured text of a menu item
 *  if no menu-specific display function has been set
 *  in the select menu item.
 */
static void (*MenuWriteFunc)(const char* Text) = NULL;

/** \internal
 *  Pointer to the currently selected menu item.
 */
static Menu_Item_t* CurrentMenuItem = &NULL_MENU;
//-------------------------------------------------------------------------------------------

//bool flagPressed = false;						// flag pressed key

#define BUTTONn	5					 		// How many buttons use

typedef enum								// Enumeration for button state
{
  BUTTON_LEFT   = 0,
  BUTTON_UP   = 1,
  BUTTON_DOWN   = 2,
  BUTTON_RIGHT   = 3,
  BUTTON_SELECT  = 4,
  BUTTON_NOTHING  = 255
} Button_TypeDef;

// Menus  Name | Next | Prev | Parent | Child | SelectFunction | EnterFunction | Text
MENU_ITEM(Menu_1, Menu_2, Menu_3, NULL_MENU, Menu_1_1, NULL, NULL, "Menu-1");
MENU_ITEM(Menu_2, Menu_3, Menu_1, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-2");
MENU_ITEM(Menu_3, Menu_1, Menu_2, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-3");

MENU_ITEM(Menu_1_1, Menu_1_2, Menu_1_2, Menu_1, Menu_1_1_1, NULL, NULL, "Menu-1.1");
MENU_ITEM(Menu_1_2, Menu_1_1, Menu_1_1, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.2");

MENU_ITEM(Menu_1_1_1, Menu_1_1_2, Menu_1_1_4, Menu_1_1, NULL_MENU, NULL, NULL, "Menu-1.1.1");
MENU_ITEM(Menu_1_1_2, Menu_1_1_3, Menu_1_1_1, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.2");
MENU_ITEM(Menu_1_1_3, Menu_1_1_4, Menu_1_1_2, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.3");
MENU_ITEM(Menu_1_1_4, Menu_1_1_1, Menu_1_1_3, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.4");


bool flagPressed = false;						// flag presed key

void menu()
{
//	bool flagPressed = false;						// flag presed key
	//Menu_Navigate(&Menu_1);			// <<<<<<< What ti i????????????????????????
	Menu_SetGenericWriteCallback(Generic_Write);
	Menu_Navigate(&Menu_1);
	while (1)
	  {
	   uint8_t pressed_key = getPressKey();

	   if (pressed_key != BUTTON_NOTHING && !flagPressed)
	   {
//		    Menu_Navigate(&Menu_1);			// <<<<<<< What ti i????????????????????????
			//Menu_SetGenericWriteCallback(Generic_Write);	// print menu
			flagPressed = true;
			pressed_key = getPressKey();
			switch(pressed_key)
			{
			 case BUTTON_LEFT:
			  Menu_Navigate(MENU_PARENT);
			  break;
			 case BUTTON_UP:
			  Menu_Navigate(MENU_PREVIOUS);
			  break;
			 case BUTTON_DOWN:
			  Menu_Navigate(MENU_NEXT);
			  break;
			 case BUTTON_RIGHT:
			  Menu_Navigate(MENU_CHILD);
			  break;
			 case BUTTON_SELECT:
			  Menu_EnterCurrentItem();
			  break;
			 default:
			  break;
			}
	   }
	   else if (getPressKey() == BUTTON_NOTHING && flagPressed)
	   {
		   flagPressed = false;
	   }

	  }
}
// -----------------------------------------------------------------------
void RED_LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
// -----------------------------------------------------------------------
void RED_LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
// -----------------------------------------------------------------------
static uint8_t getPressKey()		// Simulation keyboard
{
////	BUTTON_LEFT   = 0,
////	BUTTON_UP   = 1,
////  BUTTON_DOWN   = 2,
////	BUTTON_RIGHT   = 3,
////	BUTTON_SELECT  = 4,
////	BUTTON_NOTHING  = 255
//	//flagPressed = true;


	char digit = '\0';
	char buff_lcd[20] = "KEY: ";
	digit = read_one_digit_from_keyboard();
	switch(digit)
	{
		flagPressed = true;			// Flag: Key was pressed
		case '0':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 10, 180, WHITE, 3, BLACK);
			return BUTTON_LEFT;
		}
		case '1':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 10, 180, WHITE, 3, BLACK);
			return BUTTON_UP;
		}
		case '2':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 10, 180, WHITE, 3, BLACK);
			return BUTTON_DOWN;
		}
		case '3':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 10, 180, WHITE, 3, BLACK);
			return BUTTON_RIGHT;
		}
		case '4':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 10, 180, WHITE, 3, BLACK);
			return BUTTON_SELECT;
		}
		default:
			return BUTTON_NOTHING;
			break;
	}
}
// -----------------------------------------------------------------------
void Generic_Write(const char* Text)
{
	if (Text)
	{
//		lcdClrScr();
//		lcdPuts(Text);

		ILI9341_Draw_Text( "                   ", 10, 10, WHITE, 3, BLACK);
		ILI9341_Draw_Text( Text, 10, 10, WHITE, 3, BLACK);
	}
}
// -----------------------------------------------------------------------
Menu_Item_t* Menu_GetCurrentMenu(void)
{
 return CurrentMenuItem;
}
// -----------------------------------------------------------------------
void Menu_Navigate(Menu_Item_t* const NewMenu)
{
 if ((NewMenu == &NULL_MENU) || (NewMenu == NULL))
 {
	 return;
 }

 CurrentMenuItem = NewMenu;

 if (MenuWriteFunc)
 {
	 MenuWriteFunc(CurrentMenuItem->Text);
 }

 void (*SelectCallback)(void) = CurrentMenuItem->SelectCallback;

 if (SelectCallback)
 {
	 SelectCallback();
 }
}
// -----------------------------------------------------------------------
void Menu_SetGenericWriteCallback(void (*WriteFunc)(const char* Text))
{
 MenuWriteFunc = WriteFunc;
 Menu_Navigate(CurrentMenuItem);
}
// -----------------------------------------------------------------------
void Menu_EnterCurrentItem(void)
{
 if ((CurrentMenuItem == &NULL_MENU) || (CurrentMenuItem == NULL))
  return;

 void (*EnterCallback)(void) = CurrentMenuItem->EnterCallback;

 if (EnterCallback)
  EnterCallback();
}// -----------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------
void lcd_menu(void)
{
	ILI9341_Draw_Text( "MENU", 100, 0, GREEN, 4, BLACK);
	ILI9341_Draw_Text( "1. Communication", 10, 40, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "2. Sensors", 10, 60, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "3. Clock", 10, 80, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "4. Game", 10, 100, WHITE, 2, BLACK);

}
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
