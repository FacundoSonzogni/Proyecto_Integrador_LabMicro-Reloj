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

/** @file AppMEF.c
 ** @brief Código fuente del módulo que contiene la Máquina de Estado Finito de la Aplicación deseada
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.H"
#include "task.h"
#include "AppMEF.h"
#include "digitals.h"
#include "chip.h"
#include "key_controller.h"
#include <stdlib.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

//! Tipo de dato que representa el estado del reloj
typedef enum clock_state_e {
    STATE_INVALID_TIME,            //!< Indica que la hora es inválida
    STATE_SHOWING_CURRENT_TIME,    //!< Indica que se está mostrando la hora actual
    STATE_ADJUSTING_TIME_MINUTES,  //!< Indica que se están ajustando los minutos
    STATE_ADJUSTING_TIME_HOURS,    //!< Indica que se están ajustando las horas
    STATE_ADJUSTING_ALARM_MINUTES, //!< Indica que se están ajustando los minutos de la alarma
    STATE_ADJUSTING_ALARM_HOURS,   //!< Indica que se están ajustando las horas de la alarma
} clock_state_t;

/* === Private function declarations =============================================================================== */

/**
 * @brief Función que permite saber si ningún botón fue presionado por 30 segundos
 *
 * @return true Si ningún botón fue presionado por 30 segundos
 * @return false Si se presionó algún botón antes de que pasen 30 segundos
 */
static bool NoButtonPressedFor30secs(void);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

//! Variable global que representa el estado actual del reloj despertador
static clock_state_t current_state = STATE_SHOWING_CURRENT_TIME;

//! Variable global que cuenta la cantidad de milisegundos que pasaron sin que se pulse ningún botón
static volatile int ticks_no_button_was_pressed = 0;

//! Variable global que indica que la alarma está activada (no que está sonando)
static volatile bool alarm_is_activated = false;

//! Variable global que detecta que se salió del modo ajuste de alarma
volatile bool exit_adjusting_alarm = false;


/* === Private function definitions ================================================================================ */

static bool NoButtonPressedFor30secs(void) {
    bool result = false;

    if (ticks_no_button_was_pressed >= 30000) {
        result = true;
    }

    return result;
}

/* === Public function definitions ================================================================================= */

