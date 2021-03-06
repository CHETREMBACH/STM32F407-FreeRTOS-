/**
 ******************************************************************************
 * @file    cmd_process.c
 * @author  Trembach D.N.
 * @version V1.0.0
 * @date    28-09-2019
 * @brief   файл функций обработки запросов команд терминала
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>

#include "cmd_hdl.h"
#include "cmd_hlp.h"

/* Массив указателей на описание команд с указателями на обработчик */
cmd_t *array_cmd[MAX_SIZE_NUM_CMD];
/* число команд терминала */
uint8_t max_num_cmd = 0;

/* индекс загрузки команды/параметра команды */
uint8_t index_dat;
/* индекс текущего параметра  */
uint8_t index_param;
/* индекс текущей команды */
uint8_t index_cmd;
/* число ожидаемых параметров команды */
uint8_t num_cmd_param;
/* статус параметров команды */
stat_cmd_e status_cmd_param;


/* буфер данных для параметра команды */
char data_prm_cmd[MAX_SIZE_DATA];
/* массив обработанных параметров */
cmd_parametr_t ar_prm[MAX_PARAM];
/* автомат состояний для приема данных */
fsm_dat_prc_e fsm_dat_prc = FIND_CMD;

/**
 * @brief  функция поиска мнемомики в массиве команд
 * @param  none
 * @retval uint8_t - индекс команды, если 255 - нет совпадений по команде
 */
uint8_t scan_cmd(void) {
  /* Цикл по всем командам */
  for (uint8_t i = 0; i < max_num_cmd; i++) {
    if (strcmp(data_prm_cmd, array_cmd[i]->cmd_name) == 0) {
      /* Команда принята */
      status_cmd_param = STAT_NORM;
      /* есть совпадение */
      return i;
    }
  }
  /* нет совпадений по команде */
  return 255;
}

/**
 * @brief  функция подсчитывает число параметров в команде
 * @param  uint8_t indx_cmd - индекс команды
 * @retval uint8_t - число параметров в команде
 */
uint8_t get_num_cmd_param(uint8_t indx_cmd) {
  /* Цикл по всем командам */
  uint8_t num_param_cmd = 0;

  for (uint8_t i = 0; i < MAX_PARAM; i++) {
    if (array_cmd[indx_cmd]->cmd_type_parametr[i] == VR_NONE) {
      /* есть совпадение */
      return num_param_cmd;
    } else {
      num_param_cmd++;
    }
  }
  /* нет совпадений по команде */
  return num_param_cmd;
}

/**
 * @brief  функция конвертирования строки в параметр заданного типа с проверкой
 * @param  type_cmd_prmtr_e  cmd_type_parametr - тип параметра команды
 * @param  cmd_parametr_t *parametr - указатель на структуру параметра команды
 * @retval stat_cmd_e статус конвертирования параметра
 *    0 - успешное конвертирование
 *    1 - некорректные данные
 *    2 - переполнение
 *    3 - данные отсутствуют
 */
