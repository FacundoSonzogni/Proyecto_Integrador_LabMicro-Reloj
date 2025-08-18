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

#ifndef DIGITALS_H
#define DIGITALS_H

/** @file digitals.h
 ** @brief Cabecera del módulo de Gestión de Entradas y Salidas Digitales
 **
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Tipo de dato que representa en cambio en el estado de un pin de entrada
typedef enum digital_input_state_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1, //!< Indica que la entrada se activó
    DIGITAL_INPUT_NO_CHANGE = 0,        //!< Indica que la entrada no cambió
    DIGITAL_INPUT_WAS_ACTIVATED = 1,    //!< Indica que la entrada se desactivó
} digital_input_state_t;

//! Estructura de datos que representa una Salida Digital GPIO
typedef struct digital_output_s* digital_output_t;

//! Estructura de datos que representa una Entrada Digital GPIO
typedef struct digital_input_s* digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que permite crear una Salida Digital GPIO
 *
 * @param gpio_port Puerto GPIO correspondiente a la salida digital
 * @param gpio_bit Bit especiífico del puerto GPIO correspondiente a la salida digital
 * @param active_low TRUE si es una salida activa en bajo; FALSE si es una salida activa en alto
 * @return digital_output_t Puntero a la estructura que contiene los datos de la salida digital
 */
digital_output_t DigitalOutputCreate(uint8_t gpio_port, uint8_t gpio_bit, bool active_low);

/**
 * @brief Activa una salida digital GPIO
 *
 * @param terminal Puntero a la estructura con los datos de la salida que se quiere activar
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Desactiva una salida digital GPIO
 *
 * @param terminal Puntero a la estructura con los datos de la salida que se quiere desactivar
 */
void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Permite cambiar el estado de una salida digital GPIO
 *
 * @param terminal Puntero a la estructura con los datos de la salida que se quiere desactivar
 */
void DigitalOutputToggle(digital_output_t output);

/**
 * @brief Función que permite crear una Entrada Digital GPIO
 *
 * @param gpio_port Puerto GPIO correspondiente a la entrada digital
 * @param gpio_bit Bit especiífico del puerto GPIO correspondiente a la entrada digital
 * @param inverted_logic Determina si la entrada tiene lógica inversa o no
 * @return digital_input_t  Puntero a la estructura que contiene los datos de la entrada digital
 */
digital_input_t DigitalInputCreate(uint8_t gpio_port, uint8_t gpio_bit, bool inverted_logic);

/**
 * @brief Función que permite detectar si la entrada está activada
 *
 * @param input Puntero a la estructura que contiene los datos de la entrada
 * @return true Si la entrada está activada
 * @return false Si la entrada NO está activada
 */
bool DigitalInputGetIsActive(digital_input_t input);

/**
 * @brief Función que permite detectar si la entrada tuvo algún cambio
 *
 * @param input Puntero a la estructura que contiene los datos de la entrada
 * @return digital_input_state_t En qué sentido se produjo el cambio o si NO hubo ningún cambio
 */
digital_input_state_t DigitalInputHasChanged(digital_input_t input);

/**
 * @brief Función que permite detectar si la entrada fue activada
 *
 * @param input Puntero a la estructura que contiene los datos de la entrada
 * @return true Si la entrada pasó de desactivada a activada
 * @return false Si la entrada NO pasó de desactivada a activada
 */
bool DigitalInputWasActivated(digital_input_t input);

/**
 * @brief Función que permite detectar si la entrada fue desactivada
 *
 * @param input Puntero a la estructura que contiene los datos de la entrada
 * @return true Si la entrada pasó de activada a desactivada
 * @return false Si la entrada NO pasó de activada a desactivada
 */
bool DigitalInputWasDeactivated(digital_input_t input);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITALS_H */
