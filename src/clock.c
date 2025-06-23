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

/** @file clock.c
 ** @brief Código fuente del módulo de Gestión del Reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/*! Estructura de datos que representa un Reloj */
struct clock_s {
    clock_time_t current_time;
    bool valid_time;
    uint16_t ticks_per_second;
    uint16_t current_clock_tick;
    clock_time_t setted_alarm_time;
    bool activated_alarm;
    bool alarm_is_ringing;
    bool ringig_is_enabled;
    bool snoozed_alarm;
    uint16_t snooze_seconds;
    uint16_t seconds_count;
    clock_time_t snoozed_alarm_time;
    clock_alarm_driver_t alarm_driver;
};

/* === Private function declarations =============================================================================== */

/**
 * @brief Función interna que permite saber si una determinada hora (horas, minutos y segundos) es válida o no
 *
 * @param time Puntero a la estructura con la hora cuya validez se desea chequear
 * @return true Si la hora ingresada es válida
 * @return false Si la hora ingresada no es válida
 */
static bool CheckTimeIsValid(const clock_time_t* time);

/**
 * @brief Función de Tick que solo incrementa la hora en 1 segundo cada vez que es llamada
 *
 * @param clock Puntero a la estructura con los datos del Reloj
 */
static void ClockTickIncrement(clock_time_t* current_time);

/**
 * @brief Función interna que incrementa los minutos en 1 unidad
 *
 * @param time Puntero a la estructura con los datos de la hora cuyos minutos se desean incrementar
 */
static void IncrementMinutes(clock_time_t* time);

/**
 * @brief Función interna que decrementa los minutos en 1 unidad
 *
 * @param time Puntero a la estructura con los datos de la hora cuyos minutos se desean decrementar
 */
static void DecrementMinutes(clock_time_t* time);

/**
 * @brief Función interna que incrementa las horas en 1 unidad
 *
 * @param time Puntero a la estructura con los datos de la hora cuyas horas se desean incrementar
 */
static void IncrementHours(clock_time_t* time);

/**
 * @brief Función interna que decrementa las horas en 1 unidad
 *
 * @param time Puntero a la estructura con los datos de la hora cuyas horas se desean decrementar
 */
static void DecrementHours(clock_time_t* time);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static bool CheckTimeIsValid(const clock_time_t* time) {
    bool result = true;
    int hours, minutes, seconds;

    // Control de que la hora esté entre 00 y 23
    hours = time->time.hours[0] * 10 + time->time.hours[1];
    if (hours < 0 || hours > 23) {
        result = false;
    }

    // Control de que los minutos estén entre 00 y 59
    minutes = time->time.minutes[0] * 10 + time->time.minutes[1];
    if (minutes < 0 || minutes > 59) {
        result = false;
    }

    // Control de que los segundos estén entre 00 y 59
    seconds = time->time.seconds[0] * 10 + time->time.seconds[1];
    if (seconds < 0 || seconds > 59) {
        result = false;
    }

    return result;
}

static void ClockTickIncrement(clock_time_t* current_time) {
    // Incremento de segundos
    if (current_time->time.seconds[1] < 9) {
        current_time->time.seconds[1]++;
    } else {
        current_time->time.seconds[1] = 0;
        if (current_time->time.seconds[0] < 5) {
            current_time->time.seconds[0]++;
        } else {
            current_time->time.seconds[0] = 0;

            // Incremento de minutos
            if (current_time->time.minutes[1] < 9) {
                current_time->time.minutes[1]++;
            } else {
                current_time->time.minutes[1] = 0;
                if (current_time->time.minutes[0] < 5) {
                    current_time->time.minutes[0]++;
                } else {
                    current_time->time.minutes[0] = 0;

                    // Incremento de horas
                    if (current_time->time.hours[1] < 9) {
                        current_time->time.hours[1]++;
                    } else {
                        current_time->time.hours[1] = 0;
                        current_time->time.hours[0]++;
                    }

                    if (current_time->time.hours[0] == 2 && current_time->time.hours[1] == 4) {
                        // Pasó de 23:59:59 a 00:00:00
                        memset(current_time, 0, sizeof(clock_time_t));
                    }
                }
            }
        }
    }
}

