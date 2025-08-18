/*********************************************************************************************************************
Copyright (c) 2025, Facundo Sonzogni <facundosonzogni1@gmail.com>
Copyright (c) 2025, Laboratorio de Microprocesadores, Universidad Nacional de Tucumán

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef APPMEF_H
#define APPMEF_H

/** @file AppMEF.h
 ** @brief Cabecera del módulo que contiene la Máquina de Estado Finito de la Aplicación deseada
 **
 **/

/* === Headers files inclusions ==================================================================================== */

#include "bsp.h"
#include "clock.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Estructura con los datos que deben pasarse como argumento de la tarea MEFTask()
typedef struct mef_task_args_s {
    const struct board_s* board;    //!< Puntero a la estructura con los datos de la placa
    clock_t clock;                  //!< Puntero a la estructura con los datos del reloj
    uint8_t set_time_mask;          //!< Máscara que representa al evento producido al mantener pulsado el botón "set_time"
    uint8_t increment_mask;         //!< Máscara que representa al evento producido al pulsar el botón "increment"
    uint8_t decrement_mask;         //!< Máscara que representa al evento producido al pulsar el botón "decrement"
    uint8_t accept_mask;            //!< Máscara que representa al evento producido al pulsar el botón "accept"
    uint8_t cancel_mask;            //!< Máscara que representa al evento producido al pulsar el botón "cancel"
    uint8_t set_alarm_mask;         //!< Máscara que representa al evento producido al mantener pulsado el botón "set_alarm"
    EventGroupHandle_t event_group; //!< Grupo de 32 bits que representan los posibles eventos producidos por los botones
}* mef_task_args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Tarea que realiza el cambio de estados de la MEF al usar FreeRTOS
 *
 * @param arguments Argumentos que deben pasarse a la tarea.
 */
void MEFTask(void* arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* APPMEF_H */
