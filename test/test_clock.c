/*********************************************************************************************************************
Copyright (c) 2025, Facundo Sonzogni <facundosonzogni1@gmail.com>
Copyright (c) 2025, Laboratorio de Microprocesadores, Universidad Nacional de
Tucumán

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file test_clock.c
 ** @brief Pruebas para seguir un patrón TDD para la bliblioteca de Reloj
 ** LISTADO DE PRUEBAS:
 ** - 1) Probar que el reloj, al iniciar, se encuentra en un estado inválido
 ** - 2) Probar que el reloj se inicializa internamente en 00:00
 ** - 3) Probar función ClockGetTime() con NULL como argumento "clock" (La hora debe ser 00:00)
 ** - 4) Probar que se puede establecer la hora a un valor válido
 ** - 5) Probar que se comprueba que la hora ingresada esté entre 00 y 23
 ** - 6) Probar que se comprueba que los minutos ingresados estén entre 00 y 59
 ** - 7) Probar que se comprueba que los segundos ingresados estén entre 00 y 59
 ** - 8) Probar que puedo setear un valor válido, leer la hora seteada y comprobar que es válida
 ** - 9) Probar que la hora avanza correctamente en un segundo cada vez que se simulan N ticks de tiempo
 ** - 10) Probar que los segundos pasan de _9 a _0
 ** - 11) Probar que cuando los segundos pasan de 59, las unidades de los minutos avanzan en 1
 ** - 12) Probar que los minutos pasan de _9 a _0
 ** - 13) Probar que cuando los minutos pasan de 59, las unidades de las horas avanzan en 1
 ** - 14) Probar que las horas pasan de 09 a 10
 ** - 15) Probar que las horas pasan de 19 a 20
 ** - 16) Probar que se pasa de 23:59:59 a 00:00:00
 ** - 17) Probar que, en modo de ajuste, se puede incrementar el valor de los minutos (mediante la señal de "F4") 
 ** - 18) Probar que, en modo de ajuste, se puede incrementar el valor de los minutos de _9 a _0
 ** - 19)  Probar que el incremento  de minutos realizan un ciclo al alcanzar el valor máximo (59 a 00)
 ** - 20)  Probar que, en modo de ajuste, se puede disminuir el valor de los minutos (mediante la señal de "F3")
 ** - 21) Probar que, en modo de ajuste, se puede decrementar el valor de los minutos de _0 a _9
 ** - 22) Probar que el decremento de minutos realizan un ciclo al alcanzar el valor mínimo (00 a 59)
 ** - 23) Probar que, en modo de ajuste, se puede incrementar el valor de las horas (mediante la señal de "F4")
 ** - 24) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 09 a 10
 ** - 25) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 19 a 20
 ** - 26) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 22 a 23
 ** - 27)  Probar que el incremento de horas realiza un ciclo al alcanzar el valor máximo (23 a 00)
 ** - 28) Probar que, en modo de ajuste, se puede decrementar el valor de las horas (mediante la señal de "F3")
 ** - 29) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 10 a 09
 ** - 30) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 20 a 19
 ** - 31) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 01 a 00
 ** - 32)  Probar que el incremento de horas realiza un ciclo al alcanzar el valor máximo (00 a 23)
 ** -  Probar que se puede definir una alarma para una hora y minutos específicos
 ** -  Probar que la definición de alarma solo acepta el formato de 24 horas
 ** -  Probar que se pueden definir alarmas hasta el límite máximo soportado por el sistema
 ** -  Probar que se rechaza la definición de alarmas adicionales si se supera el límite máximo
 ** -  Probar que se puede eliminar una alarma existente
 ** -  Probar que la alarma se puede habilitar
 ** -  Probar que la alarma se puede inhabilitar
 ** -  Probar que la alarma se activa mediante la señal de "Aceptar" en modo normal del reloj
 ** -  Probar que la alarma se desactiva mediante la señal de "Cancelar" en modo normal del reloj
 ** -  Probar que la alarma se activa precisamente a la hora y minuto definidos
 ** -  Probar que la alarma no se activa si está inhabilitada
 ** -  Probar que si hay múltiples alarmas configuradas para el mismo minuto, todas se activan
 ** -  Probar que una alarma de un solo uso se activa una única vez
 ** -  Probar que cuando la alarma se activa, una señal de "Aceptar" la pospone por cinco minutos
 ** -  Probar que cuando la alarma se activa, una señal de "Cancelar" la silencia hasta el día siguiente a la hora programada
 ** -  Probar que el reloj ingresa al modo de ajuste de la alarma al recibir la señal de inicio de ajuste (señal de "F2" por más de 3 segundos)
 ** -  Probar que el ajuste de la hora de la alarma sigue el mismo comportamiento lógico que el ajuste de la hora del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_PER_SECOND 5
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units, seconds_tens, seconds_units, expected_time) \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, expected_time.bcd[0], "Diference in the tens of hours");         \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, expected_time.bcd[1], "Diference in the units of hours");       \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, expected_time.bcd[2], "Diference in the tens of minutes");     \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, expected_time.bcd[3], "Diference in the units of minutes");   \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, expected_time.bcd[4], "Diference in the tens of seconds");     \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, expected_time.bcd[5], "Diference in the units of seconds");   \

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/**
 * @brief Función que permite simular que pasaron N segundos
 * 
 * @param clock Puntero a la estructura con los datos del reloj
 * @param seconds Cantidad (N) de segundos que se quieren simular
 */