static void IncrementMinutes(clock_time_t* current_time) {
    if (current_time->time.minutes[1] < 9) {
        current_time->time.minutes[1]++;
    } else {
        current_time->time.minutes[1] = 0;
        if (current_time->time.minutes[0] < 5) {
            current_time->time.minutes[0]++;
        } else {
            current_time->time.minutes[0] = 0;
        }
    }
}

static void DecrementMinutes(clock_time_t* current_time) {
    if (current_time->time.minutes[1] > 0) {
        current_time->time.minutes[1]--;
    } else {
        current_time->time.minutes[1] = 9;
        if (current_time->time.minutes[0] > 0) {
            current_time->time.minutes[0]--;
        } else {
            current_time->time.minutes[0] = 5;
        }
    }
}

static void IncrementHours(clock_time_t* current_time) {
    if (current_time->time.hours[0] < 2) {
        if (current_time->time.hours[1] < 9) {
            current_time->time.hours[1]++;
        } else {
            current_time->time.hours[1] = 0;
            current_time->time.hours[0]++;
        }
    } else if (current_time->time.hours[1] < 3) {
        current_time->time.hours[1]++;
    } else {
        current_time->time.hours[1] = 0;
        current_time->time.hours[0] = 0;
    }
}

static void DecrementHours(clock_time_t* current_time) {
    if (current_time->time.hours[0] > 0) {
        if (current_time->time.hours[1] > 0) {
            current_time->time.hours[1]--;
        } else {
            current_time->time.hours[1] = 9;
            current_time->time.hours[0]--;
        }
    } else if (current_time->time.hours[1] > 0) {
        current_time->time.hours[1]--;
    } else {
        current_time->time.hours[0] = 2;
        current_time->time.hours[1] = 3;
    }
}

/* === Public function definitions ================================================================================= */

clock_t ClockCreate(uint16_t ticks_per_second, uint16_t snooze_seconds, clock_alarm_driver_t driver) {

    clock_t self = malloc(sizeof(struct clock_s));
    if (self != NULL) {
        self->valid_time = false;
        self->ticks_per_second = ticks_per_second;
        self->current_clock_tick = 0;
        memset(&(self->current_time.bcd), 0, sizeof(clock_time_t));
        memset(&(self->setted_alarm_time.bcd), 0, sizeof(clock_time_t));
        self->activated_alarm = false;
        self->alarm_is_ringing = false;
        self->ringig_is_enabled = true;
        self->snoozed_alarm = false;
        self->seconds_count = 0;
        self->snooze_seconds = snooze_seconds;
        self->alarm_driver = driver;
    }
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t* result) {
    bool valid;

    if (self != NULL) {
        if (self->valid_time) {
            valid = true;
        } else {
            valid = false;
        }

        memcpy(result, &(self->current_time), sizeof(clock_time_t));

    } else {
        valid = false;
    }

    return valid;
}

bool ClockSetTime(clock_t self, const clock_time_t* time_set) {
    bool result;

    if (self == NULL) {
        return false;
    } else {
        result = CheckTimeIsValid(time_set);

        if (result == true) {
            memcpy(&(self->current_time), time_set, sizeof(clock_time_t));
            self->valid_time = true;
        }

        return result;
    }
}

void ClockTick(clock_t self) {

    self->current_clock_tick++;

    if (self != NULL) {
        if (self->current_clock_tick == self->ticks_per_second) {
            self->current_clock_tick = 0;

            if (self->snoozed_alarm == false) {
                if (self->ringig_is_enabled) {
                    if (memcmp(&(self->current_time.bcd), &(self->setted_alarm_time.bcd), sizeof(clock_time_t))) {
                        ClockRingAlarm(self);
                    }
                } else {
                    self->alarm_is_ringing = false;
                }

                memcpy(&(self->snoozed_alarm_time.bcd), &(self->setted_alarm_time.bcd), sizeof(clock_time_t));

            } else {
                if (memcmp(&(self->current_time.bcd), &(self->snoozed_alarm_time.bcd), sizeof(clock_time_t))) {
                    self->ringig_is_enabled = true;
                    self->alarm_is_ringing = true;
                    self->snoozed_alarm = false;
                    self->alarm_driver->ClockAlarmTurnOn(self);
                }
            }

            ClockTickIncrement(&(self->current_time));
        }
    }
}

