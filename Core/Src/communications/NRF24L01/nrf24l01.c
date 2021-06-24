/*
 * nrf24l01.c
 *
 *  Created on: Jun 11, 2021
 *      Author: odemki
 */

#include "communications/NRF24L01/nrf24l01.h"

extern SPI_HandleTypeDef hspi1;

#define TX_ADR_WIDTH 3
#define TX_PLOAD_WIDTH 5   // 2
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xb3,0xb4,0x01};
uint8_t RX_BUF[TX_PLOAD_WIDTH] = {0};

/*
char str1[20]={0};
uint8_t buf1[20]={0};
*/

char str1[150]={0};
uint8_t buf1[20]={0};

uint8_t ErrCnt_Fl = 0; 		// Error counter

void test_nrf(void);

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
uint8_t NRF24_ReadReg(uint8_t addr)
{
  uint8_t dt=0, cmd;
  CS_ON;
  HAL_SPI_TransmitReceive(&hspi1,&addr,&dt,1,1000);
  if (addr!=STATUS_NRF)
  {
	  cmd=0xFF;
	  HAL_SPI_TransmitReceive(&hspi1,&cmd,&dt,1,1000);
  }
  CS_OFF;
  return dt;
}
//----------------------------------------------------------------------------------------
void NRF24_WriteReg(uint8_t addr, uint8_t dt)
{
  addr |= W_REGISTER;								// Add write bit
  CS_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);	// Send address in bus
  HAL_SPI_Transmit(&hspi1,&dt,1,1000);				// Send data in bus
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24_ToggleFeatures(void)
{
  uint8_t dt[1] = {ACTIVATE};
  CS_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  dt[0] = 0x73;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{
  CS_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);			// Send address in bus
  HAL_SPI_Receive(&hspi1,pBuf,bytes,1000);			// Save data in buffer
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{
  addr |= W_REGISTER;								//Add write bit
  CS_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);			// Send address in bus
  DelayMicro(1);
  HAL_SPI_Transmit(&hspi1,pBuf,bytes,1000);			// Send data in buffer
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24_FlushRX(void)
{
  uint8_t dt[1] = {FLUSH_RX};
  CS_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24_FlushTX(void)
{
  uint8_t dt[1] = {FLUSH_TX};
  CS_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  CS_OFF;
}
//----------------------------------------------------------------------------------------
void NRF24L01_RX_Mode(void)
{
  uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);			// 0x0B  -> 0b 0000 1010
  if((regval != 0x08) | (regval != 0x0B) )
  {
	  // Error
	  int fuc = 99;
  }

  regval |= (1<<PWR_UP) | (1<<PRIM_RX);    // Power up module. Write PWR_UP and PRIM_RX bits

  // regval = 0x7B;
  NRF24_WriteReg(CONFIG,regval);
  // Debug read CONFIG register   //////////////////////////////////////////////////
  NRF24_WriteReg(CONFIG, 0x33);     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< IRQ WORK  ~ 130 us
  //NRF24_WriteReg(CONFIG, 0x7B);   my test


//
//  regval = NRF24_ReadReg(RF_SETUP);
//  NRF24_WriteReg(RF_SETUP,0x00);     // set -16 dBm <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<,
  ////////////////////////////////////////////////////////////////////////////////////
  CE_SET;
  DelayMicro(150); // Delay 130 us
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}
//----------------------------------------------------------------------------------------
// Function waite Low IRQ signal from NRF module
void NRF24L01_Receive(void)
{
  uint8_t status=0x01;
  uint16_t dt_1=0;
  uint16_t dt_2=0;
  // NO IRQ fFROM NRF  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  while((GPIO_PinState)IRQ == GPIO_PIN_SET) {}	// Waiting interrupt from IRQ
//  while(HAL_GPIO_ReadPin(IRQ_GPIO_PORT, IRQ_nrf_Pin) != GPIO_PIN_RESET)
//  {
//	  int g= 99;
//	  g++;
//  }
  status = NRF24_ReadReg(STATUS_NRF);
//  sprintf(str1,"STATUS: 0x%02X\r\n",status);
//  HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);

//  LED_TGL;
  DelayMicro(10);
  status = NRF24_ReadReg(STATUS_NRF);
  if(status & 0x40)		//	Флаг надходження нових даних в FIFO  (Check RX_DR flag)
   {
      NRF24_Read_Buf(RD_RX_PLOAD,RX_BUF,TX_PLOAD_WIDTH);
      dt_1 = *(int16_t*)RX_BUF;
      //Clear_7219();
      //Number_7219(dt);
      dt_2 = *(int16_t*)(RX_BUF+2);
     // NumberL_7219(dt);
      NRF24_WriteReg(STATUS_NRF, 0x40);		// Turn off interrupt
    }
}
//----------------------------------------------------------------------------------------
void NRF24_ini(void)
{
	CE_RESET;
	DelayMicro(5000);

	NRF24_WriteReg(CONFIG, 0x0a); // Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter) 0b 0000 1010

	DelayMicro(5000);

	NRF24_WriteReg(EN_AA, 0x02); 			// Enable Pipe1
	NRF24_WriteReg(EN_RXADDR, 0x02); 		// Enable Pipe1
	NRF24_WriteReg(SETUP_AW, 0x01); 		// Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x5F); 		// 1500us, 15 retrans
	NRF24_ToggleFeatures();					// Send activated command
	NRF24_WriteReg(FEATURE, 0);				// Turn off all FEATURE register
	NRF24_WriteReg(DYNPD, 0); 				// Turn off all payload length data pipe
	NRF24_WriteReg(STATUS_NRF, 0x70); 		// Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); 				// Frequency = 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); 					// 0x06 //TX_PWR:0dBm, Datarate:1Mbps

	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_WriteReg(RX_PW_P1, TX_PLOAD_WIDTH);				 //Number of bytes in RX
	NRF24L01_RX_Mode();

	//test_nrf();
}
//----------------------------------------------------------------------------------------
// Read config data from nrf registers
void test_nrf(void)
{
	uint8_t dt_reg=0;

	HAL_Delay(1);

	uint8_t CONFIG_reg_dump = NRF24_ReadReg(CONFIG);			// 0x0B
	uint8_t EN_AA_reg_dump = NRF24_ReadReg(EN_AA);			    // 0x02
	uint8_t EN_RXADDR_reg_dump = NRF24_ReadReg(EN_RXADDR); 		// 0x02
	uint8_t STATUS_NRF_reg_dump = NRF24_ReadReg(STATUS_NRF);    // 0x0E
	uint8_t RF_SETUP_reg_dump = NRF24_ReadReg(RF_SETUP);		// 0x06

	uint8_t TX_ADDR_dump[10] = {0};
	uint8_t RX_ADDR_dump[10] = {0};

	NRF24_Read_Buf(TX_ADDR,TX_ADDR_dump,3);						// 0xb3,0xb4,0x01
	NRF24_Read_Buf(RX_ADDR_P1,RX_ADDR_dump,3);					// 0xb3,0xb4,0x01

	dt_reg = 0;  // For debug
}



void nrf_communication_test(void)
{
	NRF24_ini();
	test_nrf();

	while(1)
	{
		NRF24L01_Receive();

	}

}
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------


























































//----------------------------------------------------------------------------------------
