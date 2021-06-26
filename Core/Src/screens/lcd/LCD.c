/*
 * LCD.c
 *
 *  Created on: Apr 7, 2021
 *      Author: odemki
 */


// How add new menu. < -----------------------------------------
/*
For add new menu need.
1. Create char array with names created menu items.
2. Create new MENU_ITEM and fill in it.
3. Create function for print new menu.
4. Create new variable: menu_level_X_X_position_pointer (pointer on menu item).
	Add its variable in switch(pressed_key).
5. Create new variable: menu_level_X_X. (Variabls for optimisation drawing on LCD).
*/

#include "main.h"

#include "screens/lcd/LCD.h"
#include "screens/lcd/ILI9341_Touchscreen.h"
#include "screens/lcd/ILI9341_STM32_Driver.h"
#include "screens/lcd/ILI9341_GFX.h"
#include "screens/lcd/snow_tiger.h"

#include "keyboard.h"

#include <string.h>
#include <stdio.h>

#include "sensors/main_sensor_file.h"

extern RNG_HandleTypeDef hrng;

static uint8_t getPressKey(void);  				// Function for read data from keyboard    // Simulation
static void Generic_Write(const char* Text);    // Function for print text on LCD
static void Level1Item3_Enter(void);			// FOR LED

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


// Function for print data on LCD
void print_all_top_menu(void);
void print_all_menu_level_1_a_communication(void);
void print_all_menu_level_1_b_sensors(void);
void print_all_menu_level_1_c_sensors(void);
void print_all_menu_1_1_menus(void);

// Clean some LCD part functions
void clean_all_lines_of_menu (void);
void clearn_all_pointers_on_menu(void);

void show_pointer_on_menu_item(uint8_t *position_pointer_on_item_menu, uint8_t number_items_on_menu);


// Function for measure and print data from sensors on LCD
void print_status_resdy_all_sensors_on_lcd(void);
void print_data_from_BME280_on_lcd(void);						// Temperature, humidity and pressure
void print_data_from_MPU6050_Acc_and_Gyro_on_lcd(void); 		//  Acceleration X, Y, Z and Gyroscope X, Y, Z
void print_data_from_HMC5883L_on_lcd(void);						// Magnetometer
void print_data_from_MS5611_on_lcd(void);						// Temperature and pressure
void print_data_from_AM2302_on_lcd(void);						// Temperature and humidity
void print_data_from_APD59960_on_lcd(void);						// Motion sensor

void toggle_green_led(void);									// Function for toggle LED (example)

void measure_and_print_all_sensors(void);

bool flagPressed = false;						// flag pressed key

int min_line_position = 90;	// Position on LCD
int max_line_position = 210;	// Position on LCD

// Variabls for optimisation drawing on LCD
bool menu_level_0 = false;
bool menu_level_1_A = false;
bool menu_level_1_B = false;
bool menu_level_1_C = false;

uint8_t menu_level_0_position_pointer = 1;
uint8_t menu_level_1_A_position_pointer = 1;
uint8_t menu_level_1_B_position_pointer = 1;
uint8_t menu_level_1_C_position_pointer = 1;

uint8_t pointer_on_selected_menu =1;


char main_menus[5][20] =
{
		"1. Communication",
		"2. Sensors",
		"3. Data logging",
		"4. Clock",
		"5. Menu"
};

char menu_level_1_a_str[4][20] =
{
		"1. GSM",
		"2. NRf",
		"3. ESP",
		"4. LoRa"
};

char menu_level_1_b_str[7][20] =
{
		"1. BME280",
		"2. MPU6050",
		"3. HMC5883L",
		"4. MS5611",
		"5. AM2302",
		"6. APDS9960",
		"7. MEASURE ALL"
};

char menu_level_1_c_str[2][20] =
{
		"1. Flash memory",
		"2. W25Q memory"
};

char menu_1_1[4][20] =
{
		"Menu-1.1.1",
		"Menu-1.1.2",
		"Menu-1.1.3",
		"Menu-1.1.4"
};


