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

#ifndef KEY_CONTROLLER_H
#define KEY_CONTROLLER_H

/** @file key_controller.h
 ** @brief Cabecera del módulo de gestión del pulsado de las teclas
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "digitals.h"
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define KEY_EVENT_KEY_0     (1 << 0) //!< Máscara que representa un bit en alto en la posición 0 del grupo de eventos
#define KEY_EVENT_KEY_1     (1 << 1) //!< Máscara que representa un bit en alto en la posición 1 del grupo de eventos
#define KEY_EVENT_KEY_2     (1 << 2) //!< Máscara que representa un bit en alto en la posición 2 del grupo de eventos
#define KEY_EVENT_KEY_3     (1 << 3) //!< Máscara que representa un bit en alto en la posición 3 del grupo de eventos
#define KEY_EVENT_KEY_4     (1 << 4) //!< Máscara que representa un bit en alto en la posición 4 del grupo de eventos
#define KEY_EVENT_KEY_5     (1 << 5) //!< Máscara que representa un bit en alto en la posición 5 del grupo de eventos
#define KEY_EVENT_KEY_6     (1 << 6) //!< Máscara que representa un bit en alto en la posición 6 del grupo de eventos
#define KEY_EVENT_KEY_7     (1 << 7) //!< Máscara que representa un bit en alto en la posición 7 del grupo de eventos

#define KEY_EVENT_ANY_KEY   0xFF //!< Máscara que representa un bit en alto en la posiciones 0 a 7 del grupo de eventos

#define KEY_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE) //!< Cantidad de memoria necesaria en la pila para las tareas de las teclas

/* === Public data type declarations =============================================================================== */

//! Estructura de datos que representa un botón de la placa
typedef struct button_s* button_t;

//! Estructura de datos que representa los argumentos para las tareas al usar FreeRTOS
typedef struct button_task_args_s{
    EventGroupHandle_t event_group;  //!< Grupo de 32 bits que representan los posibles eventos
    uint8_t event_mask;              //!< Máscara para representar un bit en particular del grupo de eventos
    digital_input_t key;             //!< Tecla correspondiente a una entrada digital de la placa
}* button_task_args_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que permite inicializar un botón
 *
 * @param key Puntero a la estructura con los datos de la tecla (digital_input_t)
 * @return button_t Puntero a la estructura con los datos del botón
 */
button_t ButtonCreate(digital_input_t key);

/**
 * @brief Función que permite saber si un botón fue pulsado por 3 segundos o mas. Debe llamarse cada 1ms
 *
 * @param button Puntero a la estructura con los datos de la tecla
 * @return true Si el botón fue pulsado por 3 segundos o más
 * @return false Si el botón NO fue pulsado por 3 segundos o más
 */
bool ButtonWasPressed3secs(button_t button);

/**
 * @brief Función que permite saber si un botón fue pulsado
 *
 * @param button Puntero a la estructura con los datos de la tecla
 * @return true Si el botón fue pulsado
 * @return false Si el botón no fue pulsado
 */
bool ButtonWasPressed(button_t button);

/**
 * @brief Tarea que genera un evento cuando se pulsa una tecla (con antirrebote) al usar FreeRTOS
 * 
 * @param arguments Argumentos necesarios para la tarea.
 */
void ButtonPressedTask(void *arguments);

/**
 * @brief Tarea que genera un evento cuando se pulsa una tecla 3 segundos, al usar FreeRTOS
 * 
 * @param arguments Argumentos necesarios para la tarea.
 */
void ButtonPressed3secsTask(void *arguments);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* KEY_CONTROLLER_H */