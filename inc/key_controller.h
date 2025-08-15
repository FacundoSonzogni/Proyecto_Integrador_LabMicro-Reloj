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

#include "digitals.h"
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Estructura de datos que representa un botón de la placa
typedef struct button_s* button_t;

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

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* KEY_CONTROLLER_H */