// Menus  Name | Next | Prev | Parent | Child | SelectFunction | EnterFunction | Text
// Main menu Level 0
MENU_ITEM(Menu_1, Menu_2, Menu_5, NULL_MENU, Menu_1_1, print_all_top_menu, NULL, "1. Communication");
MENU_ITEM(Menu_2, Menu_3, Menu_1, NULL_MENU, Menu_2_1, print_all_top_menu, NULL, "2. Sensors");
MENU_ITEM(Menu_3, Menu_4, Menu_2, NULL_MENU, Menu_3_1, print_all_top_menu, NULL, "3. Data logging");
MENU_ITEM(Menu_4, Menu_5, Menu_3, NULL_MENU, NULL_MENU, print_all_top_menu, NULL, "4. Clock");
MENU_ITEM(Menu_5, Menu_1, Menu_4, NULL_MENU, NULL_MENU, print_all_top_menu, NULL, "5. Menu");
//MENU_ITEM(Menu_4, Menu_5, Menu_3, NULL_MENU, NULL_MENU, print_all_top_menu, Level1Item3_Enter, "4. Menu. Red LED");

// Menu level 1 a (Communications)      menu_level_1_a     print_all_menu_1_communication
MENU_ITEM(Menu_1_1, Menu_1_2, Menu_1_4, Menu_1, Menu_1_1_1, print_all_menu_level_1_a_communication, NULL, "1. GSM");
MENU_ITEM(Menu_1_2, Menu_1_3, Menu_1_1, Menu_1, NULL_MENU, print_all_menu_level_1_a_communication, NULL, "2. NRf");
MENU_ITEM(Menu_1_3, Menu_1_4, Menu_1_2, Menu_1, NULL_MENU, print_all_menu_level_1_a_communication, NULL, "3. ESP");
MENU_ITEM(Menu_1_4, Menu_1_1, Menu_1_3, Menu_1, NULL_MENU, print_all_menu_level_1_a_communication, NULL, "4. LoRa");

// Menu level 1 b (Sensors)
MENU_ITEM(Menu_2_1, Menu_2_2, Menu_2_7, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "1. BME280");
MENU_ITEM(Menu_2_2, Menu_2_3, Menu_2_1, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "2. MPU6050");
MENU_ITEM(Menu_2_3, Menu_2_4, Menu_2_2, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "3. HMC5883L");
MENU_ITEM(Menu_2_4, Menu_2_5, Menu_2_3, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "4. MS5611");
MENU_ITEM(Menu_2_5, Menu_2_6, Menu_2_4, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "5. AM2302");
//MENU_ITEM(Menu_2_5, Menu_2_6, Menu_2_4, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, print_AM2302_data_on_lcd, "5. AM2302");
MENU_ITEM(Menu_2_6, Menu_2_7, Menu_2_5, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, NULL, "6. APDS9960");
MENU_ITEM(Menu_2_7, Menu_2_1, Menu_2_6, Menu_1, NULL_MENU, print_all_menu_level_1_b_sensors, measure_and_print_all_sensors, "7. MEASURE ALL");


//// Menu level 1 c (Data logging)
MENU_ITEM(Menu_3_1, Menu_3_2, Menu_3_2, Menu_1, NULL_MENU, print_all_menu_level_1_c_sensors, NULL, "1. Flash memory");
MENU_ITEM(Menu_3_2, Menu_3_1, Menu_3_1, Menu_1, NULL_MENU, print_all_menu_level_1_c_sensors, NULL, "2. W25Q memory");

// Example
MENU_ITEM(Menu_1_1_1, Menu_1_1_2, Menu_1_1_4, Menu_1_1, NULL_MENU, print_all_menu_1_1_menus, NULL, "Menu-1.1.1");
MENU_ITEM(Menu_1_1_2, Menu_1_1_3, Menu_1_1_1, NULL_MENU, NULL_MENU, print_all_menu_1_1_menus, NULL, "Menu-1.1.2");
MENU_ITEM(Menu_1_1_3, Menu_1_1_4, Menu_1_1_2, NULL_MENU, NULL_MENU, print_all_menu_1_1_menus, NULL, "Menu-1.1.3");
MENU_ITEM(Menu_1_1_4, Menu_1_1_1, Menu_1_1_3, NULL_MENU, NULL_MENU, print_all_menu_1_1_menus, toggle_green_led, "Menu-1.1.4");


