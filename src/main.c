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

#include "bsp.h"
#include "chip.h"
#include "clock.h"
#include "key_controller.h"
#include <stdbool.h>
#include <string.h>

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

//! Tipo de dato que representa el estado del reloj
typedef enum clock_state_e {
    STATE_INVALID_TIME,            //!< Indica que la hora es inválida
    STATE_SHOWING_CURRENT_TIME,    //!< Indica que se está mostrando la hora actual
    STATE_ADJUSTING_TIME_MINUTES,  //!< Indica que se están ajustando los minutos
    STATE_ADJUSTING_TIME_HOURS,    //!< Indica que se están ajustando las horas
    STATE_ADJUSTING_ALARM_MINUTES, //!< Indica que se están ajustando los minutos de la alarma
    STATE_ADJUSTING_ALARM_HOURS,   //!< Indica que se están ajustando las horas de la alarma
} clock_state_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Función que permite configurar el Systick. Genera una interrupción cada 1ms
 *
 */
static void SystickConfig(void);

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

/**
 * @brief Función que permite saber si ningún botón fue presionado por 30 segundos
 *
 * @return true Si ningún botón fue presionado por 30 segundos
 * @return false Si se presionó algún botón antes de que pasen 30 segundos
 */
static bool NoButtonPressedFor30secs(void);

/* === Public variable definitions ============================================================= */

//! Variable global que representa a la placa
static const struct board_s* board = NULL;

//! Variable global que representa al reloj interno
static clock_t clock = NULL;

//! Variable global que representa el estado actual del reloj despertador
static clock_state_t current_state = STATE_SHOWING_CURRENT_TIME;

//! Variable global que cuenta los milisegundos transcurridos desde que se inicia el programa
static volatile uint64_t milis = 0;

//! Variable global que cuenta la cantidad de milisegundos que pasaron sin que se pulse ningún botón
static volatile int ticks_no_button_was_pressed = 0;

//! Variable global que indica que la alarma está activada (no que está sonando)
static volatile bool alarm_is_activated = false;

//! Variable global que detecta que se salió del modo ajuste de alarma
static volatile bool exit_adjusting_alarm = false;

//! Estructura constante que representa el driver del reloj con las funciones de callback
static const struct clock_alarm_driver_s driver = {
    .ClockAlarmTurnOn = ClockAlarmTurnOn,
    .ClockAlarmTurnOff = ClockAlarmTurnOff,
};

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

