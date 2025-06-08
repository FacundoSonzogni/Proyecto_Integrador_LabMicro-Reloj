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

#ifndef BSP_H
#define BSP_H

/** @file bsp.h
 ** @brief Cabecera del módulo de Soporte de Placa (Board Support)
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digitals.h"
#include "screen.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

//! Estructura de datos que representa a la placa de desarrollo
typedef struct board_s {
    digital_input_t key_F1;      //!< Tecla "F1" del Poncho (En el Reloj, sería la tecla para configurar la hora)
    digital_input_t key_F2;      //!< Tecla "F2" del Poncho (En el Reloj, sería la tecla para configurar la alarma)
    digital_input_t key_F3;      //!< Tecla "F3" del Poncho (En el Reloj, sería la tecla para decrementar el valor del display)
    digital_input_t key_F4;      //!< Tecla "F4" del Poncho (En el Reloj, sería la tecla para incrementar el valor del display)
    digital_input_t key_accept;  //!< Tecla "Aceptar" del Poncho
    digital_input_t key_cancel;  //!< Tecla "Cancelar" del Poncho
    digital_output_t led_alarm;  //!< Led RGB (se prende en rojo) del poncho (En el reloj, representaría la alarma)
    screen_t screen;             //!< Pantalla formada por los displays 7 segmentos del pocnho
} const* const board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que permite inicializar la placa, a medida del proyecto planteado
 *
 * @return board_t Puntero a la estructura con los datos de la placa
 */
board_t BoardCreate();

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */