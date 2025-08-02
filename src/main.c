/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file main.c
 * @brief Código Fuente principal del proyecto (Por ahora son solo pruebas)
 *
 */
/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include "chip.h"
#include "clock.h"
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
 * @brief Función que permite configurar el Systick
 *
 */
static void SystickConfig(void);

/**
 * @brief Función que permite simular el encendido del sonido de la alarma
 *
 * @param clock Puntero a la estructura con los datos del reloj
 */
static void ClockAlarmTurnOn(clock_t clock);

/**
 * @brief Función que permite simular el apagado del sonido de la alarma
 *
 * @param clock Puntero a la estructura con los datos del reloj
 */
static void ClockAlarmTurnOff(clock_t clock);

/* === Public variable definitions ============================================================= */

//! Variable global que representa a la placa
static const struct board_s* board = NULL;

//! Variable global que representa al reloj interno
static clock_t clock = NULL;

//! Variable global que representa el estado actual del reloj despertador
static clock_state_t current_state = STATE_SHOWING_CURRENT_TIME;

//! Variable global que indica cuando se está en estado de ajustar hora
static volatile bool enter_adjusting_mode = false;

//! Variable global que indica cuando se está en estado de ajustar alarma
static volatile bool enter_adjusting_alarm_mode = false;

//! Variable global que detecta que F4 fue pulsado por más de 50 ms (para evitar rebotes)
static volatile bool F4_was_pressed = false;

//! Variable global que detecta que F3 fue pulsado por más de 50 ms (para evitar rebotes)
static volatile bool F3_was_pressed = false;

//! Variable global que detecta que "Aceptar" fue pulsado por más de 50 ms (para evitar rebotes)
static volatile bool key_accept_was_pressed = false;

//! Variable global que detecta que "Aceptar" fue pulsado por más de 50 ms (para evitar rebotes)
static volatile bool key_cancel_was_pressed = false;

//! Variable global que detecta que no se pulsó ningún botón por 30 segundos
static volatile bool no_button_was_pressed_for_30secs = false;

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

