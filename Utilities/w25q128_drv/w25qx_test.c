/**
 ******************************************************************************
 * @file    w25qx_test.c
 * @version V1.0.0
 * @date    08-03-2020
 * @brief   Файл c функциями тестирования драйвера w25qx
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "conf_w25qx.h"
#include "spi_w25qx_ll.h" 
#include "w25qx.h" 
#include "cmd_hlp.h"
#include "cmd_process.h"
#include "printf_dbg.h"

//time_temp = cyc_tick();
//          .....
//time_temp = cyc_tick() - time_temp;
//printf("time = %d\r\n", time_temp / 168);

/**
 * @brief  Заполнение памяти дампом
 * @param  uint8_t* buff_damp - указатель на буфер дампа
 * @retval none
 */
void GenDataDamp(uint8_t* buff_damp) {
  uint8_t num_box_data = 0;

  for (uint16_t block_cnt = 0; block_cnt < 16; block_cnt++) {
    for (uint16_t damp_cnt = 0; damp_cnt < 16; damp_cnt++) {
      buff_damp[(block_cnt * 16) + damp_cnt] = num_box_data++;
    }
  }
}

/**
 * @brief  Вывод дампа памяти на экран
 * @param  uint8_t* buff_damp - указатель на буфер дампа
 * @param  uint16_t index_damp - индекс старта выдачи данных
 * @retval uint16_t - число обработанных данных
 */
uint16_t DataDampPrint(uint8_t* buff_damp, uint16_t index_damp) {
  for (uint16_t contic = 0; contic < 32; contic++) {
    /* отступ между банками */
    if (contic == 16) printf("   ");
    /* Формирование строки HEX */
    printf(" %.2X", buff_damp[index_damp + contic]);
  }
  printf("\n");
  return 32;
}

/**
 * @brief  Функция визуализации содержимого блока памяти размером 256 байт
 * @param  uint8_t* buff_damp - указатель на буфер дампа
 * @param  uint16_t index_damp - индекс старта выдачи данных
 * @retval void
 */
void BlocDampPrint(uint8_t* buff_damp) {
  for (uint16_t damp_cnt = 0; damp_cnt < 8; damp_cnt++) {
    DataDampPrint(buff_damp, damp_cnt * 32);
  }
}

/**
 * @brief  обработчик команд тестирования 
 * @param  cmd_parametr_t *parametr - указатель на параметр команды
 * @retval uint16_t - возвращаемое значение
 */
uint16_t w25qx_cmd_handler(cmd_parametr_t *parametr) {
  if (parametr != NULL) {
    if (strcmp(parametr->var_ch, "info") == 0) {
      W25qxx_ChipInfo();
      printf("\r\n");
      return 0;
    }

    if (strcmp(parametr->var_ch, "status") == 0) {
      W25qxx_ChipStatus();
      printf("\r\n");
      return 0;
    }
  }

  printf("Invalid parameter\r\n");
  return 0;
}


/* массив для тестирования */
uint8_t damp_test_page[2560];

/**
 * @brief  обработчик команд тестирования чтения
 * @param  cmd_parametr_t *parametr - указатель на параметр команды
 * @retval uint16_t - возвращаемое значение
 */
uint16_t w25qx_rd_cmd_handler(cmd_parametr_t* parametr) {

  printf("\r\n Read page of the address = %d\r\n", parametr[0].var_u16);
  /* function of read page of the address */
  W25qxx_ReadPage(damp_test_page, parametr[0].var_u16, parametr[1].var_u16, parametr[2].var_u16);
  /* Вывод дампа памяти на экран  */
  BlocDampPrint(damp_test_page);
  printf("\r\n");
  return 0;
}

/**
 * @brief  обработчик команд тестирования записи
 * @param  cmd_parametr_t *parametr - указатель на параметр команды
 * @retval uint16_t - возвращаемое значение
 */
uint16_t w25qx_wr_cmd_handler(cmd_parametr_t* parametr) {

  printf("\r\n Write page of the address = %d\r\n", parametr[0].var_u16); 
  /* Генерирование данных */
  GenDataDamp(damp_test_page);
  /* Вывод дампа памяти на экран  */
  BlocDampPrint(damp_test_page); 
  /* function of writing page at the address */
  W25qxx_WritePage(damp_test_page, parametr[0].var_u16, parametr[1].var_u16, parametr[2].var_u16);
  printf("\r\n");
  return 0;
}

/*================= структура команды =============================*/
const cmd_t w25qx_test_cmd = {
    /* имя ( мнемомика ) команды        */
    "w25qx",
    /* тип параметра команды            */
    VR_STRING, VR_NONE, VR_NONE, VR_NONE, VR_NONE,
    /* краткое описание команды         */
    "w25qx driver test command",
    /* расширенное описание команды     */
    "the command is designed to test the driver w25qx in various modes.\r\n  command "
    "format: >w25qx <STRING> \r\n   info - w25qx chip info, \r\n   status - w25qx status info.",
    /* указатель на обработчик команд   */
    w25qx_cmd_handler};

const cmd_t w25qx_rd_test_cmd = {
    /* имя ( мнемомика ) команды        */
    "rd_w25qx",
    /* тип параметра команды            */
    VR_UINT16, VR_UINT16, VR_UINT16, VR_NONE, VR_NONE,
    /* краткое описание команды         */
    "read dw25qx test command",
    /* расширенное описание команды     */
    "the command is reading from the w25qx page at.\r\n  "
    "command "
    "format: >rd_w25qx <uint32_t> \r\n   uint32_t addr start data damp \r\n",
    /* указатель на обработчик команд   */
    w25qx_rd_cmd_handler};

const cmd_t w25qx_wr_test_cmd = {
    /* имя ( мнемомика ) команды        */
    "wr_w25qx",
    /* тип параметра команды            */
    VR_UINT16, VR_UINT16, VR_UINT16, VR_NONE, VR_NONE,
    /* краткое описание команды         */
    "write dw25qx test command",
    /* расширенное описание команды     */
    "the command is writing from the w25qx page at.\r\n  "
    "command "
    "format: >wr_w25qx <uint32_t> \r\n   uint32_t addr start data damp \r\n",
    /* указатель на обработчик команд   */
    w25qx_wr_cmd_handler};

/**
 * @brief  Инициализация указателя на команды тестирования драйвера w25qx
 * @retval none
 */
void w25qx_cmd_init() 
{
    add_terminal_cmd(&w25qx_test_cmd); 
    add_terminal_cmd(&w25qx_rd_test_cmd); 
    add_terminal_cmd(&w25qx_wr_test_cmd); 
}
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/