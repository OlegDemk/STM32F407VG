/*
 * main_sensor_file.c
 *
 *  Created on: May 27, 2021
 *      Author: odemki
 */

#include "main.h"

#include "sensors/main_sensor_file.h"

#include "screens/oled/oled.h"
#include "screens/oled/gfx.h"

#include "sensors/mpu6050.h"
#include <sensors/hmc5883l.h>

#include "sensors/bme280_defs.h"
#include "sensors/bme280.h"

#include "sensors/ms5611.h"

#include "sensors/apds9960.h"


extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;


//MPU6050_t MPU6050;
// Functions for detect i2c devices
void detect_bme280(void );
void detect_mpu6050(void );
void detect_apds9960(void );
void denect_hmc5883l(void );
void detect_ms5611(void );
void detect_oled_screen(void );
void detect_ds3231(void );


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// BME280 part
struct bme280_dev dev;
struct bme280_data comp_data;
int8_t rslt;

int8_t init_bme280(void);
void bme280_measure(void);

//----------------------------------------------------------------------------------------
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if(HAL_I2C_Master_Transmit(&hi2c3, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
  if(HAL_I2C_Master_Receive(&hi2c3, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

  return 0;
}
//----------------------------------------------------------------------------------------
void user_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}
//----------------------------------------------------------------------------------------
int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);

  if(HAL_I2C_Master_Transmit(&hi2c3, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

  free(buf);
  return 0;
}
// End BME280 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// MPU6050 part
int8_t init_mpu6050(void);
void mpu6050_measure(void);

MPU6050_t MPU6050;
// End MPU6050 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
// MPU6050 part
void init_ms5611(void);
void ms5611_measure(void);
// End MPU6050 part
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

void am2302_measure(void);

//
//struct {
//	// Sensors
//	bool BME280_ready_status;		// Temperature, Humidity and pleasure
//	bool MPU6050_ready_status; 		// IMU acceleration and gyroscope
//	bool HMC5883L_ready_status; 	// Magnetometer
//	bool MS5611_ready_status;		// Reassure
//	bool APDS9960_ready_status; 	// Motion sensor
//
//	bool AM2302_ready_status;
//
//	// Clock
//	bool OLED_ready_status;			// OLED screen
//	bool DS3231_ready_status; 		// RTC Clock module
//
//	// variable for data from i2c device
//	float  BME280_temperature;
//	float  BME280_humidity;
//	float  BME280_preasure;
//
//	// Data from MPU6050
//	double MPU6050_acceleration_Ax;
//	double MPU6050_acceleration_Ay;
//	double MPU6050_acceleration_Az;
//	double MPU6050_gyro_Gx;
//	double MPU6050_gyro_Gy;
//	double MPU6050_gyro_Gz;
//	float MPU6050_temperature;
//
//	// Data from MS5611
//	double MS5611_temperature;
//	double MS5611_pressure;
//
//	// Data from AM2302
//	float AM2302_temperature;
//	float AM2302_humidity;
//
//	// Data from APDS9960
//	bool APDS9960_Gesture_UP;
//	bool APDS9960_Gesture_DOWN;
//	bool APDS9960_Gesture_RIGHT;
//	bool APDS9960_Gesture_LEFT;
//	// ..... some another data fron APDS9960
//
//
//
//
//}i2c_device;


//----------------------------------------------------------------------------------------
void detect_all_sensors_and_init(void)
{
	detect_bme280();
	if(i2c_device.BME280_ready_status == true)
	{
		init_bme280();
	}

	detect_mpu6050();
	if(i2c_device.MPU6050_ready_status == true)
	{
		init_mpu6050();
	}


	detect_ms5611();
	if(i2c_device.MS5611_ready_status == true)
	{
		init_ms5611();
	}

	init_am2302();			// Init AM2302 sensor

	//////////////////////////////////////////////////////////////
	// DON'T WORK
	denect_hmc5883l();								// Don't work

	detect_apds9960();
	if(i2c_device.APDS9960_ready_status == true)
	{
		//init_apds9960();
	}
	///////////////////////////////////////////////////////////////
	detect_oled_screen();
	detect_ds3231();

}
//---------------------------------------------------------------------------------------
// Measure one time
void measure_sensors(void)
{
//	while(1)
//	{

		bme280_measure();
		mpu6050_measure();
		ms5611_measure();

		bool am2302_measure_ok = true;
		if(am2302_measure_ok)
		{
			HAL_Delay(1000);
			am2302_measure();	// Measure must be less than one time per 2-3 seconds
		}


		//	apds9960();   DON't work !!!!
		//HAL_Delay(1000);
//	}



}


//----------------------------------------------------------------------------------------
void init_apds9960(void)
{
	//////////////////////////////////////////////////////////////////////////////////////
	if(apds9960init() != true)
	{
		// Init error !!
		while(1)
		{
			int j = 0;
		}
	}
	else
	{
		if(enableGestureSensor(true) != true)
		{
			// Init error !!
			while(1)
			{
				int j = 0;
			}
		}
	}

//    while(1)            // For debug
//    {
    	int gesture = 0;
    	HAL_Delay(100);
    	gesture = apds9960ReadSensor();

    	detect_apds9960();
//    }
    //////////////////////////////////////////////////////////////////////////////////////



	// From  https://habr.com/ru/post/423847/     DON'T WORK //////////////////////
//	i2c1_write(APDS9960_CONTROL, DEFAULT_PGAIN);
//	i2c1_write(APDS9960_GPENTH, DEFAULT_GPENTH);
//	i2c1_write(APDS9960_GEXTH, DEFAULT_GEXTH);
//	i2c1_write(APDS9960_GCONF2, DEFAULT_GGAIN);
//	//i2c1_write(APDS9960_GPULSE, DEFAULT_PULSE_LENGTH);  // DEFAULT_GPULSE
//	//i2c1_write(APDS9960_PPULSE, DEFAULT_PULSE_LENGTH);
//
//	HAL_Delay(200);
//	//GesturesSet(1);
//
//
//
//	while(1)
//	{
//		i2c1_write(APDS9960_ENABLE, APDS9960_GEN | APDS9960_PEN | APDS9960_PON);
//
//		uint8_t GestureUp = 0;
//		uint8_t GestureDown = 0;
//		uint8_t GestureLeft = 0;
//		uint8_t GestureRight = 0;
//
//		uint8_t GestUpDown = 0;
//		uint8_t GestLeftRight = 0;
//
//		uint8_t GSTATUS_buf = 0;
//		uint8_t GFLVL_buf = 0;
//
//		GSTATUS_buf = i2c1_read(0xAF);
//
//		if(GSTATUS_buf & APDS9960_GVALID)
//		{
//			GFLVL_buf = i2c1_read(APDS9960_GFLVL);
//			if(GFLVL_buf)
//			{
//				GestureUp = i2c1_read(APDS9960_GFIFO_U);
//				GestureDown = i2c1_read(APDS9960_GFIFO_D);
//				GestureLeft = i2c1_read(APDS9960_GFIFO_L);
//				GestureRight = i2c1_read(APDS9960_GFIFO_R);
//
//							//Truth table:
//							//UP: 	 GestUpDown(+) | GestLeftRight(+)
//							//DOWN:  GestUpDown(-) | GestLeftRight(-)
//							//LEFT:  GestUpDown(+) | GestLeftRight(-)
//							//RIGHT: GestUpDown(-) | GestLeftRight(+)
//
//				GestUpDown = GestureUp-GestureDown;
//				GestLeftRight = GestureLeft-GestureRight;
//
//				i2c1_write(APDS9960_ENABLE, APDS9960_PEN | APDS9960_PON);
//
//			}
//		}
//
//	}
	//////////////////////////////////////////////////////////////////////////////////////

//	while(1)
//	{
////		if(external_interrupt_is_set)
////		{
////
////		}
////		else
////		{
////			// nothing
////		}
//
//	}



//	while(1)
//	{
//		int gesture = 0;
//		gesture = apds9960ReadSensor();  // <----------------------------------
//		HAL_Delay(300);
//	}

}
//----------------------------------------------------------------------------------------
void apds9960_measure(void)
{

}

//----------------------------------------------------------------------------------------
/*
 * Function make us delay
 */
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
	uint32_t test_micros = SystemCoreClock;
	micros *= (SystemCoreClock / 100000) /84;
	while (micros--);
}
//----------------------------------------------------------------------------------------
void init_am2302(void)
{
	GPIOC->MODER |= GPIO_MODER_MODER1_0;            // Output mode GPIOC0
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_1;             // Push-pull mode
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1;     // Speed
	GPIOC->ODR ^= 0x02; 							// set GPIOC pin 1 on high
	HAL_Delay(2000); 							// First init must be 2 seconds delay
	am2302_measure(); 							// For fill in i2c_device.AM2302_ready_status
}
//----------------------------------------------------------------------------------------
void am2302_measure(void)
{
	//  function must use less than one time per 2-3 seconds.
/* Init work with sensor:
 *
 * From microcontroller
 * 						            From sensor
 * 	   Low 10 msec	      High 39 us|	80us Pull down     80us Pull up 	Start Receive data from sensor
 * ____                     _______	|					 __________________
 * 	   \	   	           /	   \|					/				   \
 * 	   	\_________________/			\__________________/		 			\_______
 *
 * Receive '0' Bit
 *     Low 50 us    High 26 - 28 us
 * __                ___________
 * 	 \			    /			\
 * 	  \____________/			 \_
 *
 * Receive '1' Bit
 *     Low 50 us             High 70 us
 * __                ________________________
 * 	 \			    /				         \
 * 	  \____________/			              \_
 */

	bool get_data_status = false;
	int j = 0;   							// Counter bytes
	int i = 0;								// Counter bits
	uint8_t data[4] = {0};					// Buffer for incoming data from sensor
	float temper, hum;						// Buffer variables

	// Init GPIO like output
	GPIOC->MODER |= GPIO_MODER_MODER1_0;            // Output mode GPIOC0
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_1;             // Push-pull mode
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1;     // Speed

	// Make output pin C1
	GPIOC->ODR &= ~0x02;		// Low level
	DelayMicro(18000);
	GPIOC->ODR ^= 0x02;			// High level
	DelayMicro(39);

	// Make input pin C1
	GPIOC->MODER &= ~0x04;  	// Set Pin C1 Input   (MODER GPIOC_1 Must be 00)
	GPIOC->PUPDR &= ~0x04;		// Set Pin C1 Pull up

	if(GPIOC->IDR & GPIO_IDR_ID1)		// Sensor must pull down
	{
		get_data_status = false; 					// Error. Sensor not response
	}
	else
	{
		get_data_status = true;
	}

	DelayMicro(80);
	if(!(GPIOC->IDR & GPIO_IDR_ID1))  	// Sensor must pull up
	{
		get_data_status = false; 					// Error. Sensor not response
	}
	else
	{
		get_data_status = true;
	}
	DelayMicro(80);

	if(get_data_status == true)
	{
		for(j = 0; j <5; j++)							// Reading 5 bytes
		{
			data[4-j] = 0;
			for(i = 0; i < 8; i++)						// Reading 8 bits
			{
				while(!(GPIOC->IDR & GPIO_IDR_ID1));	// While signal is "0"
				DelayMicro(30);
				if(GPIOC->IDR & GPIO_IDR_ID1)			// If signal is high when wrute "1" in buffer (data[])
				{
					data[4-j] |= (1 << (7 - i));        // Shift received bite
				}
				while(GPIOC->IDR & GPIO_IDR_ID1);		// Wait end of "1" signal
			}
			get_data_status = true;										// Data was been written okay
		}

		temper = (float)((*(uint16_t*)(data+1)) & 0x3FFF) /10;
		if((*(uint16_t*)(data+1)) & 0x8000) temper  *= -1.0;

		i2c_device.AM2302_temperature = temper;

		hum = (float)(*(int16_t*)(data+3)) / 10;
		i2c_device.AM2302_humidity = hum;

		i2c_device.AM2302_ready_status = true;
	}
	else
	{
		i2c_device.AM2302_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void init_ms5611(void)
{
	ms5611_set_i2c(&hi2c2);
	ms5611_init();
}
//----------------------------------------------------------------------------------------
void ms5611_measure(void)
{
	ms5611_update();

	i2c_device.MS5611_temperature = ms5611_get_temperature();
	i2c_device.MS5611_pressure = ms5611_get_pressure();
}
//----------------------------------------------------------------------------------------
int8_t init_mpu6050(void)
{
	if(MPU6050_Init(&hi2c2) == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
//----------------------------------------------------------------------------------------
void mpu6050_measure(void)
{
	MPU6050_Read_All(&hi2c2, &MPU6050);

	i2c_device.MPU6050_acceleration_Ax = MPU6050.Ax;
	i2c_device.MPU6050_acceleration_Ay = MPU6050.Ay;
	i2c_device.MPU6050_acceleration_Az = MPU6050.Az;

	i2c_device.MPU6050_gyro_Gx = MPU6050.Gx;
	i2c_device.MPU6050_gyro_Gy = MPU6050.Gy;
	i2c_device.MPU6050_gyro_Gz = MPU6050.Gz;

	i2c_device.MPU6050_temperature = MPU6050.Temperature;
}
//----------------------------------------------------------------------------------------
int8_t init_bme280(void)
{
	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_ms = user_delay_ms;

	rslt = bme280_init(&dev);

	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);

	//	  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);

	dev.delay_ms(40);
}
//----------------------------------------------------------------------------------------
void bme280_measure(void)
{
	rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);

	if(rslt == BME280_OK)
	{
		// Save data in main structure
		i2c_device.BME280_temperature = comp_data.temperature;
		i2c_device.BME280_humidity = comp_data.humidity;
		i2c_device.BME280_preasure = comp_data.pressure;
		//temperature = comp_data.temperature / 100.0;      /* ??C  */
		//humidity = comp_data.humidity / 1024.0;           /* %   */
		//pressure = comp_data.pressure / 10000.0;          /* hPa */
	}
}
//----------------------------------------------------------------------------------------




























// Function for detect i2c devices ////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Temperature, humidity and pressure sensor
void detect_bme280(void)
{
	uint16_t STATUS=0;
	uint16_t addres_device = 0x76;  		 	// BME280
	uint16_t id_addr = 0xD0;
	uint8_t id = 96;							// in hex form
	uint8_t buff=0;        						// Return 0x96 -> Dec 60

	STATUS = HAL_I2C_Mem_Read(&hi2c3, addres_device<<1, id_addr, 1, &buff, 1, 1000);
	//HAL_OK == 0
	if((buff == id) && (STATUS == 0))
	{
		i2c_device.BME280_ready_status = true;
	}
	else
	{
		i2c_device.BME280_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
// 1. IMU Module. Measure Acceleration X, Y, Z and Gyroscope X, Y, Z.
// 2. Turn on ability work with  hmc5883l ( Magnetometer  sensor ).
void detect_mpu6050(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0xD0;
	uint16_t id_addr = 0x75;
	uint8_t id = 104;							// in hex form
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, id_addr, 1, &buff, 1, 1000);

	if((buff == id) && (STATUS == 0))
	{
		i2c_device.MPU6050_ready_status = true;
	}
	else
	{
		i2c_device.MPU6050_ready_status = false;
	}

	// turn on direct i2c communication with hmc5883l
	if(i2c_device.MPU6050_ready_status == true)
	{
		uint8_t data = 0x00;
		STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x6A, 1, &data, 1, 1000);		// Disable i2c master mode
		data = 0x02;
		STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x37, 1, &data, 1, 1000);		// Enable i2c master bypass mode

		// Check waited registers
		buff = 0;
		STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, 0x6A, 1, &buff, 1, 1000);
		buff = 0;
		STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device, 0x37, 1, &buff, 1, 1000);

		if(STATUS == 0)
		{
			denect_hmc5883l();     // Don't work  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
	}
}
//----------------------------------------------------------------------------------------
void denect_hmc5883l(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x1E;       //0x1E                		 ///// 0x3C
	uint16_t addr = 0x0A;				// HMC5883L_REG_IDENT_A      ///// 0xA0
	//uint8_t id = 0xa8;							// in hex form
	uint8_t buff = 0;

	//STATUS = HAL_I2C_Mem_Write(&hi2c2, addres_device, 0x37, 1, &data, 1, 1000);

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1 , addr, 1, &buff, 1, 1000);
	if(STATUS == 0)
	{
		i2c_device.HMC5883L_ready_status = true;
	}
	else
	{
		i2c_device.HMC5883L_ready_status = false;
	}

//	HMC5883L_setRange(HMC5883L_RANGE_1_3GA);
//	HMC5883L_setMeasurementMode(HMC5883L_CONTINOUS);
//	HMC5883L_setDataRate(HMC5883L_DATARATE_15HZ);
//	HMC5883L_setSamples(HMC5883L_SAMPLES_1); HMC5883L_setOffset(0, 0);
//
//	Vector mag = HMC5883L_readRaw();

}
//----------------------------------------------------------------------------------------
void detect_ms5611(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x77;
	uint16_t id_addr = 0x00 ;
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if(STATUS == 0)
	{
		i2c_device.MS5611_ready_status = true;
	}
	else
	{
		i2c_device.MS5611_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_apds9960(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x39;
	uint16_t id_addr = 0x92;
	uint8_t id = 0xa8;							// in hex form
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c2, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if((buff == id) && (STATUS == 0))
	{
		i2c_device.APDS9960_ready_status = true;
	}
	else
	{
		i2c_device.APDS9960_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_oled_screen(void)
{
	uint16_t STATUS = 0;
	uint8_t buffer[2];
	buffer[0] = 0x00;
	buffer[1] = 0xAE;

	STATUS = HAL_I2C_Master_Transmit(&hi2c3, 0x78,buffer, 2, 1000);

	if(STATUS == 0)
	{
		i2c_device.OLED_ready_status = true;
	}
	else
	{
		i2c_device.OLED_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------
void detect_ds3231(void)
{
	uint16_t STATUS = 0;
	uint16_t addres_device = 0x68;
	uint16_t id_addr = 0x00;						// Read seconds
	uint8_t buff = 0;

	STATUS = HAL_I2C_Mem_Read(&hi2c3, addres_device<<1, id_addr, 1, &buff, 1, 1000);

	if(STATUS == 0)
	{
		i2c_device.DS3231_ready_status = true;
	}
	else
	{
		i2c_device.DS3231_ready_status = false;
	}
}
//----------------------------------------------------------------------------------------


















	    //