void SimulateNSeconds(clock_t clock, uint8_t seconds){
    for (uint8_t i = 0; i < CLOCK_TICKS_PER_SECOND * seconds; i++){
        ClockTick(clock);
    }
}

/* === Public function definitions ================================================================================= */

// 1) Probar que el reloj, al iniciar, se encuentra en un estado inválido
void test_set_up_clock_is_invalid(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);
    clock_time_t current_time;

    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
}

// 2) Probar que el reloj se inicializa internamente en 00:00 
void test_set_up_time_is_00_00 (void){   
    clock_time_t current_time = {
        .bcd = {1,2,3,4,5,6},
    };
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    ClockGetTime(clock, &current_time);
    TEST_ASSERT_EACH_EQUAL_UINT8 (0, current_time.bcd, 6);
}

// 3) Probar función ClockGetTime() con NULL como argumento "clock" 
void test_ClockGetTime_with_argument_NULL(void){
    clock_t clock = NULL;
    clock_time_t current_time = {
        .bcd = {1,2,3,4,5,6},
    };

    TEST_ASSERT_FALSE (ClockGetTime(clock, &current_time));
}

// 4) Probar que se puede establecer la hora a un valor válido
void test_time_can_be_set_in_valid_value(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t set_time = {
        .time.hours = {1,4},
        .time.minutes = {3,0}, 
        .time.seconds = {1,5},              
    };

    TEST_ASSERT_TRUE(ClockSetTime(clock, &set_time));   
}

// 5) Probar que se comprueba que la hora ingresada esté entre 00 y 23
void test_setted_hour_is_a_valid_hour(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t new_time = {
        .time.hours = {2,4},
        .time.minutes = {3,0}, 
        .time.seconds = {1,5}, 
    };

    TEST_ASSERT_FALSE(ClockSetTime(clock, &new_time));
}

// 6) Probar que se comprueba que los minutos ingresados estén entre 00 y 59
void test_setted_minutes_is_a_valid_value_for_minutes(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t new_time = {
        .time.hours = {2,1},
        .time.minutes = {6,0},    
        .time.seconds = {1,5},    
    };

    TEST_ASSERT_FALSE(ClockSetTime(clock, &new_time));
}

// 7) Probar que se comprueba que los segundos ingresados estén entre 00 y 59
void test_setted_seconds_is_a_valid_value_for_seconds(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t new_time = {
        .time.hours = {2,1},
        .time.minutes = {3,0},
        .time.seconds = {6,0},        
    };

    TEST_ASSERT_FALSE(ClockSetTime(clock, &new_time));
}

// 8) Probar que puedo setear un valor válido, leer la hora seteada y comprobar que es válida
void test_set_valid_time_and_read_it (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t new_time = {
        .time.hours = {2,1},
        .time.minutes = {3,0},
        .time.seconds = {3,0},
    };

    clock_time_t current_time;

    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TRUE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EQUAL_UINT8_ARRAY (current_time.bcd, new_time.bcd, 6);
    
}

// 9) Probar que la hora avanza correctamente en un segundo cada vez que se simulan N ticks de tiempo
void test_clock_advance_one_second(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);
    
    static const clock_time_t current_time = {
        .time.hours = {1,4},
        .time.minutes = {3,0},
        .time.seconds = {1,5},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,4,3,0,1,6,new_time);
}

// 10) Probar que los segundos pasan de _9 a _0
void test_seconds_from_x9_to_y0 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,4},
        .time.minutes = {3,0},
        .time.seconds = {1,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,4,3,0,2,0,new_time);
}

// 11) Probar que cuando los segundos pasan de 59, las unidades de los minutos avanzan en 1
void test_seconds_from_59_to_00_and_adding_1_to_minutes(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,4},
        .time.minutes = {3,0},
        .time.seconds = {5,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,4,3,1,0,0,new_time);
}

// 12) Probar que los minutos pasan de _9 a _0
void test_minutes_from_x9_to_y0 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,4},
        .time.minutes = {3,9},
        .time.seconds = {5,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,4,4,0,0,0,new_time);
}