// Genera un interrupción cada 1 ms y se ejcuta el código de Systick_Handler (1 Tick = 1 ms)
static void SystickConfig(void) {
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    // NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

static void ClockAlarmTurnOn(void) {
    if (exit_adjusting_alarm) {
        DigitalOutputActivate(board->led_alarm);
    }
}

static void ClockAlarmTurnOff(void) {
    DigitalOutputDeactivate(board->led_alarm);
}

static bool NoButtonPressedFor30secs(void) {
    bool result = false;

    if (ticks_no_button_was_pressed >= 30000) {
        result = true;
    }

    return result;
}

/* === Public function implementation ========================================================== */

//! Función que se ejecuta cada 1 ms (Rutina de servicio de interrupción del Systick)
void SysTick_Handler(void) {
    milis++;

    if (board != NULL) {
        ScreenRefresh(board->screen);
    }

    if (clock != NULL) {
        ClockTick(clock);
    }
}

//! Programa principal con la aplicación deseada
int main(void) {
    SystickConfig();

    uint64_t aux_milis = 0;

    clock_time_t current_time;
    clock_time_t adjusted_time;
    clock_time_t alarm_time;
    clock_time_t adjusted_alarm_time;

    memset(&alarm_time, 0, sizeof(alarm_time));

    bool valid_time;

    board = BoardCreate();
    clock = ClockCreate(1000, 300, &driver);

    button_t key_set_time = ButtonCreate(board->key_F1);
    button_t key_set_alarm = ButtonCreate(board->key_F2);
    button_t key_increment = ButtonCreate(board->key_F4);
    button_t key_decrement = ButtonCreate(board->key_F3);
    button_t key_cancel = ButtonCreate(board->key_cancel);
    button_t key_accept = ButtonCreate(board->key_accept);

    while (true) {

        if (milis - aux_milis == 1) {
            aux_milis = milis;

            switch (current_state) {

                case STATE_INVALID_TIME:
                    ScreenWriteBCD(board->screen, current_time.bcd, 4);
                    ScreenFlashDigits(board->screen, 0, 3, 125);

                    ScreenSetDotState(board->screen, 2, true);
                    ScreenFlashDot(board->screen, 2, 125);

                    if (ButtonWasPressed3secs(key_set_time)) {
                        adjusted_time = current_time;
                        current_state = STATE_ADJUSTING_TIME_MINUTES;
                    }

                    break;

                case STATE_SHOWING_CURRENT_TIME:
                    valid_time = ClockGetTime(clock, &current_time);

                    if (valid_time) {
                        ScreenWriteBCD(board->screen, current_time.bcd, 4);
                        ScreenFlashDigits(board->screen, 0, 3, 0);

                        ScreenSetDotState(board->screen, 2, true);
                        ScreenFlashDot(board->screen, 2, 125);
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

                    if (!ClockGetIfAlarmIsRinging(clock)) {
                        if (ButtonWasPressed(key_accept)) {
                            alarm_is_activated = true;
                            ClockSetAlarm(clock, &alarm_time);
                        }

                        if (ButtonWasPressed(key_cancel)) {
                            alarm_is_activated = false;
                            ClockDisableAlarm(clock);
                        }
                    }

                    if (ClockGetIfAlarmIsActivated(clock)) {
                        ScreenWriteBCD(board->screen, current_time.bcd, 4);

                        ScreenSetDotState(board->screen, 2, true);
                        ScreenSetDotState(board->screen, 0, true);
                        ScreenFlashDot(board->screen, 2, 125);
                    }

                    if (ClockGetIfAlarmIsRinging(clock)) {
                        if (ButtonWasPressed(key_accept)) {
                            ClockSnoozeAlarm(clock);
                        }

                        if (ButtonWasPressed(key_cancel)) {
                            ClockCancelAlarm(clock);
                        }
                    }

                    break;

                case STATE_ADJUSTING_TIME_MINUTES:
                    ticks_no_button_was_pressed++;

                    ScreenFlashDot(board->screen, 2, 0);
                    ScreenFlashDigits(board->screen, 2, 3, 125);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        ClockSetTime(clock, &current_time);
                        ticks_no_button_was_pressed = 0;
                        if (valid_time) {
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        } else {
                            current_state = STATE_INVALID_TIME;
                        }
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(clock, &adjusted_time);
                            ClockIncrementMinutes(clock);
                            ClockGetTime(clock, &adjusted_time);

                            ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(board->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(clock, &adjusted_time);
                            ClockDecrementMinutes(clock);
                            ClockGetTime(clock, &adjusted_time);

                            ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(board->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            ticks_no_button_was_pressed = 0;
                            current_state = STATE_ADJUSTING_TIME_HOURS;
                        }
                    }

                    break;

                case STATE_ADJUSTING_TIME_HOURS:
                    ticks_no_button_was_pressed++;

                    ScreenFlashDot(board->screen, 2, 0);
                    ScreenFlashDigits(board->screen, 0, 1, 125);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        ClockSetTime(clock, &current_time);
                        ticks_no_button_was_pressed = 0;
                        if (valid_time) {
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        } else {
                            current_state = STATE_INVALID_TIME;
                        }
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(clock, &adjusted_time);
                            ClockIncrementHours(clock);
                            ClockGetTime(clock, &adjusted_time);

                            ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(board->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            ClockSetTime(clock, &adjusted_time);
                            ClockDecrementHours(clock);
                            ClockGetTime(clock, &adjusted_time);

                            ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                            ScreenSetDotState(board->screen, 2, true);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            ticks_no_button_was_pressed = 0;
                            current_state = STATE_SHOWING_CURRENT_TIME;
                        }
                    }

                    break;

                case STATE_ADJUSTING_ALARM_MINUTES:
                    ticks_no_button_was_pressed++;

                    ClockGetAlarm(clock, &adjusted_alarm_time);
                    ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                    ScreenFlashDigits(board->screen, 2, 3, 125);

                    ScreenFlashDot(board->screen, 2, 0);
                    ScreenSetDotState(board->screen, 0, true);
                    ScreenSetDotState(board->screen, 1, true);
                    ScreenSetDotState(board->screen, 2, true);
                    ScreenSetDotState(board->screen, 3, true);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        if (alarm_is_activated) {
                            ClockSetAlarm(clock, &alarm_time);
                        } else {
                            ClockDisableAlarm(clock);
                        }
                        ticks_no_button_was_pressed = 0;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                        exit_adjusting_alarm = true;
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(clock, &adjusted_alarm_time);
                            ClockIncrementAlarmMinutes(clock);
                            ClockGetAlarm(clock, &adjusted_alarm_time);
                            ClockDisableAlarm(clock);

                            ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(clock, &adjusted_alarm_time);
                            ClockDecrementAlarmMinutes(clock);
                            ClockGetAlarm(clock, &adjusted_alarm_time);
                            ClockDisableAlarm(clock);

                            ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
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

                    ScreenFlashDigits(board->screen, 0, 1, 125);

                    ScreenSetDotState(board->screen, 0, true);
                    ScreenSetDotState(board->screen, 1, true);
                    ScreenSetDotState(board->screen, 2, true);
                    ScreenSetDotState(board->screen, 3, true);

                    if (ButtonWasPressed(key_cancel) || NoButtonPressedFor30secs()) {
                        if (alarm_is_activated) {
                            ClockSetAlarm(clock, &alarm_time);
                        } else {
                            ClockDisableAlarm(clock);
                        }
                        ticks_no_button_was_pressed = 0;
                        exit_adjusting_alarm = true;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    } else {
                        if (ButtonWasPressed(key_increment)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(clock, &adjusted_alarm_time);
                            ClockIncrementAlarmHours(clock);
                            ClockGetAlarm(clock, &adjusted_alarm_time);
                            ClockDisableAlarm(clock);

                            ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_decrement)) {
                            ticks_no_button_was_pressed = 0;
                            exit_adjusting_alarm = false;
                            ClockSetAlarm(clock, &adjusted_alarm_time);
                            ClockDecrementAlarmHours(clock);
                            ClockGetAlarm(clock, &adjusted_alarm_time);
                            ClockDisableAlarm(clock);

                            ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                        }

                        if (ButtonWasPressed(key_accept)) {
                            alarm_time = adjusted_alarm_time;
                            ClockSetAlarm(clock, &alarm_time);
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

/* === End of documentation ==================================================================== */