void MEFTask(void *pointer){

    mef_task_args_t args = pointer;

    TickType_t aux_milis = 0;
    TickType_t milis;

    clock_time_t current_time;
    clock_time_t adjusted_time;
    clock_time_t alarm_time;
    clock_time_t adjusted_alarm_time;

    memset(&alarm_time, 0, sizeof(alarm_time));

    bool valid_time;

    button_t key_set_time = ButtonCreate(((board_t)args->board)->key_F1);
    button_t key_set_alarm = ButtonCreate(((board_t)args->board)->key_F2);
    button_t key_increment = ButtonCreate(((board_t)args->board)->key_F4);
    button_t key_decrement = ButtonCreate(((board_t)args->board)->key_F3);
    button_t key_cancel = ButtonCreate(((board_t)args->board)->key_cancel);
    button_t key_accept = ButtonCreate(((board_t)args->board)->key_accept);

    while (true) {

        milis = xTaskGetTickCount();

        if (milis - aux_milis == 1) {
            aux_milis = milis;

            switch (current_state) {

                case STATE_INVALID_TIME:
                    ScreenWriteBCD(((board_t)args->board)->screen, current_time.bcd, 4);
                    ScreenFlashDigits(((board_t)args->board)->screen, 0, 3, 125);

                    ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                    ScreenFlashDot(((board_t)args->board)->screen, 2, 125);

                    if (ButtonWasPressed3secs(key_set_time)) {
                        adjusted_time = current_time;
                        current_state = STATE_ADJUSTING_TIME_MINUTES;
                    }

                    break;

                case STATE_SHOWING_CURRENT_TIME:
                    valid_time = ClockGetTime(((clock_t)args->clock), &current_time);

                    if (valid_time) {
                        ScreenWriteBCD(((board_t)args->board)->screen, current_time.bcd, 4);
                        ScreenFlashDigits(((board_t)args->board)->screen, 0, 3, 0);

                        ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        ScreenFlashDot(((board_t)args->board)->screen, 2, 125);
                    } else {
                        current_state = STATE_INVALID_TIME;
                    }

                    if (ButtonWasPressed3secs(key_set_time)) {
                        adjusted_time = current_time;
                        current_state = STATE_ADJUSTING_TIME_MINUTES;
                    }

                    if (ButtonWasPressed3secs(key_set_alarm)) {
                        adjusted_alarm_time = alarm_time;
                        exit_adjusting_alarm = false;
                        current_state = STATE_ADJUSTING_ALARM_MINUTES;
                    }

                    if (!ClockGetIfAlarmIsRinging(((clock_t)args->clock))) {
                        if (ButtonWasPressed(key_accept)) {
                            alarm_is_activated = true;
                            ClockSetAlarm(((clock_t)args->clock), &alarm_time);
                        }

                        if (ButtonWasPressed(key_cancel)) {
                            alarm_is_activated = false;
                            ClockDisableAlarm(((clock_t)args->clock));
                        }
                    }

                    if (ClockGetIfAlarmIsActivated(((clock_t)args->clock))) {
                        ScreenWriteBCD(((board_t)args->board)->screen, current_time.bcd, 4);

                        ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        ScreenSetDotState(((board_t)args->board)->screen, 0, true);
                        ScreenFlashDot(((board_t)args->board)->screen, 2, 125);
                    }

                    if (ClockGetIfAlarmIsRinging(((clock_t)args->clock))) {
                        if (ButtonWasPressed(key_accept)) {
                            ClockSnoozeAlarm(((clock_t)args->clock));
                        }

                        if (ButtonWasPressed(key_cancel)) {
                            ClockCancelAlarm(((clock_t)args->clock));
                        }
                    }

                    break;

                case STATE_ADJUSTING_TIME_MINUTES:
                    ticks_no_button_was_pressed++;

                    ScreenFlashDot(((board_t)args->board)->screen, 2, 0);
                    ScreenFlashDigits(((board_t)args->board)->screen, 2, 3, 125);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        ClockSetTime(((clock_t)args->clock), &current_time);
                        ticks_no_button_was_pressed = 0;
                        if (valid_time) {
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        } else {
                            current_state = STATE_INVALID_TIME;
                        }
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(((clock_t)args->clock), &adjusted_time);
                            ClockIncrementMinutes(((clock_t)args->clock));
                            ClockGetTime(((clock_t)args->clock), &adjusted_time);

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(((clock_t)args->clock), &adjusted_time);
                            ClockDecrementMinutes(((clock_t)args->clock));
                            ClockGetTime(((clock_t)args->clock), &adjusted_time);

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            ticks_no_button_was_pressed = 0;
                            current_state = STATE_ADJUSTING_TIME_HOURS;
                        }
                    }

                    break;

                case STATE_ADJUSTING_TIME_HOURS:
                    ticks_no_button_was_pressed++;

                    ScreenFlashDot(((board_t)args->board)->screen, 2, 0);
                    ScreenFlashDigits(((board_t)args->board)->screen, 0, 1, 125);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        ClockSetTime(((clock_t)args->clock), &current_time);
                        ticks_no_button_was_pressed = 0;
                        if (valid_time) {
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        } else {
                            current_state = STATE_INVALID_TIME;
                        }
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(((clock_t)args->clock), &adjusted_time);
                            ClockIncrementHours(((clock_t)args->clock));
                            ClockGetTime(((clock_t)args->clock), &adjusted_time);

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(((clock_t)args->clock), &adjusted_time);
                            ClockDecrementHours(((clock_t)args->clock));
                            ClockGetTime(((clock_t)args->clock), &adjusted_time);

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            ticks_no_button_was_pressed = 0;
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        }
                    }

                    break;

                case STATE_ADJUSTING_ALARM_MINUTES:
                    ticks_no_button_was_pressed++;

                    ClockGetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                    ScreenWriteBCD(((board_t)args->board)->screen, adjusted_alarm_time.bcd, 4);
                    ScreenFlashDigits(((board_t)args->board)->screen, 2, 3, 125);

                    ScreenFlashDot(((board_t)args->board)->screen, 2, 0);
                    ScreenSetDotState(((board_t)args->board)->screen, 0, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 1, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 3, true);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        if (alarm_is_activated) {
                            ClockSetAlarm(((clock_t)args->clock), &alarm_time);
                        } else {
                            ClockDisableAlarm(((clock_t)args->clock));
                        }
                        ticks_no_button_was_pressed = 0;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                        exit_adjusting_alarm = true;
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockIncrementAlarmMinutes(((clock_t)args->clock));
                            ClockGetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDisableAlarm(((clock_t)args->clock));

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDecrementAlarmMinutes(((clock_t)args->clock));
                            ClockGetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDisableAlarm(((clock_t)args->clock));

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            ticks_no_button_was_pressed = 0;
                            current_state = STATE_ADJUSTING_ALARM_HOURS;
                            exit_adjusting_alarm = false;
                        }
                    }

                    break;

                case STATE_ADJUSTING_ALARM_HOURS:
                    ticks_no_button_was_pressed++;

                    ScreenFlashDigits(((board_t)args->board)->screen, 0, 1, 125);

                    ScreenSetDotState(((board_t)args->board)->screen, 0, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 1, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 2, true);
                    ScreenSetDotState(((board_t)args->board)->screen, 3, true);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        if (alarm_is_activated) {
                            ClockSetAlarm(((clock_t)args->clock), &alarm_time);
                        } else {
                            ClockDisableAlarm(((clock_t)args->clock));
                        }
                        ticks_no_button_was_pressed = 0;
                        exit_adjusting_alarm = true;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockIncrementAlarmHours(((clock_t)args->clock));
                            ClockGetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDisableAlarm(((clock_t)args->clock));

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDecrementAlarmHours(((clock_t)args->clock));
                            ClockGetAlarm(((clock_t)args->clock), &adjusted_alarm_time);
                            ClockDisableAlarm(((clock_t)args->clock));

                            ScreenWriteBCD(((board_t)args->board)->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            alarm_time = adjusted_alarm_time;
                            ClockSetAlarm(((clock_t)args->clock), &alarm_time);
                            ticks_no_button_was_pressed = 0;
                            alarm_is_activated = true;
                            exit_adjusting_alarm = true;
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        }
                    }

                    break;
            }
        }
    }
}

/* === End of documentation ======================================================================================== */