// 13) Probar que cuando los minutos pasan de 59, las unidades de las horas avanzan en 1
void test_minutes_from_59_to_00_and_adding_1_to_hours(void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,4},
        .time.minutes = {5,9},
        .time.seconds = {5,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,5,0,0,0,0,new_time);
}

// 14) Probar que las horas pasan de 09 a 10
void test_hours_from_9_to_10 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {0,9},
        .time.minutes = {5,9},
        .time.seconds = {5,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,0,0,0,0,0,new_time);
}

// 15) Probar que las horas pasan de 19 a 20
void test_hours_from_19_to_20 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,9},
        .time.minutes = {5,9},
        .time.seconds = {5,9},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 1);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,0,0,0,0,0,new_time);
}

// 16) Probar que se pasa de 23:59:59 a 00:00:00
void test_time_goes_from_23_59_59_to_00_00_00 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {5,9},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);

    SimulateNSeconds(clock, 10);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(0,0,0,0,0,0,new_time);
}

// 17) Probar que, en modo de ajuste, se puede incrementar el valor de los minutos (mediante la señal de "F4")
void test_increment_the_value_of_the_minutes (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {4,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,4,2,5,0,new_time);
}

// 18) Probar que, en modo de ajuste, se puede incrementar el valor de los minutos de _9 a _0
void test_increment_the_value_of_the_minutes_from_x9_to_y0 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {4,9},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,5,0,5,0,new_time);
}

// 19)  Probar que el incremento  de minutos realizan un ciclo al alcanzar el valor máximo (59 a 00)
void test_increment_the_value_of_the_minutes_from_59_to_00 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {5,9},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,0,0,5,0,new_time);
}

// 20)  Probar que, en modo de ajuste, se puede disminuir el valor de los minutos (mediante la señal de "F3")
void test_decrement_the_value_of_the_minutes (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {4,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,4,0,5,0,new_time);
}

// 21) Probar que, en modo de ajuste, se puede decrementar el valor de los minutos de _0 a _9
void test_decrement_the_value_of_the_minutes_from_x0_to_y9 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {4,0},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,3,9,5,0,new_time);
}

// 22) Probar que el decremento de minutos realizan un ciclo al alcanzar el valor mínimo (00 a 59)
void test_decrement_the_value_of_the_minutes_from_00_to_59 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {0,0},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementMinutes(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,5,9,5,0,new_time);
}

// 23) Probar que, en modo de ajuste, se puede incrementar el valor de las horas (mediante la señal de "F4")
void test_increment_the_value_of_the_hours (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,5},
        .time.minutes = {4,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,6,4,1,5,0,new_time);
}

// 24) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 09 a 10
void test_increment_the_value_of_the_hours_from_09_to_10 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {0,9},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,0,4,3,5,0,new_time);
}

// 25) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 19 a 20
void test_increment_the_value_of_the_hours_from_19_to_20 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,9},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,0,4,3,5,0,new_time);
}

// 26) Probar que, en modo de ajuste, se puede incrementar el valor de las horas de 22 a 23
void test_increment_the_value_of_the_hours_from_22_to_23 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,2},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,4,3,5,0,new_time);
}

// 27)  Probar que el incremento de horas realiza un ciclo al alcanzar el valor máximo (23 a 00)
void test_increment_the_value_of_the_hours_from_23_to_00 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,3},
        .time.minutes = {5,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockIncrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(0,0,5,1,5,0,new_time);
}

// 28) Probar que, en modo de ajuste, se puede decrementar el valor de las horas (mediante la señal de "F3")
void test_decrement_the_value_of_the_hours (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,5},
        .time.minutes = {4,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,4,4,1,5,0,new_time);
}

// 29) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 10 a 09
void test_decrement_the_value_of_the_hours_from_10_to_09 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {1,0},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(0,9,4,3,5,0,new_time);
}

// 30) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 20 a 19
void test_decrement_the_value_of_the_hours_from_20_to_19 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {2,0},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(1,9,4,3,5,0,new_time);
}

// 31) Probar que, en modo de ajuste, se puede decrementar el valor de las horas de 01 a 00
void test_decrement_the_value_of_the_hours_from_01_to_00 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {0,1},
        .time.minutes = {4,3},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(0,0,4,3,5,0,new_time);
}

// 32)  Probar que el incremento de horas realiza un ciclo al alcanzar el valor máximo (00 a 23)
void test_decrement_the_value_of_the_hours_from_00_to_23 (void){
    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    static const clock_time_t current_time = {
        .time.hours = {0,0},
        .time.minutes = {5,1},
        .time.seconds = {5,0},
    };

    clock_time_t new_time;

    ClockSetTime(clock, &current_time);
    ClockDecrementHours(clock);
    ClockGetTime(clock, &new_time);

    TEST_ASSERT_TIME(2,3,5,1,5,0,new_time);
}
 
/* === End of documentation ======================================================================================== */