// -----------------------------------------------------------------------
void menu()
{
	// Виводить номер меню на якому стоїть стрілка
	Menu_SetGenericWriteCallback(Generic_Write);  // передача показника на Generic_Write функцію  point in Menu_SetGenericWriteCallback
	Menu_Navigate(&Menu_1);

	while (1)
	{
		uint8_t pressed_key = getPressKey();

		if (pressed_key != BUTTON_NOTHING && !flagPressed)
		{
			flagPressed = true;
			switch(pressed_key)
			{
				case BUTTON_LEFT:
					Menu_Navigate(MENU_PARENT);
					break;

				case BUTTON_UP:
					menu_level_0_position_pointer--;
					menu_level_1_A_position_pointer--;
					menu_level_1_B_position_pointer--;
					menu_level_1_C_position_pointer--;
					Menu_Navigate(MENU_PREVIOUS);
					break;

				case BUTTON_DOWN:
					menu_level_0_position_pointer++;
					menu_level_1_A_position_pointer++;
					menu_level_1_B_position_pointer++;
					menu_level_1_C_position_pointer++;
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
/*
 * Print top menu
 */
void print_all_top_menu(void)
{
	menu_level_1_A = false;
	menu_level_1_B = false;
	menu_level_1_C = false;

	// For print all items menu one time
	if(menu_level_0 == false)
	{
		clean_all_lines_of_menu ();

		ILI9341_Draw_Text(main_menus[0], 30, 90, WHITE, 2, BLACK);
		ILI9341_Draw_Text(main_menus[1], 30, 110, WHITE, 2, BLACK);
		ILI9341_Draw_Text(main_menus[2], 30, 130, WHITE, 2, BLACK);
		ILI9341_Draw_Text(main_menus[3], 30, 150, WHITE, 2, BLACK);
		ILI9341_Draw_Text(main_menus[4], 30, 170, WHITE, 2, BLACK);

		menu_level_0 = true;					// Trigger variable
		menu_level_0_position_pointer = 1;		// Pointers on item menu always show on first item menu.
	}

	// Clear all pointers on menu
	clearn_all_pointers_on_menu();
	show_pointer_on_menu_item(&menu_level_0_position_pointer, 5);
}
// -----------------------------------------------------------------------
void print_all_menu_level_1_a_communication(void)
{
	menu_level_0 = false;
	menu_level_1_B = false;
	menu_level_1_C = false;

	// Clear all menu place
	// For print all items menu one time
	if(menu_level_1_A == false)
	{
		clean_all_lines_of_menu ();

		ILI9341_Draw_Text(menu_level_1_a_str[0], 30, 90, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_a_str[1], 30, 110, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_a_str[2], 30, 130, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_a_str[3], 30, 150, WHITE, 2, BLACK);

		menu_level_1_A_position_pointer = 1;
		menu_level_1_A = true;
	}

	// Clear all pointers on menu
	clearn_all_pointers_on_menu();
	show_pointer_on_menu_item(&menu_level_1_A_position_pointer, 4);
}
// -----------------------------------------------------------------------
void print_all_menu_level_1_b_sensors(void)
{

	menu_level_0 = false;
	menu_level_1_A = false;
	menu_level_1_C = false;

	// For print all items menu one time
	if(menu_level_1_B == false)
	{
		clean_all_lines_of_menu ();

		ILI9341_Draw_Text(menu_level_1_b_str[0], 30, 90, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[1], 30, 110, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[2], 30, 130, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[3], 30, 150, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[4], 30, 170, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[5], 30, 190, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_b_str[6], 30, 210, YELLOW, 2, BLACK);

		// 1. Show ready status  for all sensors
		print_status_resdy_all_sensors_on_lcd();

		menu_level_1_B_position_pointer = 1;
		menu_level_1_B = true;
	}

	// Clear all pointers on menu
	clearn_all_pointers_on_menu();
	show_pointer_on_menu_item(&menu_level_1_B_position_pointer, 7);


}
// -----------------------------------------------------------------------
void print_all_menu_level_1_c_sensors(void)
{

	menu_level_0 = false;
	menu_level_1_A = false;
	menu_level_1_B = false;

	// For print all items menu one time
	if(menu_level_1_C == false)
	{
		clean_all_lines_of_menu ();

		ILI9341_Draw_Text(menu_level_1_c_str[0], 30, 90, WHITE, 2, BLACK);
		ILI9341_Draw_Text(menu_level_1_c_str[1], 30, 110, WHITE, 2, BLACK);

		menu_level_1_C_position_pointer = 1;
		menu_level_1_C = true;
	}

	// Clear all pointers on menu
	clearn_all_pointers_on_menu();
	show_pointer_on_menu_item(&menu_level_1_C_position_pointer, 2);
}
// -----------------------------------------------------------------------
void print_all_menu_1_1_menus(void)
{
	menu_level_0 = false;
	// Clean all menu place
	clean_all_lines_of_menu ();

	// Clear all pointers on menu
	clearn_all_pointers_on_menu();

	ILI9341_Draw_Text(menu_1_1[0], 30, 110, WHITE, 2, BLACK);
	ILI9341_Draw_Text(menu_1_1[1], 30, 130, WHITE, 2, BLACK);
	ILI9341_Draw_Text(menu_1_1[2], 30, 150, WHITE, 2, BLACK);
	ILI9341_Draw_Text(menu_1_1[3], 30, 170, WHITE, 2, BLACK);

	if(pointer_on_selected_menu <= 0)
	{
		pointer_on_selected_menu = 4;
	}
	if(pointer_on_selected_menu >= 5)
	{
		pointer_on_selected_menu = 1;
	}

	// Print pointer on LCD
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
	}
}
// -----------------------------------------------------------------------
void toggle_green_led(void)
{
	HAL_GPIO_TogglePin( GPIOD, GPIO_PIN_12);
}
// -----------------------------------------------------------------------
void clean_all_lines_of_menu (void)
{
	for (int line = min_line_position; line <= max_line_position; line = line +20)
	{
		ILI9341_Draw_Text( "                                   ", 30, line, RED, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
// Clean all pointers on menu (arrows)
void clearn_all_pointers_on_menu(void)
{
	for (int line = min_line_position; line <= max_line_position; line = line +20)
	{
		ILI9341_Draw_Text( "  ", 5, line, WHITE, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
// Show -> near menu items
void show_pointer_on_menu_item(uint8_t *position_pointer_on_item_menu, uint8_t number_items_on_menu)
{
	if(*position_pointer_on_item_menu <= 0)
	{
		*position_pointer_on_item_menu = number_items_on_menu;
	}

	//
	if(*position_pointer_on_item_menu >= (number_items_on_menu + 1))
	{
		*position_pointer_on_item_menu = 1;
	}

	// Print pointer on menu
	switch (*position_pointer_on_item_menu)
	{
		case 1:
			ILI9341_Draw_Text( "->", 5, 90, RED, 2, BLACK);
			break;
		case 2:
			ILI9341_Draw_Text( "->", 5, 110, RED, 2, BLACK);
			break;
		case 3:
			ILI9341_Draw_Text( "->", 5, 130, RED, 2, BLACK);
			break;
		case 4:
			ILI9341_Draw_Text( "->", 5, 150, RED, 2, BLACK);
			break;
		case 5:
			ILI9341_Draw_Text( "->", 5, 170, RED, 2, BLACK);
			break;
		case 6:
			ILI9341_Draw_Text( "->", 5, 190, RED, 2, BLACK);
			break;
		case 7:
			ILI9341_Draw_Text( "->", 5, 210, RED, 2, BLACK);
			break;
		}
}
// -----------------------------------------------------------------------
// Print "ready" or "Error""status all sensors.
void print_status_resdy_all_sensors_on_lcd(void)
{
	if(i2c_device.BME280_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 90, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 90, RED, 2, BLACK);
	}

	if(i2c_device.MPU6050_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 110, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 110, RED, 2, BLACK);
	}

	if(i2c_device.HMC5883L_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 130, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 130, RED, 2, BLACK);
	}

	if(i2c_device.MS5611_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 150, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 150, RED, 2, BLACK);
	}

	if(i2c_device.AM2302_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 170, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 170, RED, 2, BLACK);
	}

	if(i2c_device.APDS9960_ready_status == true)
	{
		ILI9341_Draw_Text("Ready", 240, 190, GREEN, 2, BLACK);
	}
	else
	{
		ILI9341_Draw_Text("Error", 240, 190, RED, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
void measure_and_print_all_sensors(void)
{
	bool sensor_print_status = false;			// For print only first time
	int i = 0;									// For create different period measure
	// Clear all LCD
	ILI9341_Fill_Screen(BLACK);

	while(getPressKey() != BUTTON_LEFT)
	{
		if(sensor_print_status == false)	// Print lines only first time.
		{
			// Print name sensors
			ILI9341_Draw_Text( "1.BME280", 5, 90, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "2.MPU6050a", 5, 110, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "3.MPU6050g", 5, 130, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "4.HMC5883L", 5, 150, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "5.MS5611", 5, 170, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "6.AM2302", 5, 190, WHITE, 2, BLACK);
			ILI9341_Draw_Text( "7.APDS9960", 5, 210, WHITE, 2, BLACK);

			// Print lines
			ILI9341_Draw_Horizontal_Line(0, 89, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 109, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 129, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 149, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 169, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 189, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 209, 320, GREEN);
			ILI9341_Draw_Horizontal_Line(0, 229, 320, GREEN);
			ILI9341_Draw_Vertical_Line(0, 89, 140, GREEN);
			ILI9341_Draw_Vertical_Line(23, 89, 140, GREEN);
			ILI9341_Draw_Vertical_Line(130, 89, 140, GREEN);
			ILI9341_Draw_Vertical_Line(319, 89, 140, GREEN);

			sensor_print_status = true;
		}

		// Measuring and printing
		print_data_from_MPU6050_Acc_and_Gyro_on_lcd();   	// Acceleration X, Y, Z

		// print_data_from_HMC5883L_on_lcd();		// Magnetometer DON'T WORK

		if((i >= 10) | (i == 0))					// Measured and printed only one time per one second
		{
			print_data_from_AM2302_on_lcd();		// Temperature and humidity
			print_data_from_BME280_on_lcd();		// Temperature, humidity and pressure
			print_data_from_MS5611_on_lcd();		// Temperature and pressure
			i = 0;
		}

		// print_data_from_APD59960_on_lcd();		// Motion sensor  DON'T WORK

		HAL_Delay(100);
		i++;
	}
	ILI9341_Fill_Screen(BLACK);
	Menu_Navigate(&Menu_1);     // Back to menu 2
}

// -----------------------------------------------------------------------
void print_data_from_AM2302_on_lcd(void)
{
	if(i2c_device.AM2302_ready_status == true)
	{
		am2302_measure();
		// Convert data
		int temp = i2c_device.AM2302_temperature;
		int hum = i2c_device.AM2302_humidity;

		char str_t[20] = {0};
		char str_h[10] = {0};
		char cels_s[7] = " C  ";
		char hum_s[4] = " %";

		// Add strings
		itoa(temp, str_t, 10);
		itoa(hum, str_h, 10);

		strcat(str_t, cels_s);
		strcat(str_h, hum_s);

		strcat(str_t, str_h);

		// Print like one line
		ILI9341_Draw_Text(str_t, 135, 190, WHITE, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
void print_data_from_MS5611_on_lcd(void)
{
	if(i2c_device.MS5611_ready_status)
	{
		ms5611_measure();

		char str_MS5611[30] = {0};
		char str_MS5611_buff[13] = {0};

		int temp_MS5611 = i2c_device.MS5611_temperature;
		int pres_MS5611 = i2c_device.MS5611_pressure;

		char str_t_MS5611[6] = " C  ";
		char str_p_MS5611[6] = " P";

		itoa(temp_MS5611, str_MS5611_buff, 10);
		strcat(str_MS5611, str_MS5611_buff);
		strcat(str_MS5611, str_t_MS5611);

		itoa(pres_MS5611, str_MS5611_buff, 10); 	// Convert from int in str
		strcat(str_MS5611, str_MS5611_buff);
		strcat(str_MS5611, str_p_MS5611);

		ILI9341_Draw_Text(str_MS5611, 135, 170, WHITE, 2, BLACK);
	}
}
// -----------------------------------------------------------------------
void print_data_from_MPU6050_Acc_and_Gyro_on_lcd(void)
{
	bool print_x_y_z_status = false;

	if(i2c_device.MPU6050_ready_status == true)
	{
		if(print_x_y_z_status == false)
		{
			// Print x.y and z
			char str_x[3] = "X:";
			char str_y[3] = "Y:";
			char str_z[3] = "Z:";

			// Acc
			ILI9341_Draw_Text(str_x, 135, 115, WHITE, 1, BLACK);
			ILI9341_Draw_Text(str_y, 190, 115, WHITE, 1, BLACK);
			ILI9341_Draw_Text(str_z, 250, 115, WHITE, 1, BLACK);

			// Gyro
			ILI9341_Draw_Text(str_x, 135, 135, WHITE, 1, BLACK);
			ILI9341_Draw_Text(str_y, 190, 135, WHITE, 1, BLACK);
			ILI9341_Draw_Text(str_z, 250, 135, WHITE, 1, BLACK);

			print_x_y_z_status = true;
		}

		mpu6050_measure();

		char str_MPU6050[6] = {0};

		// Acceleration
		gcvt(i2c_device.MPU6050_acceleration_Ax, 2, str_MPU6050 );
		ILI9341_Draw_Text("       ", 145, 115, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 145, 115, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));

		gcvt(i2c_device.MPU6050_acceleration_Ay, 2, str_MPU6050 );
		ILI9341_Draw_Text("        ", 200, 115, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 200, 115, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));

		gcvt(i2c_device.MPU6050_acceleration_Az, 2, str_MPU6050 );
		ILI9341_Draw_Text("        ", 260, 115, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 260, 115, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));

		// Gyroscope
		gcvt(i2c_device.MPU6050_gyro_Gx, 2, str_MPU6050 );
		ILI9341_Draw_Text("       ", 145, 135, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 145, 135, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));

		gcvt(i2c_device.MPU6050_gyro_Gy, 2, str_MPU6050 );
		ILI9341_Draw_Text("        ", 200, 135, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 200, 135, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));

		gcvt(i2c_device.MPU6050_gyro_Gz, 2, str_MPU6050 );
		ILI9341_Draw_Text("        ", 260, 135, WHITE, 1, BLACK);
		ILI9341_Draw_Text(str_MPU6050, 260, 135, WHITE, 1, BLACK);
		memset(str_MPU6050, 0 , sizeof(str_MPU6050));
	}
}
// -----------------------------------------------------------------------
void print_data_from_BME280_on_lcd(void)
{
	if(i2c_device.BME280_ready_status == true)
	{
		bme280_measure();

		char str_t_BME280[20] = {0};
		char str_h_BME280[20] = {0};
		char str_p_BME280[20] = {0};

		int temp = i2c_device.BME280_temperature;
		int hum = i2c_device.BME280_humidity;
		int pres = i2c_device.BME280_preasure;

		itoa(temp, str_t_BME280, 10);
		itoa(hum, str_h_BME280, 10);
		itoa(pres, str_p_BME280, 10);

		char cels_s_BME280[7] = " C ";
		char hum_s_BME280[4] = "% ";
		char pres_s_BME280[4] = "P";

		strcat(str_t_BME280, cels_s_BME280);
		strcat(str_h_BME280, hum_s_BME280);
		strcat(str_p_BME280, pres_s_BME280);

		strcat(str_t_BME280, str_h_BME280);
		strcat(str_t_BME280, str_p_BME280);

		ILI9341_Draw_Text(str_t_BME280, 135, 90, WHITE, 2, BLACK);
	}
}
// -----------------------------------------------------------------------








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
// print debug text on LCD
void Generic_Write(const char* Text)		// Print "Text" data on LCD
{
	if (Text)
	{
		ILI9341_Draw_Text( "                                 ", 10, 60, WHITE, 1, BLACK);
		ILI9341_Draw_Text( Text, 10, 60, WHITE, 1, BLACK);
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
	if ((NewMenu == &NULL_MENU) || (NewMenu == NULL))
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
//------------------------------------------------------------------------------
