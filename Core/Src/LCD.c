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

#include "keyboard.h"

#include <string.h>

extern RNG_HandleTypeDef hrng;

static uint8_t getPressKey(void);  				// Function for read data from keyboard    // Simulation
static void Generic_Write(const char* Text);    // Function for print text on LCD
static void Level1Item3_Enter(void);// FOR LED

/* Це використовується, коли у визначенні \ ref MENU_ITEM ()
 потрібен недійсний дескриптор меню, тобто, щоб вказати,
 що меню не має пов'язаних батьків, дочірніх елементів, наступного або попереднього запису.
 */
Menu_Item_t NULL_MENU = {0};

/** \internal
Вказівник на загальний виклик функції відображення тексту меню,
щоб відобразити конфігурований текст елемента меню, якщо у
вибраному пункті меню не було встановлено жодної функції відображення меню.
*/
static void (*MenuWriteFunc)(const char* Text) = NULL;

/** \internal
 *  Pointer to the currently selected menu item.
 */
static Menu_Item_t* CurrentMenuItem = &NULL_MENU;
//-------------------------------------------------------------------------------------------

//bool flagPressed = false;					// flag pressed key

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


// Test place ///////////////////////////////////////////////////////
//void (*print_all_top_menu)(const char* Text);
void print_all_top_menu();
/////////////////////////////////////////////////////////////////////

char main_menus[5][20] =
{
		"Menu-1",
		"Menu-2",
		"Menu-3",
		"Menu-4 Red LED",
		"Menu-5"
};

// Menus  Name | Next | Prev | Parent | Child | SelectFunction | EnterFunction | Text
MENU_ITEM(Menu_1, Menu_2, Menu_4, NULL_MENU, Menu_1_1, print_all_top_menu, NULL, "Menu-1");
MENU_ITEM(Menu_2, Menu_3, Menu_1, NULL_MENU, NULL_MENU, print_all_top_menu, NULL, "Menu-2");
MENU_ITEM(Menu_3, Menu_4, Menu_2, NULL_MENU, NULL_MENU, print_all_top_menu, NULL, "Menu-3");
MENU_ITEM(Menu_4, Menu_5, Menu_3, NULL_MENU, NULL_MENU, print_all_top_menu, Level1Item3_Enter, "Menu-4 Red LED");
MENU_ITEM(Menu_5, Menu_1, Menu_4, NULL_MENU, NULL_MENU, print_all_top_menu, Level1Item3_Enter, "Menu-5");

