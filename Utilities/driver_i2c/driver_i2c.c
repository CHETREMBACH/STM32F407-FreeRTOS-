 /**
  ******************************************************************************
  * @file    driver_i2c.c
  * @author  Trembach D.N.
  * @version V1.2.0
  * @date    07-10-2019
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "driver_i2c.h"

#define I2C_ADDRESS        0x30F

/* I2C handler declaration */
I2C_HandleTypeDef handle_i2c;
I2C_HandleTypeDef hi2c3;
GPIO_InitTypeDef GPIO_InitStruct = { 0 };

void Setup_I2C ( void )
{
  
	hi2c3.Instance = I2C3;
	hi2c3.Init.ClockSpeed = 100000;
	hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c3.Init.OwnAddress1 = 0;
	hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c3.Init.OwnAddress2 = 0;
	hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c3) != HAL_OK)
	{
		//Error_Handler();
	}	
		
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**I2C3 GPIO Configuration    
	PC9     ------> I2C3_SDA
	PA8     ------> I2C3_SCL 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_I2C3_CLK_ENABLE();
    
}
     
void init_I2C_Functions (void)
{
  Setup_I2C();
}

/**************************************************************************************************************************************************
*                                 I2C Write Routines                                                                                              *
**************************************************************************************************************************************************/
unsigned char tps_WriteI2CReg(unsigned int i2cAddress, unsigned char registerAddress, unsigned char writeValue)
{
	uint8_t buf[2] = { 0 };
	
	buf[0] = registerAddress;
	buf[1] = writeValue;	
	HAL_I2C_Master_Transmit(&hi2c3, i2cAddress, buf, 2, 100);	
	return I2C_SUCCESSFUL;
}

unsigned char tps_WriteI2CMultiple(unsigned int i2cAddress, unsigned char registerAddress, unsigned char *writeValues, unsigned char numWriteBytes)
{
	uint8_t buf[50] = { 0 };		
	
	for (uint8_t i2c_cntic = 1; i2c_cntic <= numWriteBytes; numWriteBytes++)
	{
		buf[i2c_cntic] = writeValues[i2c_cntic - 1];		
	}
	
	buf[0] = registerAddress;
	
	HAL_I2C_Master_Transmit(&hi2c3, i2cAddress, buf, numWriteBytes+1, 100);	
	return I2C_SUCCESSFUL;
}

/**************************************************************************************************************************************************
*                                 I2C Read Routines                                                                                               *
**************************************************************************************************************************************************/
unsigned char tps_ReadI2CReg(unsigned int i2cAddress, unsigned char registerAddress, unsigned char *readValue)
{
	uint8_t buf[2] = { 0 };
	
	buf[0] = registerAddress;
	HAL_I2C_Master_Transmit(&hi2c3, i2cAddress, buf, 1, 100);
	HAL_I2C_Master_Receive(&hi2c3, i2cAddress, readValue, 1, 100); 
	return I2C_SUCCESSFUL;
}

unsigned char tps_ReadI2CMultiple(unsigned int i2cAddress, unsigned char registerAddress, unsigned char *readValue, unsigned char numReadBytes)
{
	uint8_t buf[2] = { 0 };
	buf[0] = registerAddress;
	HAL_I2C_Master_Transmit(&hi2c3, i2cAddress, buf, 1, 100);
	HAL_I2C_Master_Receive(&hi2c3, i2cAddress, readValue, numReadBytes, 100);
	return I2C_SUCCESSFUL;
}