stat_cmd_e get_parametr_cmd(type_cmd_prmtr_e cmd_type_parametr,
                         cmd_parametr_t *parametr) {
  uint32_t temp_var_u32;
  int32_t temp_var_i32;

  /* предустановка флага данных - данные присутствуют */
  parametr->flag_def = 0;
  /* Проверка наличие данных для конвертирования */
  if (data_prm_cmd[0] == '\0') {
    /* Если задан тип с пустым параметром */
    if (cmd_type_parametr == VR_NONE) {
      /* успешное конвертирование*/
      /* данные для конвертирования отсутствуют - установить по умолчанию */
      parametr->flag_def = 1;
      return STAT_NORM;
    }
    /* данные для конвертирования отсутствуют - установить по умолчанию */
    parametr->flag_def = 1;
    return STAT_NORM;
  }

  switch (cmd_type_parametr) {
    case VR_UINT8:
    case VR_UINT16:
    case VR_UINT32:

      if (sscanf(data_prm_cmd, "%lu", &temp_var_u32) == 1) {
        /* Проверка для UINT8 */
        if (cmd_type_parametr == VR_UINT8) {
          if (temp_var_u32 <= 0xFF) {
            parametr->var_u08 = (uint8_t)temp_var_u32;
            /* успешное конвертирование*/
            return STAT_NORM;
          } else {
            /* переполнение */
            return STAT_OVERFLOW;
          }
        }

        /* Проверка для UINT16 */
        if (cmd_type_parametr == VR_UINT16) {
          if (temp_var_u32 <= 0xFFFF) {
            parametr->var_u16 = (uint16_t)temp_var_u32;
            /* успешное конвертирование*/
            return STAT_NORM;
          } else {
            /* переполнение */
            return STAT_OVERFLOW;
          }
        }

        /* Проверка для UINT32 уже выполнена при конвертировнии */
        parametr->var_u32 = temp_var_u32;
        /* успешное конвертирование*/
        return STAT_NORM;
      }
      /* некорректные данные */
      return STAT_INCORRECT_PARAM;

    case VR_INT8:
    case VR_INT16:
    case VR_INT32:

      if (sscanf(data_prm_cmd, "%li", &temp_var_i32) == 1) {
        /* Проверка для INT8 */
        if (cmd_type_parametr == VR_INT8) {
          if ((temp_var_i32 <= 127) && (temp_var_i32 >= -128)) {
            parametr->var_i08 = (int8_t)temp_var_i32;
            /* успешное конвертирование*/
            return STAT_NORM;
          } else {
            /* переполнение */
            return STAT_OVERFLOW;
          }
        }

        /* Проверка для INT16 */
        if (cmd_type_parametr == VR_INT16) {
          if ((temp_var_i32 <= 32767) && (temp_var_i32 >= -32768)) {
            parametr->var_i16 = (int16_t)temp_var_i32;
            /* успешное конвертирование*/
            return STAT_NORM;
          } else {
            /* переполнение */
            return STAT_OVERFLOW;
          }
        }
        /* Проверка для INT32 уже выполнена при конвертировнии */
        parametr->var_i32 = temp_var_i32;
        /* успешное конвертирование*/
        return STAT_NORM;
      }
      /* некорректные данные */
      return STAT_INCORRECT_PARAM;

    case VR_DOUBLE:
      if (sscanf(data_prm_cmd, "%la", &(parametr->var_dbl)) == 1) {
        /* успешное конвертирование*/
        return STAT_NORM;
      } else {
        /* некорректные данные */
        return STAT_INCORRECT_PARAM;
      }

    case VR_CHAR:
      /* Если второй символ - завершение строки */
      if (data_prm_cmd[1] == '\0') {
        parametr->var_ch[0] = data_prm_cmd[0];
        /* успешное конвертирование*/
        return STAT_NORM;
      } else {
        /* переполнение */
        return STAT_OVERFLOW;
      }

    case VR_STRING:
      /* Копирование данных */
      strncpy(parametr->var_ch, data_prm_cmd, (MAX_SIZE_DATA - 1));
      /* Установка завершающего символа ( на всякий случай ) */
      parametr->var_ch[(MAX_SIZE_DATA - 1)] = '\0';
      /* успешное конвертирование*/
      return STAT_NORM;

    case VR_NONE:
    default:
      /* некорректные данные */
      return STAT_INCORRECT_PARAM;
  }
}

/**
 * @brief  функция обработки полученного параметра
 * @param  none
 * @retval none
 */
void proc_parametr(void) {
  /* Проверка проверка индекса команды */ 
  if (index_cmd != 255) 
  {
    /* Конвертирование параметра */
    status_cmd_param = get_parametr_cmd(array_cmd[index_cmd]->cmd_type_parametr[index_param],
                         &(ar_prm[index_param]));
  } else { 
    /* Команда не распознана */
    status_cmd_param = STAT_INCORRECT_CMD;
  }
}

/**
 * @brief  функция предустановки параметров команды
 * @param  none
 * @retval none
 */
void preset_cmd(void) {
  /* Команда не загружена */
  status_cmd_param = STAT_CMD_NOT_LOADED;
  for (uint8_t i = 0; i < MAX_PARAM; i++) {
    ar_prm[i].flag_def = 1;
  }
  /* Обнуление индекса записи */
  index_dat = 0;
}

/**
 * @brief  функция обработки команд
 * @param  none
 * @retval none
 */
