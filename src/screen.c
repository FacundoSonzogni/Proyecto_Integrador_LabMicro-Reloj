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

/** @file screen.c
 ** @brief Código fuente del módulo de Gestión de la pantalla de displays 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "task.h"
#include "screen.h"
#include "bsp.h"
#include <stdlib.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

/*! Estructura de datos que representa una Pantalla de displays 7 segmentos */
struct screen_s {
    uint8_t digits;                                  //!< Cantidad de digitos que tiene la pantalla
    uint8_t memory_video[SCREEN_MAX_DIGITS];         //!< Arreglo en el que cada elemento representa los segmentos (8 bits) de cada uno de los displays
    uint8_t current_digit;                           //!< Digito actual que se está mostrando en la pantalla
    uint8_t flashing_from;                           //!< Digito desde el cual se produce el parapdeo (si es que parpadean los segmentos)
    uint8_t flashing_to;                             //!< Digito hasta el cual se produce el parapdeo (si es que parpadean los segmentos)
    uint8_t flashing_count;                          //!< Cuenta la cantidad de ciclos que van pasando (si es que parpadean los segmentos)
    uint16_t flashing_period;                        //!< Período del parpadeo de segmentos (Cantidad de ciclos totales entre que se enciende, se apaga y se vuelve a encender)
    uint8_t flashing_dot_count[SCREEN_MAX_DIGITS];   //!< Cuenta la cantidad de ciclos que van pasando (si es que parpadean los puntos)
    uint16_t flashing_dot_period[SCREEN_MAX_DIGITS]; //!< Período del parpadeo de los puntos (Cantidad de ciclos totales entre que se enciende, se apaga y se vuelve a encender)
    screen_driver_t driver;                          //!< Driver de la pantalla con las funciones de callback
};

/*! Arreglo constante de 10 elementos en los que cada elemnto representa los segmentos correspondientes a cada número del 0 al 9 */
static const uint8_t DIGIT_MAP[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             //!< Representa los segmentos del número "0"
    SEGMENT_B | SEGMENT_C,                                                             //!< Representa los segmentos del número "1"
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         //!< Representa los segmentos del número "2"
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         //!< Representa los segmentos del número "3"
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     //!< Representa los segmentos del número "4"
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         //!< Representa los segmentos del número "5"
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             //!< Representa los segmentos del número "6"
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 //!< Representa los segmentos del número "7"
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, //!< Representa los segmentos del número "8"
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,             //!< Representa los segmentos del número "9"
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

screen_t ScreenCreate(uint8_t digits, screen_driver_t driver) {
    screen_t self = malloc(sizeof(struct screen_s));

    if (digits > SCREEN_MAX_DIGITS) {
        digits = SCREEN_MAX_DIGITS;
    }

    if (self != NULL) {
        self->digits = digits;
        self->driver = driver;
        self->current_digit = 0;
        self->flashing_count = 0;
        self->flashing_period = 0;

        for (int i = 0; i < self->digits; i++) {
            self->flashing_dot_count[i] = 0;
            self->flashing_dot_period[i] = 0;
        }
    }

    return self;
}

void ScreenWriteBCD(screen_t self, uint8_t value[], uint8_t size) {

    for (int j = 0; j < self->digits; j++) {
        self->memory_video[j] = self->memory_video[j] & SEGMENT_P_MASK;
    }

    if (size > self->digits) {
        size = self->digits;
    }

    for (int i = 0; i < size; i++) {
        self->memory_video[i] = DIGIT_MAP[value[i]];
    }
}

void ScreenRefresh(screen_t self) {

    uint8_t segments;

    self->driver->DigitsTurnOff();

    if (self->current_digit < self->digits - 1) {
        self->current_digit = self->current_digit + 1;
    } else {
        self->current_digit = 0;
    }

    segments = self->memory_video[self->current_digit];

    // Parpadeo de los números (segmentos)
    if (self->flashing_period != 0) {
        if (self->current_digit == 0) {
            self->flashing_count = (self->flashing_count + 1) % (self->flashing_period);
        }

        if (self->flashing_count < (self->flashing_period / 2)) {
            if (self->current_digit >= self->flashing_from) {
                if (self->current_digit <= self->flashing_to) {
                    segments = segments & SEGMENT_P_MASK;
                }
            }
        }
    }

    // Parpadeo de los puntos
    for (int i = 0; i < self->digits; i++) {
        if (self->flashing_dot_period[i] != 0) {
            if (self->current_digit == 0) {
                if (self->flashing_dot_count[i] < self->flashing_dot_period[i] - 1) {
                    self->flashing_dot_count[i] = self->flashing_dot_count[i] + 1;
                } else {
                    self->flashing_dot_count[i] = 0;
                }
            }

            if (self->flashing_dot_period[self->current_digit] != 0) {
                if (self->flashing_dot_count[self->current_digit] < self->flashing_dot_period[self->current_digit] / 2) {
                    segments = segments & (~SEGMENT_P_MASK);
                }
            }
        }
    }

    self->driver->SegmentsUpdate(segments);
    self->driver->DigitTurnOn(self->current_digit);
}

int ScreenFlashDigits(screen_t self, uint8_t from, uint8_t to, uint16_t half_period) {
    int result = 0;

    if ((from > to) || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)) {
        result = -1;
    } else if (self == NULL) {
        result = -1;
    } else {

        uint16_t new_period = 2 * half_period;

        // Solo modificar si cambió la configuración
        if ((self->flashing_from != from) || (self->flashing_to != to) || (self->flashing_period != new_period)) {

            self->flashing_from = from;
            self->flashing_to = to;
            self->flashing_period = new_period;
            self->flashing_count = 0;
        }
    }

    return result;
}

void ScreenSetDotState(screen_t self, uint8_t digit, bool turn_on) {

    if (digit >= self->digits) {
        digit = (self->digits - 1);
    }

    if (self != NULL) {
        if (turn_on == true) {
            self->memory_video[(self->digits - 1) - digit] = self->memory_video[(self->digits - 1) - digit] | SEGMENT_P_MASK;
        } else if (turn_on == false) {
            self->memory_video[(self->digits - 1) - digit] = self->memory_video[(self->digits - 1) - digit] & (~SEGMENT_P_MASK);
        }
    }
}

int ScreenFlashDot(screen_t self, uint8_t digit, uint16_t half_period) {
    int result = 0;

    if (digit >= self->digits) {
        result = -1;
    } else if (self == NULL) {
        result = -1;
    } else {

        uint8_t i = (self->digits - 1) - digit;
        uint16_t new_period = 2 * half_period;

        // Solo cambiar si es distinto para evitar reiniciar el parpadeo
        if (self->flashing_dot_period[i] != new_period) {
            self->flashing_dot_period[i] = new_period;
            self->flashing_dot_count[i] = 0;
        }
    }

    return result;
}

void ScreenRefreshTask(void *screen){
    
    TickType_t last_value = xTaskGetTickCount();

    while (true) {

        if (screen != NULL) {
            ScreenRefresh(screen);
        }

        xTaskDelayUntil(&last_value, pdMS_TO_TICKS(1));
    }
}

/* === End of documentation ======================================================================================== */