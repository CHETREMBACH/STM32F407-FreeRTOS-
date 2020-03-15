/**
 ******************************************************************************
 * @file    cmd_hdl.c
 * @author  Trembach D.N.
 * @version V1.2.0
 * @date    07-10-2019
 * @brief   файл  функций обработчиков команд  
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
 ******************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/
#include "cmd_hdl.h"
#include "cmd_hlp.h"

extern void delay_us(uint32_t time_val);

/**
  * @brief  обработчик команды програмного перезапуска контроллера
  * @param  cmd_parametr_t *parametr - указатель на параметр команды
  * @retval uint16_t - возвращаемое значение
  */
uint16_t reset_cmd_handler(cmd_parametr_t *parametr)
{
  /* Вывод информации о том что сейчас будет програмный перезапуск */
  printf("The microcontroller will restart after 1 second.\r\n");
  /* Пауза на время выдачи информации в терминад */
  for (uint32_t cnt_time = 0; cnt_time < 10000000; cnt_time++)
  {
    __asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");
    __asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");
    __asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");
    __asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");
    __asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");__asm volatile ("nop");
    
  }
  /* Сброс микроконтроллера */
  NVIC_SystemReset();
  return 0;					
}

/*================= команда помощи =============================*/
const cmd_t reset_cmd = 
{ 
  /* имя ( мнемомика ) команды        */   
  "reset",
  /* тип параметра команды            */
    VR_NONE, VR_NONE, VR_NONE, VR_NONE, VR_NONE,
  /* краткое описание команды         */
  "controller restart command",
  /* расширенное описание команды     */ 
  "the command is intended for soft restarting the microcontroller.\r\n  command format: >reset <NONE> \r\n",
  /* указатель на обработчик команд   */    		
  reset_cmd_handler
};

/**
  * @brief  Инициализация указателя на команду управления сбросом микроконтроллера
  * @param  none
  * @retval none
  */
void reset_cmd_init()
{
   add_terminal_cmd(&reset_cmd);   
}

/**
 * @brief  обработчик команды установки режима светодиода
 * @param  cmd_parametr_t *parametr - указатель на параметр команды
 * @retval uint16_t - возвращаемое значение
 */
uint16_t led_cmd_handler(cmd_parametr_t *parametr)
{
  if (parametr != NULL)
  {
    if (strcmp(parametr->var_ch, "on") == 0)
    {
      //LED_LO;
      printf("LED is on\r\n"); 
      return 0;	
    }
    
    if (strcmp(parametr->var_ch, "off") == 0)    
    {  
      //LED_HI;
      printf("LED is off\r\n");
      return 0;	
    }
  }    
  
  printf("Invalid parameter\r\n");    
  return 0;					
}

/*================= команда помощи =============================*/
const cmd_t led_cmd = 
{ 
  /* имя ( мнемомика ) команды        */   
  "led",
  /* тип параметра команды            */
    VR_STRING, VR_NONE, VR_NONE, VR_NONE, VR_NONE,
  /* краткое описание команды         */
  "LED control command",
  /* расширенное описание команды     */ 
  "the command is intended to set the LED operating mode.\r\n  command format: >led <STRING> \r\n   on - LED is on, \r\n   off - LED is off.",
  /* указатель на обработчик команд   */    		
  led_cmd_handler
};

/**
  * @brief  Инициализация указателя на команду управления светодиодом
  * @param  none
  * @retval none
  */
void led_cmd_init()
{
   add_terminal_cmd(&led_cmd);   
}

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/