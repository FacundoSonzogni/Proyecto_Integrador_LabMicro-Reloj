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

#define SET_TIME_BUTTON  KEY_EVENT_KEY_0 //!< Representa que el evento generado por el "set_time" corresponde al bit 0 del grupo de eventos
#define INCREMENT_BUTTON KEY_EVENT_KEY_1 //!< Representa que el evento generado por el "increment" corresponde al bit 1 del grupo de eventos
#define DECREMENT_BUTTON KEY_EVENT_KEY_2 //!< Representa que el evento generado por el "decrement" corresponde al bit 2 del grupo de eventos
#define ACCEPT_BUTTON    KEY_EVENT_KEY_3 //!< Representa que el evento generado por el "accept" corresponde al bit 3 del grupo de eventos
#define CANCEL_BUTTON    KEY_EVENT_KEY_4 //!< Representa que el evento generado por el "cancel" corresponde al bit 4 del grupo de eventos
#define SET_ALARM_BUTTON KEY_EVENT_KEY_5 //!< Representa que el evento generado por el "set_alarm" corresponde al bit 5 del grupo de eventos

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

//! Estructura constante que representa el driver del reloj con las funciones de callback
static const struct clock_alarm_driver_s driver = {
    .ClockAlarmTurnOn = ClockAlarmTurnOn,
    .ClockAlarmTurnOff = ClockAlarmTurnOff,
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void ClockAlarmTurnOn(void) {
    DigitalOutputActivate(board->led_alarm);
}

static void ClockAlarmTurnOff(void) {
    DigitalOutputDeactivate(board->led_alarm);
}

/* === Public function implementation ========================================================== */

//! Programa principal con la aplicación deseada
int main(void) {

    EventGroupHandle_t buttons_events;
    BaseType_t result;

    board = BoardCreate();
    clock = ClockCreate(1000, 300, &driver);

    buttons_events = xEventGroupCreate();

    /*================= Creación de todas las tareas correspondientes a los botones ==================*/
    if (buttons_events != NULL) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = SET_TIME_BUTTON;
        buttons_args->key = board->key_F1;
        result = xTaskCreate(ButtonPressed3secsTask, "SetTimeTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = INCREMENT_BUTTON;
        buttons_args->key = board->key_F4;
        result = xTaskCreate(ButtonPressedTask, "IncrementTimeTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = DECREMENT_BUTTON;
        buttons_args->key = board->key_F3;
        result = xTaskCreate(ButtonPressedTask, "DecrementTimeTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = ACCEPT_BUTTON;
        buttons_args->key = board->key_accept;
        result = xTaskCreate(ButtonPressedTask, "AcceptPressedTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = CANCEL_BUTTON;
        buttons_args->key = board->key_cancel;
        result = xTaskCreate(ButtonPressedTask, "CancelPressedTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_task_args_t buttons_args = malloc(sizeof(*buttons_args));
        buttons_args->event_group = buttons_events;
        buttons_args->event_mask = SET_ALARM_BUTTON;
        buttons_args->key = board->key_F2;
        result = xTaskCreate(ButtonPressed3secsTask, "SetAlarmTask", KEY_TASK_STACK_SIZE, buttons_args, tskIDLE_PRIORITY + 1, NULL);
    }

    /* ====================== Creación de la tarea correspondiente a la MEF ========================== */

    if (result == pdPASS) {
        mef_task_args_t mef_args = malloc(sizeof(*mef_args));
        mef_args->board = board;
        mef_args->clock = clock;
        mef_args->set_time_mask = SET_TIME_BUTTON;
        mef_args->increment_mask = INCREMENT_BUTTON;
        mef_args->decrement_mask = DECREMENT_BUTTON;
        mef_args->accept_mask = ACCEPT_BUTTON;
        mef_args->cancel_mask = CANCEL_BUTTON;
        mef_args->set_alarm_mask = SET_ALARM_BUTTON;
        mef_args->event_group = buttons_events;

        result = xTaskCreate(MEFTask, "MEFTask", configMINIMAL_STACK_SIZE, mef_args, tskIDLE_PRIORITY + 2, NULL);
    }

    /* ============== Creación de la tarea correspondiente al Refresco de Pantalla  ================== */

    if (result == pdPASS) {
        result = xTaskCreate(ScreenRefreshTask, "ScreenRefresh", configMINIMAL_STACK_SIZE, board->screen, tskIDLE_PRIORITY + 3, NULL);
    }

    /* =============== Creación de la tarea correspondiente al Refresco del Reloj  =================== */

    if (result == pdPASS) {
        result = xTaskCreate(ClockTickTask, "ClockTick", configMINIMAL_STACK_SIZE, clock, tskIDLE_PRIORITY + 4, NULL);
    }

    vTaskStartScheduler();

    while (1) {
    }
}

/* === End of documentation ==================================================================== */
