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

/**
 * @file main.c
 * @brief Código Fuente principal del proyecto
 *
 */
/* === Headers files inclusions =============================================================== */

#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "chip.h"
#include "clock.h"
#include "key_controller.h"
#include "AppMEF.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Función que permite simular el encendido del sonido de la alarma
 *
 * @param clock Puntero a la estructura con los datos del reloj
 */
static void ClockAlarmTurnOn(void);

/**
 * @brief Función que permite simular el apagado del sonido de la alarma
 *
 * @param clock Puntero a la estructura con los datos del reloj
 */
static void ClockAlarmTurnOff(void);

/* === Public variable definitions ============================================================= */

//! Variable global que representa a la placa
static const struct board_s* board = NULL;

//! Variable global que representa al reloj interno
static clock_t clock = NULL;

//! Variable global que detecta que se salió del modo ajuste de alarma
extern volatile bool exit_adjusting_alarm;

//! Estructura constante que representa el driver del reloj con las funciones de callback
static const struct clock_alarm_driver_s driver = {
    .ClockAlarmTurnOn = ClockAlarmTurnOn,
    .ClockAlarmTurnOff = ClockAlarmTurnOff,
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void ClockAlarmTurnOn(void) {
    if (exit_adjusting_alarm) {
        DigitalOutputActivate(board->led_alarm);
    }
}

static void ClockAlarmTurnOff(void) {
    DigitalOutputDeactivate(board->led_alarm);
}

/* === Public function implementation ========================================================== */


//! Programa principal con la aplicación deseada
int main(void) {

    board = BoardCreate();
    clock = ClockCreate(1000, 300, &driver);

    mef_task_args_t mef_args = malloc(sizeof(struct mef_task_args_s));;
    mef_args->board = board;
    mef_args->clock = clock;

    xTaskCreate(ClockTickTask, "ClockTick", configMINIMAL_STACK_SIZE, clock, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(ScreenRefreshTask, "ScreenRefresh", configMINIMAL_STACK_SIZE, board->screen, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(MEFTask, "MEF", configMINIMAL_STACK_SIZE, mef_args, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

    while (1) {
    }
}

/* === End of documentation ==================================================================== */
