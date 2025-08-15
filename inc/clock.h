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

#ifndef CLOCK_H
#define CLOCK_H

/** @file clock.h
 ** @brief Cabecera del módulo de Gestión del Reloj
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

/* === Public data type declarations =============================================================================== */

//! Estructura de datos que representa la hora de dos posibles formas: Como un struct y como un arreglo
typedef union {
    struct {
        uint8_t hours[2];
        uint8_t minutes[2];
        uint8_t seconds[2];
    } time;
    uint8_t bcd[6];
} clock_time_t;

//! Estructura de datos que representa el Reloj
typedef struct clock_s* clock_t;

//! Tipo de dato que representa una función que permite encender la alarma
typedef void (*clock_alarm_turn_on)(void);

//! Tipo de dato que representa una función que permite apagar la alarma
typedef void (*clock_alarm_turn_of)(void);

//! Estructura de datos que representa el driver del reloj con las funciones de callback para gestionar la alarma
typedef struct clock_alarm_driver_s {
    clock_alarm_turn_on ClockAlarmTurnOn;  //!< Función que permite encender el sonido de la alarma
    clock_alarm_turn_of ClockAlarmTurnOff; //!< Función que permite apagar el sonido de la alarma
} const* clock_alarm_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función que permite crear al objeto reloj
 *
 * @param ticks_per_second Cantidad de ticks que hay en un segundo
 * @param snooze_seconds Cantidad de segundos que se pospone la alarma (si es que se pospone)
 * @param driver Driver con las funciones para encender y apagar la alarma
 * @return clock_t
 */
clock_t ClockCreate(uint16_t ticks_per_second, uint16_t snooze_seconds, clock_alarm_driver_t driver);

/**
 * @brief Función que permite obtener la hora actual del reloj
 *
 * @param clock Puntero con los datos del reloj
 * @param result Puntero a la estructura con la hora actual del reloj
 * @return true Si la hora actual es válida
 * @return false Si la hora actual es inválida
 */
bool ClockGetTime(clock_t clock, clock_time_t* result);

/**
 * @brief Función que permite poner el reloj en una determinada hora
 *
 * @param clock Puntero a la estructura con los datos del reloj
 * @param time_set Puntero a la estructura con la hora, minutos y segundos que se desean setear
 * @return true Si la hora seteada es válida
 * @return false Si la hora seteada es inválida
 */
bool ClockSetTime(clock_t clock, const clock_time_t* time_set);

/**
 * @brief Función de Tick para el Reloj
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockTick(clock_t clock);

/**
 * @brief Función que permite incrementar el valor de los minutos
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockIncrementMinutes(clock_t clock);

/**
 * @brief Función que permite decrementar el valor de los minutos
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDecrementMinutes(clock_t clock);

/**
 * @brief Función que permite incrementar el valor de las horas
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockIncrementHours(clock_t clock);

/**
 * @brief Función que permite decrementar el valor de las horas
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDecrementHours(clock_t clock);

/**
 * @brief Función que permite habilitar y setear la hora de la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 * @param time_set Puntero a la estructura con la hora, minutos y segundos de la alarma
 * @return true Si se pudo setear la alarma
 * @return false Si no se pudo setear la alarma
 */
bool ClockSetAlarm(clock_t clock, const clock_time_t* time_set);

/**
 * @brief Función que permite leer la hora seteada para la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 * @param alarm_time Puntero a la estructura donde se guardará la hora de la alarma
 * @return true Si fue posible leer la hora setetada para la alarma
 * @return false Si no fue posible leer la hora setetada para la alarma
 */
bool ClockGetAlarm(clock_t clock, clock_time_t* alarm_time);

/**
 * @brief Función que permite saber si la alarma está activada
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 * @return true Si la alarma está activada
 * @return false Si la alarma NO está activada
 */
bool ClockGetIfAlarmIsActivated(clock_t clock);

/**
 * @brief Función que permite deshabilitar una alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDisableAlarm(clock_t clock);

/**
 * @brief Función que permite incrementar el valor de los minutos de la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockIncrementAlarmMinutes(clock_t clock);

/**
 * @brief Función que permite decrementar el valor de los minutos de la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDecrementAlarmMinutes(clock_t clock);

/**
 * @brief Función que permite incrementar el valor de las horas de la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockIncrementAlarmHours(clock_t clock);

/**
 * @brief Función que permite decrementar el valor de las horas de la alarma
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDecrementAlarmHours(clock_t clock);

/**
 * @brief Función que permite hacer que la alarma suene
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 * @return true Si es posible hacer sonar la alarma
 * @return false Si no es poisble hacer sonar la alarma
 */
bool ClockRingAlarm(clock_t clock);

/**
 * @brief Función que permite saber si la alarma está sonando o no
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 * @return true Si la alarma está sonando
 * @return false Si la alarma no está sonando
 */
bool ClockGetIfAlarmIsRinging(clock_t clock);

/**
 * @brief Fución que permite habilitar el sonido de la alarma (Significa que PUEDE sonar)
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockEnableRinging(clock_t clock);

/**
 * @brief Fución que permite deshabilitar el sonido de la alarma (Significa que NO PUEDE sonar)
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockDisableRingig(clock_t clock);

/**
 * @brief Función que permite posponer una alarma un determinado tiempo
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockSnoozeAlarm(clock_t clock);

/**
 * @brief Función que permite apagar el sonido de la alarma hasta el otro día
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
void ClockCancelAlarm(clock_t clock);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H */
