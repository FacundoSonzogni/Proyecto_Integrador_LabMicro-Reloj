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
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

clock_t ClockCreate(uint16_t ticks_per_second){
    (void)ticks_per_second;

    clock_t self = malloc(sizeof(struct clock_s));
    if(self != NULL){
        self->valid_time = false;
        self->ticks_per_second = ticks_per_second;
        self->current_clock_tick = 0;
        memset(&(self->current_time), 0, sizeof(clock_time_t));
    }
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t *result){
    bool valid;
    
    if(self != NULL){
        if(self->valid_time){
            valid =  true;
        }else{
            valid = false;
        }

        memcpy(result, &(self->current_time), sizeof(clock_time_t));

    }else{
        valid = false;
    }

    return valid;
    
}

bool ClockSetTime(clock_t self, const clock_time_t *time_set){
    bool result = true;
    int hours, minutes, seconds;

    // Control de que la hora esté entre 00 y 23
    hours = time_set->time.hours[0] * 10 + time_set->time.hours[1];
    if(hours < 0 || hours > 23){
        result = false;
    }

    // Control de que los minutos estén entre 00 y 59
    minutes = time_set->time.minutes[0] * 10 + time_set->time.minutes[1];
    if(minutes < 0 || minutes > 59){
        result = false;
    }

    // Control de que los segundos estén entre 00 y 59
    seconds = time_set->time.seconds[0] * 10 + time_set->time.seconds[1];
    if(seconds < 0 || seconds > 59){
        result = false;
    }
    
    if(result == true){
        memcpy(&(self->current_time), time_set, sizeof(clock_time_t));
        self->valid_time = true;
    }

    return result;
}

void ClockTick(clock_t self){

    self->current_clock_tick++;
    
    if(self->current_clock_tick == self->ticks_per_second){
        self->current_clock_tick = 0;

        // Incremento de segundos
        if (self->current_time.time.seconds[1] < 9) {
            self->current_time.time.seconds[1]++;
        } else {
            self->current_time.time.seconds[1] = 0;
            if (self->current_time.time.seconds[0] < 5) {
                self->current_time.time.seconds[0]++;
            } else {
                self->current_time.time.seconds[0] = 0;

                // Incremento de minutos
                if (self->current_time.time.minutes[1] < 9) {
                    self->current_time.time.minutes[1]++;
                } else {
                    self->current_time.time.minutes[1] = 0;
                    if (self->current_time.time.minutes[0] < 5) {
                        self->current_time.time.minutes[0]++;
                    } else {
                        self->current_time.time.minutes[0] = 0;

                        // Incremento de horas
                        if (self->current_time.time.hours[1] < 9) {
                            self->current_time.time.hours[1]++;
                        } else {
                            self->current_time.time.hours[1] = 0;
                            self->current_time.time.hours[0]++;
                        }

                        if (self->current_time.time.hours[0] == 2 && self->current_time.time.hours[1] == 4) {
                            // Pasó de 23:59:59 a 00:00:00
                            memset(&(self->current_time), 0, sizeof(clock_time_t));
                        }
                    }
                }
            }
        }
    }
        
}

void ClockIncrementMinutes(clock_t self){
    if (self->current_time.time.minutes[1] < 9) {
        self->current_time.time.minutes[1]++;
    } else {
        self->current_time.time.minutes[1] = 0;
        if (self->current_time.time.minutes[0] < 5) {
            self->current_time.time.minutes[0]++;
        } else {
            self->current_time.time.minutes[0] = 0;
        }
    }
}

void ClockDecrementMinutes(clock_t self){
    if(self->current_time.time.minutes[1] > 0){
        self->current_time.time.minutes[1]--;
    } else {
        self->current_time.time.minutes[1] = 9;
        if(self->current_time.time.minutes[0] > 0){
            self->current_time.time.minutes[0]--;
        } else {
            self->current_time.time.minutes[0] = 5;
        }
        
    }
}

void ClockIncrementHours(clock_t self){
    if(self->current_time.time.hours[0] < 2){
        if(self->current_time.time.hours[1] < 9){
            self->current_time.time.hours[1]++;
        }else{
            self->current_time.time.hours[1] = 0;
            self->current_time.time.hours[0]++;
        }
    }else if(self->current_time.time.hours[1] < 3){
        self->current_time.time.hours[1]++;
    }else{
        self->current_time.time.hours[1] = 0;
        self->current_time.time.hours[0] = 0;
    }
}

void ClockDecrementHours(clock_t self){
    if(self->current_time.time.hours[0] > 0){
        if(self->current_time.time.hours[1] > 0){
            self->current_time.time.hours[1]--;
        }else{
            self->current_time.time.hours[1] = 9;
            self->current_time.time.hours[0]--;
        }
    }else if (self->current_time.time.hours[1] > 0){
        self->current_time.time.hours[1]--;
    }else{
        self->current_time.time.hours[0] = 2;
        self->current_time.time.hours[1] = 3;
    }
    
}

/* === End of documentation ======================================================================================== */