MENU_ITEM(Menu_1_1, Menu_1_2, Menu_1_2, Menu_1, Menu_1_1_1, NULL, NULL, "Menu-1.1");
MENU_ITEM(Menu_1_2, Menu_1_1, Menu_1_1, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.2");

MENU_ITEM(Menu_1_1_1, Menu_1_1_2, Menu_1_1_4, Menu_1_1, NULL_MENU, NULL, NULL, "Menu-1.1.1");
MENU_ITEM(Menu_1_1_2, Menu_1_1_3, Menu_1_1_1, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.2");
MENU_ITEM(Menu_1_1_3, Menu_1_1_4, Menu_1_1_2, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.3");
MENU_ITEM(Menu_1_1_4, Menu_1_1_1, Menu_1_1_3, NULL_MENU, NULL_MENU, NULL, NULL, "Menu-1.1.4");


bool flagPressed = false;						// flag pressed key

uint8_t pointer_on_selected_menu =1;
//uint8_t p =0;
//void print_sign(p);

void menu()
{
	Menu_SetGenericWriteCallback(Generic_Write);  //передача показника на Generic_Write функцію  point in Menu_SetGenericWriteCallback

	//print_all_top_menu();
	Menu_Navigate(&Menu_1);
//	Menu_Navigate(MENU_NEXT);
//	Menu_Navigate(MENU_NEXT);
//	print_sign(p);
//	Menu_Navigate(&Menu_2);
//	Menu_Navigate(&Menu_3);
	while (1)
	  {
	   uint8_t pressed_key = getPressKey();

	   if (pressed_key != BUTTON_NOTHING && !flagPressed)
	   {
			flagPressed = true;
			switch(pressed_key)
			{
				case BUTTON_LEFT:
					Menu_Navigate(MENU_PARENT);	// Перейти на MENU_PARENT
					break;

				case BUTTON_UP:
					pointer_on_selected_menu--;
					Menu_Navigate(MENU_PREVIOUS);  // Перейти на MENU_PREVIOUS
					break;

				case BUTTON_DOWN:
					pointer_on_selected_menu++;
					Menu_Navigate(MENU_NEXT);      // Перейти на MENU_NEXT
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
//// TEST PRINT FUNCTION <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void print_all_top_menu(void)
{
	// Print all main menus
	ILI9341_Draw_Text(main_menus[0], 40, 110, WHITE, 2, BLACK);
	ILI9341_Draw_Text(main_menus[1], 40, 130, WHITE, 2, BLACK);
	ILI9341_Draw_Text(main_menus[2], 40, 150, WHITE, 2, BLACK);
	ILI9341_Draw_Text(main_menus[3], 40, 170, WHITE, 2, BLACK);
	ILI9341_Draw_Text(main_menus[4], 40, 190, WHITE, 2, BLACK);

	//
	if(pointer_on_selected_menu <= 0)
	{
		pointer_on_selected_menu = 5;
	}
	if(pointer_on_selected_menu >= 6)
	{
		pointer_on_selected_menu = 1;
	}
	// Clearn all pointers on menu
	ILI9341_Draw_Text( "  ", 5, 110, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "  ", 5, 130, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "  ", 5, 150, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "  ", 5, 170, WHITE, 2, BLACK);
	ILI9341_Draw_Text( "  ", 5, 190, WHITE, 2, BLACK);

	// Point pointer on menu
	switch (pointer_on_selected_menu)
	{
		case 1:
			ILI9341_Draw_Text( "->", 5, 110, RED, 2, BLACK);
			break;
		case 2:
			ILI9341_Draw_Text( "->", 5, 130, RED, 2, BLACK);
			break;
		case 3:
			ILI9341_Draw_Text( "->", 5, 150, RED, 2, BLACK);
			break;
		case 4:
			ILI9341_Draw_Text( "->", 5, 170, RED, 2, BLACK);
			break;
		case 5:
			ILI9341_Draw_Text( "->", 5, 190, RED, 2, BLACK);
			break;
	}

}
//// -----------------------------------------------------------------------
void print_main_menus(Menu_Item_t* const NewMenu)
{
//	static uint8_t k =0;
//	CurrentMenuItem = NewMenu;
//	k = k + 20;
//	ILI9341_Draw_Text( CurrentMenuItem->Text, 10, 60+k, WHITE, 2, BLACK);
//	k = k + 20;

	//MenuWriteFunc(CurrentMenuItem->Text);			// Print Text string on LCD

}
//// -----------------------------------------------------------------------
//void print_sign(p)
//{
//	ILI9341_Draw_Text( "<", 180, 60+p, WHITE, 2, BLACK);
//}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------
void RED_LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
// -----------------------------------------------------------------------
void RED_LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
}
// -----------------------------------------------------------------------
static void Level1Item3_Enter(void)
{
 bool flagPressed = false;

 ILI9341_Draw_Text( "                          ", 10, 60, WHITE, 2, BLACK);   // Clearn lcd

 ILI9341_Draw_Text( "Red LED is:", 10, 60, WHITE, 2, BLACK);

 while(getPressKey() != BUTTON_LEFT)
 {
	 // Read PIN
	 int led_status =  HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_14);

	 if(flagPressed == true)
	 {
		 if(led_status == GPIO_PIN_SET)
		 {
			 ILI9341_Draw_Text( "ON ", 150, 60, WHITE, 2, BLACK);
		 }
		 else
		 {
		 	 ILI9341_Draw_Text( "OFF", 150, 60, WHITE, 2, BLACK);
		 }
	 }


	 if(getPressKey() != BUTTON_NOTHING && !flagPressed)
	 {
		 flagPressed = true;

		 switch(getPressKey())  // If press any key
		 {
	  	  	  case BUTTON_UP:
	  	  		  RED_LED_ON();
	  	  		  break;
	  	  	  case BUTTON_DOWN:
	  	  		  RED_LED_OFF();
	  	  		  break;
	  	  	  default:
	  	  		  break;
		 }
	 }
	 else if(getPressKey() == BUTTON_NOTHING && flagPressed)
	 {
		 flagPressed = false; 	// if button was release
	 }
 }

 ILI9341_Draw_Text( "                           ", 10, 60, WHITE, 2, BLACK); // Cleaning one row LCD
 Menu_Navigate(&Menu_3);  // Back to menu 3
}
// -----------------------------------------------------------------------
static uint8_t getPressKey()		// 3x4 keyboard
{
	char digit = '\0';
	char buff_lcd[20] = "KEY: ";
	digit = read_one_digit_from_keyboard();
	switch(digit)
	{
		case '4':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 0, 0, WHITE, 2, BLACK);
			return BUTTON_LEFT;
		}
		case '2':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 0, 0, WHITE, 2, BLACK);
			return BUTTON_UP;
		}
		case '8':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 0, 0, WHITE, 2, BLACK);
			return BUTTON_DOWN;
		}
		case '6':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 0, 0, WHITE, 2, BLACK);
			return BUTTON_RIGHT;
		}
		case '5':
		{
			strncat(buff_lcd, &digit, 1);
			ILI9341_Draw_Text( buff_lcd, 0, 0, WHITE, 2, BLACK);
			return BUTTON_SELECT;
		}
		default:
			return BUTTON_NOTHING;
			break;


	}
}
// -----------------------------------------------------------------------
void Generic_Write(const char* Text)		// Print "Text" data on LCD
{
	if (Text)
	{
		ILI9341_Draw_Text( "                   ", 10, 60, WHITE, 2, BLACK);
		ILI9341_Draw_Text( Text, 10, 60, WHITE, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
Menu_Item_t* Menu_GetCurrentMenu(void)		// Повертає поточни вибраний пункт меню
{
	return CurrentMenuItem;
}
// -----------------------------------------------------------------------
// Переходи по меню
// in: ПОказник на апсолютний пункт меню, для вибору
// MENU_PARENT, MENU_CHILD, MENU_NEXT або  MENU_PREVIOUS
void Menu_Navigate(Menu_Item_t* const NewMenu)
{
	if ((NewMenu == &NULL_MENU) || (NewMenu == NULL))  // What it mean???
	{
		return;		// Exit
	}

	CurrentMenuItem = NewMenu;    // Передане маню стає вибране

	if (MenuWriteFunc)    // If   MenuWriteFunc  != NULL  ???????
	{
		MenuWriteFunc(CurrentMenuItem->Text);			// Print Text string on LCD
	}

	//   void (*SelectCallback)(void) - Creating pointer on function
	// Write  CurrentMenuItem->SelectCallback in  void (*SelectCallback)(void) pinter
	void (*SelectCallback)(void) = CurrentMenuItem->SelectCallback;

	if (SelectCallback)		// If SelectCallback != NULL
	{
		SelectCallback();   // It does - CurrentMenuItem->SelectCallback;
	}
}
// -----------------------------------------------------------------------
/*Налаштовує функцію зворотного виклику тексту меню, що спрацьовує для всіх
 пунктів меню. В рамках цієї функції зворотного виклику користувач повинен
 реалізувати код для відображення поточного тексту меню, що зберігається
 в  ref MENU_ITEM_STORAGE пам'яті.. */
// In: вказівник на функцію зворотного виклику для виконання кожного вибраного пункту меню.
void Menu_SetGenericWriteCallback(void (*WriteFunc)(const char* Text))    //  What doing this function??????? <<<<<<<<<<<
{
	MenuWriteFunc = WriteFunc;  		// Запис показника функції WriteFunc в
	//  показник на функцію під назвою MenuWriteFunc
	Menu_Navigate(CurrentMenuItem);      // Передача цього показника в функцію Menu_Navigate
}
// -----------------------------------------------------------------------
/* Функція входить у вибраний на даний момент пункт меню, виконуючи налаштовану
   функцію зворотного дзвінка (якщо така є) */
void Menu_EnterCurrentItem(void)
{
	if ((CurrentMenuItem == &NULL_MENU) || (CurrentMenuItem == NULL))
	{
		return;
	}

	void (*EnterCallback)(void) = CurrentMenuItem->EnterCallback;
	if (EnterCallback)		// If EnterCallback != NULL
	{
		EnterCallback();
	}

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
