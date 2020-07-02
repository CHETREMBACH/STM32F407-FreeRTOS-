/**
  ******************************************************************************
  * @file    poe_cntrl_auto.h
  * @author  Трембач Д.Н., Дэйта Экспресс
  * @version V2.0.0
  * @date    Создан:  11.06.2020
  *   	    Изменен:  11.06.2020	
  * @brief   Модуль работы интерфейса poe контроль
  *          
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POE_CNTRL_AUTO_H
#define __POE_CNTRL_AUTO_H

#if MODE_CNTRL == 2

void init_poe_cntrl(void);
void irq_poe_cntrl(void);
void period_poe_cntrl(void);

#endif /* #if MODE_CNTRL == 1 */ 

#endif /* __POE_CNTRL_H */