void ClockIncrementMinutes(clock_t self) {
    if (self != NULL) {
        IncrementMinutes(&(self->current_time));
    }
}

void ClockDecrementMinutes(clock_t self) {
    if (self != NULL) {
        DecrementMinutes(&(self->current_time));
    }
}

void ClockIncrementHours(clock_t self) {
    if (self != NULL) {
        IncrementHours(&(self->current_time));
    }
}

void ClockDecrementHours(clock_t self) {
    if (self != NULL) {
        DecrementHours(&(self->current_time));
    }
}

bool ClockSetAlarm(clock_t self, const clock_time_t* time_set) {
    bool result;

    if (self == NULL) {
        return false;
    } else {
        self->activated_alarm = false;

        result = CheckTimeIsValid(time_set);

        if (result == true) {
            memcpy(&(self->setted_alarm_time.bcd), time_set, sizeof(clock_time_t));
            memcpy(&(self->snoozed_alarm_time.bcd), &(self->setted_alarm_time.bcd), sizeof(clock_time_t));
            self->activated_alarm = true;
        }

        return result;
    }
}

bool ClockGetAlarm(clock_t self, clock_time_t* alarm_time) {
    bool result = false;

    if (self != NULL) {
        if (self->activated_alarm) {
            memcpy(alarm_time, &(self->setted_alarm_time), sizeof(clock_time_t));
            result = true;
        }
    }
    return result;
}

bool ClockGetIfAlarmIsActivated(clock_t self) {
    bool result;
    if (self == NULL) {
        result = false;
    } else {
        result = self->activated_alarm;
    }

    return result;
}

void ClockDisableAlarm(clock_t self) {
    self->activated_alarm = false;
}

void ClockIncrementAlarmMinutes(clock_t self) {
    if (self != NULL) {
        IncrementMinutes(&(self->setted_alarm_time));
    }
}

void ClockDecrementAlarmMinutes(clock_t self) {
    if (self != NULL) {
        DecrementMinutes(&(self->setted_alarm_time));
    }
}

void ClockIncrementAlarmHours(clock_t self) {
    if (self != NULL) {
        IncrementHours(&(self->setted_alarm_time));
    }
}

void ClockDecrementAlarmHours(clock_t self) {
    if (self != NULL) {
        DecrementHours(&(self->setted_alarm_time));
    }
}

bool ClockRingAlarm(clock_t self) {
    bool result = false;

    if (self != NULL) {
        if (ClockGetIfAlarmIsActivated(self)) {
            self->alarm_is_ringing = true;
            self->alarm_driver->ClockAlarmTurnOn(self);
            result = true;
        }
    }

    return result;
}

bool ClockGetIfAlarmIsRinging(clock_t self) {
    bool result = false;

    if (self != NULL) {
        result = self->alarm_is_ringing;
    }

    return result;
}

void ClockEnableRinging(clock_t self) {
    self->ringig_is_enabled = true;
}

void ClockDisableRingig(clock_t self) {
    self->ringig_is_enabled = false;
}

void ClockSnoozeAlarm(clock_t self) {
    for (uint16_t i = 0; i < self->snooze_seconds; i++) {
        ClockTickIncrement(&(self->snoozed_alarm_time));
    }

    self->snoozed_alarm = true;
    self->ringig_is_enabled = false;
    self->alarm_is_ringing = false;
    self->alarm_driver->ClockAlarmTurnOff(self);
}

void ClockCancelAlarm(clock_t self) {
    memcpy(&(self->snoozed_alarm_time.bcd), &(self->setted_alarm_time.bcd), sizeof(clock_time_t));

    self->snoozed_alarm = true;
    self->ringig_is_enabled = false;
    self->alarm_is_ringing = false;
    self->alarm_driver->ClockAlarmTurnOff(self);
}
/* === End of documentation ======================================================================================== */