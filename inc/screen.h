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

#ifndef SCREEN_H
#define SCREEN_H

/** @file screen.h
 ** @brief Cabecera del módulo de gestión de la pantalla con displays de 7 segmmentos
 **
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A   (1 << 0)
#define SEGMENT_B   (1 << 1)
#define SEGMENT_C   (1 << 2)
#define SEGMENT_D   (1 << 3)
#define SEGMENT_E   (1 << 4)
#define SEGMENT_F   (1 << 5)
#define SEGMENT_G   (1 << 6)
#define SEGMENT_P   (1 << 7)

/* === Public data type declarations =============================================================================== */

//! Estructura de datos que representa una Pantalla de displays 7 segmentos
typedef struct screen_s * screen_t;

//! Tipo de dato que representa una función que permite apagar todos los habilitadores de los displays
typedef void (*digits_turn_off_t)(void);

//! Tipo de dato que representa una función que permite modificar los segmentos de un correspondiente display para escribir un número en la pantalla
typedef void (*segments_update_t)(uint8_t);

//! Tipo de dato que representa una función que permite encender un display específico de la pantalla
typedef void (*digit_turn_on)(uint8_t);


/*! Estructura de datos que representa el driver de la pantalla con las funciones de callback */
typedef struct screen_driver_s{
    digits_turn_off_t DigitsTurnOff;  //!< Función que permite apagar todos los habilitadores de los displays
    segments_update_t SegmentsUpdate; //!< Función que permite modificar los segmentos de un correspondiente display para escribir un número en la pantalla
    digit_turn_on DigitTurnOn;        //!< Función que permite encender un display específico de la pantalla
}const * screen_driver_t;


/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que permite crear una pantalla de displays 7 segmentos
 * 
 * @param digits Cantidad de d;igitos que tendrá la pantalla (es decir, cantidad de displays que forman la pantalla)
 * @param driver Driver con las funciones de callback, que utilizan las funciones del fabricante
 * @return screen_t Puntero a la estructura con los datos de la pantalla creada
 */
screen_t ScreenCreate(uint8_t digits, screen_driver_t driver);

/**
 * @brief Función que permite escribir un número en código BCD en la pantalla
 * 
 * @param screen Puntero a la estructura con los datos de la pantalla que se quiere escribir
 * @param value Arreglo en el que cada elemento es un dígito a mostrar codificado en BCD
 * @param size Cantidad de dígitos que se quiere que tenga la pantalla
 */
void ScreenWriteBCD(screen_t screen, uint8_t value[], uint8_t size);

/**
 * @brief Función de Tick que debe incluirse en un lazo externo para el refresco de la pantalla
 * 
 * @param screen Puntero a la estructura con los datos de la pantalla 
 */
void ScreenRefresh(screen_t screen);

/**
 * @brief Función que permite el parpadeo de los dígitos de la pantalla
 * 
 * @param screen Puntero a la estructura con los datos de la pantalla 
 * @param from Dígito desde el cual se quiere que empiece a parpadear
 * @param to Dígito hasta el cual se quiere que empiece a parpadear
 * @param half_period Cantidad de ciclos que los digitos están encendido (y que están apagados). Semi-período de parpadeo
 * @return 0 si fue posible el parpadeo. -1 si NO es posible realizar el parpadeo.
 * 
 * NOTA: Para realizar el parpadeo, el dígito 0 corresponde al dígito LSB (Unidades).
 * 
 * NOTA: Si se quiere que parpadee un solo dígito, se manda el mismo valor para "from" y "to"
 * 
 * NOTA: Si half_period = 50, significa que 50 ciclos está prendido y 50 ciclos está apagado (periódo total = 100 ciclos)
 * 
 * NOTA: Si half_period = 0, los segmentos NO parpadean
 */
int ScreenFlashDigits(screen_t screen, uint8_t from, uint8_t to, uint16_t half_period);

/**
 * @brief Función que permite configurar el estado del punto de uno de los displays
 * 
 * @param screen Puntero a la estructura con los datos de la pantalla
 * @param digit Número del dígito específico cuyo punto se quiere configurar (0 para el LSB)
 * @param turn_on TRUE si se quiere encender el punto. FALSE si se quiere apagar el punto
 */
void ScreenSetDotState(screen_t screen, uint8_t digit, bool turn_on);

/**
 * @brief Función que permite el parpadeo del punto de uno de los displays
 * 
 * @param screen Puntero a la estructura con los datos de la pantalla
 * @param digit Número del dígito específico cuyo punto se quiere configurar (0 para el LSB)
 * @param half_period Cantidad de ciclos que los digitos están encendido (y que están apagados). Semi-período de parpadeo
 * @return int 0 si fue posible el parpadeo. -1 si NO es posible realizar el parpadeo.
 * 
 * NOTA: Si half_period = 50, significa que 50 ciclos está prendido y 50 ciclos está apagado (periódo total = 100 ciclos)
 * 
 * NOTA: Si half_period = 0, el punto NO parpadea
 */
int ScreenFlashDot(screen_t screen, uint8_t digit, uint16_t half_period);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H */