void command_processing() {

    switch (status_cmd_param) {
    case STAT_NORM: /*- успешное конвертирование*/
      /* Вызов функции обработчика */
      array_cmd[index_cmd]->cmd_handler_cb(ar_prm);
      break;

    case STAT_INCORRECT_PARAM: /*- некорректные данные     */
      printf("Неккоректные данные  \r\n\r\n");
      break;

    case STAT_OVERFLOW: /*- переполнение            */
      printf("Переполнение  \r\n\r\n");
      break;

    case STAT_DATA_IS_EMPTY: /*- данные отсутствуют      */
      printf("Данные отсутствуют  \r\n\r\n");
      break;

    case STAT_CMD_NOT_LOADED: /*- команда не загружена    */
      printf("Команда не загружена  \r\n\r\n");
      break;

    case STAT_INCORRECT_CMD: /*- неккоректная команда    */
      printf("Неккоректная команда  \r\n\r\n");
      break;

    default:
      break;
  }
}

/**
 * @brief  функция обработки терминальных данных
 * @param  char ch - входящий байт
 * @retval none
 */
void terminal_data_processing(char ch) {
  /**/
  switch (fsm_dat_prc) {
    case FIND_CMD:
      /* Поиск начала команды */
      if (ch == '>') {
        /* Найдено начало команды - начинаем загрузку */
        fsm_dat_prc = PRC_CMD;
        /* функция предустановки параметров команды */
        preset_cmd();
       }
      break;

    case PRC_CMD:
      /* Загрузка мнемомики команды */
      if ((ch == ' ') || (ch == 0x0D) || (ch == 0x0A) || (ch == '>')) {
        /* Установка завершающего символа */
        data_prm_cmd[index_dat] = 0;
        /* Предварительная обработка команды */
        index_cmd = scan_cmd();
        if (index_cmd != 255) {
          /* Запрос числа ожидаемых параметров */
          num_cmd_param = get_num_cmd_param(index_cmd);
        } else {
          /* Нет команды - нет параметров */
          num_cmd_param = 0;
        }

        /* Если найдено начало следующей команды */
        if (ch == '>') {
          if (index_cmd == 255) {
            /* Команда не распознана */
            status_cmd_param = STAT_INCORRECT_CMD;
          }
          /* функция обработки команд   */
          command_processing();
          /* Найдено начало команды - начинаем загрузку */
          fsm_dat_prc = PRC_CMD;
          /* функция предустановки параметров команды */
          preset_cmd();
        } else {
          /* Если найден символ завершения строки */
          if ((ch == 0x0D) || (ch == 0x0A)) {
            if (index_cmd == 255) {
              /* Команда не распознана */
              status_cmd_param = STAT_INCORRECT_CMD;
            }
            /* функция обработки команд   */
            command_processing();
            /* Включение режима загрузки команды */
            fsm_dat_prc = FIND_CMD;
          } else {
            /* Если найден символ пробел */
            if (index_cmd != 255) {
              /* Проверка числа ожидаемых параметров */
              if (num_cmd_param > 0) {
                /* Число ожидаемых параметров больше нуля */
                /* Обнуляем индекс обрабатываемого параметра */
                index_param = 0;
                /* Включение режима загрузки параметров */
                fsm_dat_prc = FIND_PRM;
              } else {
                /* Если параметров нет - запрос обработки команды без
                 * параметра */
                status_cmd_param = STAT_NORM;
                /* функция обработки команд   */
                command_processing();
              }
            } else {
              /* Команда не распознана */
              status_cmd_param = STAT_INCORRECT_CMD;
              /* функция обработки команд   */
              command_processing();
              /* Включение режима поиск начала команды */
              fsm_dat_prc = FIND_CMD;
            }
          }
        }
      } else {
        /* Загрузка команды */
        data_prm_cmd[index_dat] = ch;
        /* При переполнении отключаем инкремент */
        if (index_dat < (MAX_SIZE_DATA - 1)) {
          index_dat++;
        }
      }
      break;

    case FIND_PRM:
      /* Поиск начала параметра команды */
      /* пропуск пробелов */
      if (ch == ' ') break;

      if ((ch == 0x0D) || (ch == 0x0A) || (ch == '>')) {
        /* функция обработки команд   */
        command_processing();
        /* Если найдено начало следующей команды */
        if (ch == '>') {
          /* Найдено начало команды - начинаем загрузку */
          fsm_dat_prc = PRC_CMD;
          /* функция предустановки параметров команды */
          preset_cmd();
        } else {
          /* Включение режима поиск начала команды */
          fsm_dat_prc = FIND_CMD;
        }
      } else {
        /* Загрузка загрузка параметра */
        data_prm_cmd[0] = ch;
        index_dat = 1;
        /* Включение режима загрузки параметра */
        fsm_dat_prc = PRC_PRM;
      }
      break;

    case PRC_PRM:
      /* Загрузка параметра */
      if (ch == ' ') {
        /* Установка завершающего символа */
        data_prm_cmd[index_dat] = 0;
        /* Обработка полученного параметра  */
        proc_parametr();

        if ((index_param < (MAX_PARAM - 1)) &&
            (status_cmd_param == STAT_NORM)) {
          index_param++;
          /* Включение режима загрузки параметров */
          fsm_dat_prc = FIND_PRM;
        } else {
          /* функция обработки команд   */
          command_processing();
          /* Включение режима поиск начала команды */
          fsm_dat_prc = FIND_CMD;
        }
      } else {
        if ((ch == 0x0D) || (ch == 0x0A) || (ch == '>')) {
          /* Установка завершающего символа */
          data_prm_cmd[index_dat] = 0;
          /* Обработка полученного параметра  */
          proc_parametr();
          /* функция обработки команд   */
          command_processing();
          /* Если найдено начало следующей команды */
          if (ch == '>') {
            /* Найдено начало команды - начинаем загрузку */
            fsm_dat_prc = PRC_CMD;
            /* функция предустановки параметров команды */
            preset_cmd();
          } else {
            /* Включение режима поиск начала команды */
            fsm_dat_prc = FIND_CMD;
          }
        } else {
          /* Загрузка загрузка параметра */
          data_prm_cmd[index_dat] = ch;
          /* При переполнении отключаем инкремент */
          if (index_dat < (MAX_SIZE_DATA - 1)) {
            index_dat++;
          }
        }
      }
      break;
    default:
      fsm_dat_prc = FIND_CMD;
      break;
  }
}

#if (DBG_UART_ENABLE == 1)
#include "uart_dbg.h"
#endif /* (DBG_UART_ENABLE == 1) */

/**
 * @brief  функция полинга терминала команд
 * @param  none
 * @retval none
 */
void terminal_cntrl(void) {
#if (DBG_UART_ENABLE == 1)
  while (index_wr_buf_cmd != index_rd_buf_cmd) {
    /* Обработка байта */
    terminal_data_processing(buf_cmd[index_rd_buf_cmd]);
    /* смещение индекса чтения */
    index_rd_buf_cmd++;
    /* Проверка на переполнение  */
    if (index_rd_buf_cmd >= DBG_UART_MAX_SIZE_CMD_BUFF) {
      /* Организация циклического буфера */
      index_rd_buf_cmd = 0;
    }
  }
#endif /* (DBG_UART_ENABLE == 1) */
}

/**
 * @brief  terminal task
 * @param  cmd_t* p_cmp - указатель на структуру обработки и описания команды
 * @retval bool false - команда добавлена успешно
 *              true - ошибка добавления команды
 */
bool add_terminal_cmd(const cmd_t *p_cmp) {
  if (max_num_cmd < MAX_SIZE_NUM_CMD) {
    if (p_cmp != NULL) {
      /* добавить команду */
      array_cmd[max_num_cmd] = (void *)p_cmp;
      /* нарастить максимум активных команд */
      max_num_cmd++;
      /* команда добавлена успешно */
      return false;
    }
  }
  max_num_cmd = MAX_SIZE_NUM_CMD;
  /* ошибка добавления команды */
  return true;
}

#include "cmd_freertos_cntrl.h"

/**
 * @brief  terminal task
 * @param  pvParameters not used
 * @retval None
 */
void terminal_task(void *pvParameters) {
  /* Инициализация указателя на команду помощи                               */
  help_cmd_init();
  /* Инициализация указателя на команду управления сбросом микроконтроллера  */
  reset_cmd_init();
  /* Инициализация указателя на команду управления светодиодом               */
  led_cmd_init();
  /* Инициализация указателя на команду просмотра параметров Freertos        */
  freertos_cmd_init();

  for (;;) {
    vTaskDelay(100);
    /* функция полинга терминала команд */
    terminal_cntrl();
  }
}
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/