static void SystickConfig(void) {
    // Genera un interrupción cada 1 ms y se ejcuta el código de Systick_Handler (1 Tick = 1 ms)
    SystemCoreClockUpdate();
    SysTick_Config((SystemCoreClock / 1000) - 1);

    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

static void ClockAlarmTurnOn(clock_t self) {
    if(exit_adjusting_alarm){
        DigitalOutputActivate(board->led_alarm);
    }
}

static void ClockAlarmTurnOff(clock_t self) {
    DigitalOutputDeactivate(board->led_alarm);
}

/* === Public function implementation ========================================================== */

//! Función que se ejecuta cada 1 ms (Rutina de servicio de interrupción del Systick)
void SysTick_Handler(void) {

    static int ticks_F1_was_pressed = 0;
    static int ticks_F2_was_pressed = 0;
    static int ticks_F3_was_pressed = 0;
    static int ticks_F4_was_pressed = 0;

    static int ticks_key_accept_was_pressed = 0;
    static int ticks_key_cancel_was_pressed = 0;

    static int ticks_no_button_was_pressed = 0;

    if (board != NULL) {
        ScreenRefresh(board->screen);
    }

    if (clock != NULL) {
        ClockTick(clock);
    }

    // Controla si el botón F1 se mantuvo pulsado por 3 segundos
    if (DigitalInputGetIsActive(board->key_F1)) {
        ticks_F1_was_pressed++;

        if (ticks_F1_was_pressed == 3000) {
            enter_adjusting_mode = true;
        }
    } else {
        ticks_F1_was_pressed = 0;
    }

    // Controla si el botón F2 se mantuvo pulsado por 3 segundos
    if(current_state == STATE_SHOWING_CURRENT_TIME){
        if (DigitalInputGetIsActive(board->key_F2)) {
        ticks_F2_was_pressed++;

        if (ticks_F2_was_pressed == 3000) {
            enter_adjusting_alarm_mode = true;
        }
    } else {
        ticks_F2_was_pressed = 0;
    }
    }else{
        ticks_F2_was_pressed = 0;
        enter_adjusting_alarm_mode = false;
    }
    


    // Controla si el botón F3 se mantuvo presionado por mas de 50ms (para evitar rebotes)
    if (DigitalInputGetIsActive(board->key_F3)) {
        ticks_F3_was_pressed++;

        if (ticks_F3_was_pressed == 50) {
            F3_was_pressed = true;
            ticks_no_button_was_pressed = 0;
        }
    } else {
        ticks_F3_was_pressed = 0;
    }

    // Controla si el botón F4 se mantuvo presionado por mas de 50ms (para evitar rebotes)
    if (DigitalInputGetIsActive(board->key_F4)) {
        ticks_F4_was_pressed++;

        if (ticks_F4_was_pressed == 50) {
            F4_was_pressed = true;
            ticks_no_button_was_pressed = 0;
        }
    } else {
        ticks_F4_was_pressed = 0;
    }

    // Controla si el botón Aceptar se mantuvo presionado por mas de 50ms (para evitar rebotes)
    if (DigitalInputGetIsActive(board->key_accept)) {
        ticks_key_accept_was_pressed++;

        if (ticks_key_accept_was_pressed == 50) {
            key_accept_was_pressed = true;
            ticks_no_button_was_pressed = 0;
        }
    } else {
        ticks_key_accept_was_pressed = 0;
    }

    // Controla si el botón Cancelar se mantuvo presionado por mas de 50ms (para evitar rebotes)
    if (DigitalInputGetIsActive(board->key_cancel)) {
        ticks_key_cancel_was_pressed++;

        if (ticks_key_cancel_was_pressed == 50) {
            key_cancel_was_pressed = true;
            ticks_no_button_was_pressed = 0;
        }
    } else {
        ticks_key_cancel_was_pressed = 0;
    }

    // Controla si han pasado 30 segundos sin que se presione ningún botón
    if (current_state == STATE_ADJUSTING_TIME_MINUTES || current_state == STATE_ADJUSTING_TIME_HOURS
        || current_state == STATE_ADJUSTING_ALARM_MINUTES || current_state == STATE_ADJUSTING_ALARM_HOURS) {

        ticks_no_button_was_pressed++;

        if (ticks_no_button_was_pressed == 30000) {
            ticks_no_button_was_pressed = 0;
            no_button_was_pressed_for_30secs = true;
        }

    } else {
        ticks_no_button_was_pressed = 0;
        no_button_was_pressed_for_30secs = false;
    }
}

//! Programa principal con la aplicación deseada
int main(void) {

    SystickConfig();

    clock_time_t current_time;
    clock_time_t adjusted_time;
    clock_time_t alarm_time;
    clock_time_t adjusted_alarm_time;

    memset(&alarm_time, 0, sizeof(alarm_time));

    bool valid_time;

    board = BoardCreate();
    clock = ClockCreate(50, 300, &driver);
                    
    while (true) { // CORREGIR: PARPADEO RARO EN ALGUNOS MOMENTOS
                   // ACORDARSE DE PONER EN FALSE ALARM_IS_ACTIVATED CUANDO SE DESACTIVE

        switch (current_state) {

            case STATE_INVALID_TIME:
                ScreenWriteBCD(board->screen, current_time.bcd, 4);
                ScreenFlashDigits(board->screen, 0, 3, 125);

                ScreenSetDotState(board->screen, 2, true);
                ScreenFlashDot(board->screen, 2, 125);

                if (enter_adjusting_mode) {
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

                if (enter_adjusting_mode) {
                    adjusted_time = current_time;
                    current_state = STATE_ADJUSTING_TIME_MINUTES;
                }

                if (enter_adjusting_alarm_mode) {
                    adjusted_alarm_time = alarm_time;
                    exit_adjusting_alarm = false;
                    current_state = STATE_ADJUSTING_ALARM_MINUTES;
                }

                if(!ClockGetIfAlarmIsRinging(clock)){
                    if(key_accept_was_pressed){
                        key_accept_was_pressed = false;
                        alarm_is_activated = true;
                        ClockSetAlarm(clock, &alarm_time);
                    }

                    if(key_cancel_was_pressed){
                        key_cancel_was_pressed = false;
                        alarm_is_activated = false;
                        ClockDisableAlarm(clock);
                    }
                }
                

                if(ClockGetIfAlarmIsActivated(clock)){
                    ScreenWriteBCD(board->screen, current_time.bcd, 4);

                    ScreenSetDotState(board->screen, 2, true);
                    ScreenSetDotState(board->screen, 0, true);
                    ScreenFlashDot(board->screen, 2, 125);
                }

                if(ClockGetIfAlarmIsRinging(clock)){
                    if(key_accept_was_pressed){
                        key_accept_was_pressed = false;
                        ClockSnoozeAlarm(clock);
                    }

                    if(key_cancel_was_pressed){
                        key_cancel_was_pressed = false;
                        ClockCancelAlarm(clock);
                    }
                }

                break;

            case STATE_ADJUSTING_TIME_MINUTES:
                ScreenFlashDot(board->screen, 2, 0);
                ScreenFlashDigits(board->screen, 2, 3, 125);

                if (key_cancel_was_pressed || no_button_was_pressed_for_30secs) {
                    ClockSetTime(clock, &current_time);
                    key_cancel_was_pressed = false;
                    no_button_was_pressed_for_30secs = false;
                    enter_adjusting_mode = false;
                    if(valid_time){
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    }else{
                        current_state = STATE_INVALID_TIME;
                    }
                } else {
                    if (F4_was_pressed) {
                        F4_was_pressed = false;
                        ClockSetTime(clock, &adjusted_time);
                        ClockIncrementMinutes(clock);
                        ClockGetTime(clock, &adjusted_time);

                        ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                        ScreenSetDotState(board->screen, 2, true);
                    }

                    if (F3_was_pressed) {
                        F3_was_pressed = false;
                        ClockSetTime(clock, &adjusted_time);
                        ClockDecrementMinutes(clock);
                        ClockGetTime(clock, &adjusted_time);

                        ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                        ScreenSetDotState(board->screen, 2, true);
                    }

                    if (key_accept_was_pressed) {
                        key_accept_was_pressed = false;
                        current_state = STATE_ADJUSTING_TIME_HOURS;
                    }
                }

                break;

            case STATE_ADJUSTING_TIME_HOURS:
                ScreenFlashDot(board->screen, 2, 0);
                ScreenFlashDigits(board->screen, 0, 1, 125);

                if (key_cancel_was_pressed || no_button_was_pressed_for_30secs) {
                    ClockSetTime(clock, &current_time);
                    enter_adjusting_mode = false;
                    key_cancel_was_pressed = false;
                    no_button_was_pressed_for_30secs = false;
                    if(valid_time){
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    }else{
                        current_state = STATE_INVALID_TIME;
                    }
                } else {
                    if (F4_was_pressed) {
                        F4_was_pressed = false;
                        ClockSetTime(clock, &adjusted_time);
                        ClockIncrementHours(clock);
                        ClockGetTime(clock, &adjusted_time);

                        ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                        ScreenSetDotState(board->screen, 2, true);
                    }

                    if (F3_was_pressed) {
                        F3_was_pressed = false;
                        ClockSetTime(clock, &adjusted_time);
                        ClockDecrementHours(clock);
                        ClockGetTime(clock, &adjusted_time);

                        ScreenWriteBCD(board->screen, adjusted_time.bcd, 4);
                        ScreenSetDotState(board->screen, 2, true);
                    }

                    if (key_accept_was_pressed) {
                        key_accept_was_pressed = false;
                        enter_adjusting_mode = false;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    }
                }

                break;

            case STATE_ADJUSTING_ALARM_MINUTES:
                ClockGetAlarm(clock, &adjusted_alarm_time);
                ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                ScreenFlashDigits(board->screen, 2, 3, 125);

                ScreenFlashDot(board->screen, 2, 0);
                ScreenSetDotState(board->screen, 0, true);
                ScreenSetDotState(board->screen, 1, true);
                ScreenSetDotState(board->screen, 2, true);
                ScreenSetDotState(board->screen, 3, true);

                if (key_cancel_was_pressed || no_button_was_pressed_for_30secs) {
                    if(alarm_is_activated){
                        ClockSetAlarm(clock, &alarm_time);
                    }else{
                        ClockDisableAlarm(clock);
                    }
                    enter_adjusting_alarm_mode = false;
                    key_cancel_was_pressed = false;
                    no_button_was_pressed_for_30secs = false;
                    current_state = STATE_SHOWING_CURRENT_TIME;
                    exit_adjusting_alarm = true;
                }else{
                    if (F4_was_pressed) {
                        F4_was_pressed = false;
                        exit_adjusting_alarm = false;
                        ClockSetAlarm(clock, &adjusted_alarm_time);
                        ClockIncrementAlarmMinutes(clock);
                        ClockGetAlarm(clock, &adjusted_alarm_time);
                        ClockDisableAlarm(clock);

                        ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                    }

                    if (F3_was_pressed) {
                        F3_was_pressed = false;
                        exit_adjusting_alarm = false;
                        ClockSetAlarm(clock, &adjusted_alarm_time);
                        ClockDecrementAlarmMinutes(clock);
                        ClockGetAlarm(clock, &adjusted_alarm_time);
                        ClockDisableAlarm(clock);

                        ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                    }

                    if (key_accept_was_pressed) {
                        key_accept_was_pressed = false;
                        current_state = STATE_ADJUSTING_ALARM_HOURS;
                        exit_adjusting_alarm = false;
                    }
                }

                break;
                
            case STATE_ADJUSTING_ALARM_HOURS:
                ScreenFlashDigits(board->screen, 0, 1, 125);

                ScreenSetDotState(board->screen, 0, true);
                ScreenSetDotState(board->screen, 1, true);
                ScreenSetDotState(board->screen, 2, true);
                ScreenSetDotState(board->screen, 3, true);

                if (key_cancel_was_pressed || no_button_was_pressed_for_30secs) {
                    if(alarm_is_activated){
                        ClockSetAlarm(clock, &alarm_time);
                    }else{
                        ClockDisableAlarm(clock);
                    }
                    enter_adjusting_alarm_mode = false;
                    key_cancel_was_pressed = false;
                    no_button_was_pressed_for_30secs = false;
                    exit_adjusting_alarm = true;
                    current_state = STATE_SHOWING_CURRENT_TIME;
                } else {
                    if (F4_was_pressed) {
                        F4_was_pressed = false;
                        exit_adjusting_alarm = false;
                        ClockSetAlarm(clock, &adjusted_alarm_time);
                        ClockIncrementAlarmHours(clock);
                        ClockGetAlarm(clock, &adjusted_alarm_time);
                        ClockDisableAlarm(clock);

                        ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                    }

                    if (F3_was_pressed) {
                        F3_was_pressed = false;
                        exit_adjusting_alarm = false;
                        ClockSetAlarm(clock, &adjusted_alarm_time);
                        ClockDecrementAlarmHours(clock);
                        ClockGetAlarm(clock, &adjusted_alarm_time);
                        ClockDisableAlarm(clock);

                        ScreenWriteBCD(board->screen, adjusted_alarm_time.bcd, 4);
                    }

                    if (key_accept_was_pressed) {
                        alarm_time = adjusted_alarm_time;
                        ClockSetAlarm(clock, &alarm_time);
                        key_accept_was_pressed = false;
                        enter_adjusting_alarm_mode = false;
                        alarm_is_activated = true;
                        exit_adjusting_alarm = true;
                        current_state = STATE_SHOWING_CURRENT_TIME;
                    }
                }

                break;
        }
    }
}

/* === End of documentation ==================================================================== */
