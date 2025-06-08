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

/** @file bsp.c
 ** @brief Código fuente del módulo de Soporte de Placa (Board Support)
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digitals.h"
#include "chip.h"
#include "bsp.h"
#include "edu-ciaa-nxp.h"
#include "shield.h"
#include "screen.h"
#include <stdint.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/**
 * @brief Función que configura los habilitadores de los displays de la pantalla
 * y los iniciliza apagados
 *
 */
void DigitsInit(void);

/**
 * @brief Función que configura los segmentos de los displays de la pantalla y
 * los iniciliza apagados
 *
 */
void SegmentsInit(void);

/**
 * @brief Función que configura los puntos de los segmentos de los displays de
 * la pantalla y los iniciliza apagados
 *
 */
void DotsInit(void);

/**
 * @brief Función que permite deshabilitar todos los displays, de modo que se
 * apaga la pantalla
 *
 */
void DigitsTurnOff(void);

/**
 * @brief  Función que permite modificar los segmentos de un correspondiente
 * display para escribir un número en la pantalla
 *
 * @param segments Entero de 8 bits, en el que cada bit representa un segmento
 * del display según el numero que se quiera mostrar
 */
void SegmentsUpdate(uint8_t segments);

/**
 * @brief Función que permite encender un display específico de la pantalla
 *
 * @param digit Digito específico que se desea habilitar
 */
void DigitTurnOn(uint8_t digit);

/* === Private variable definitions ================================================================================ */

//! Estructura constante que representa el driver de la pantalla con las
//! funciones de callback
static const struct screen_driver_s screen_driver = {
    .DigitsTurnOff = DigitsTurnOff,
    .SegmentsUpdate = SegmentsUpdate,
    .DigitTurnOn = DigitTurnOn,
};

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

void DigitsInit(void) {

    // DIGITO_1 es el LSB y DIGITO_4 es el MSB
    Chip_SCU_PinMuxSet(DIGIT_1_PORT, DIGIT_1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_1_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_1_GPIO, DIGIT_1_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_2_PORT, DIGIT_2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_2_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_2_GPIO, DIGIT_2_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_3_PORT, DIGIT_3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_3_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_3_GPIO, DIGIT_3_BIT, true);

    Chip_SCU_PinMuxSet(DIGIT_4_PORT, DIGIT_4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | DIGIT_4_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, DIGIT_4_GPIO, DIGIT_4_BIT, true);

    Chip_GPIO_ClearValue(LPC_GPIO_PORT, DIGITS_GPIO, DIGITS_MASK);
}

void SegmentsInit(void) {

    Chip_SCU_PinMuxSet(SEGMENT_A_PORT, SEGMENT_A_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_A_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_A_GPIO, SEGMENT_A_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_B_PORT, SEGMENT_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_B_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_B_GPIO, SEGMENT_B_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_C_PORT, SEGMENT_C_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_C_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_C_GPIO, SEGMENT_C_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_D_PORT, SEGMENT_D_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_D_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_D_GPIO, SEGMENT_D_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_E_PORT, SEGMENT_E_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_E_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_E_GPIO, SEGMENT_E_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_F_PORT, SEGMENT_F_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_F_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_F_GPIO, SEGMENT_F_BIT, true);

    Chip_SCU_PinMuxSet(SEGMENT_G_PORT, SEGMENT_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_G_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_G_GPIO, SEGMENT_G_BIT, true);

    Chip_GPIO_ClearValue(LPC_GPIO_PORT, SEGMENTS_GPIO, SEGMENTS_MASK);
}

void DotsInit(void) {

    Chip_SCU_PinMuxSet(SEGMENT_P_PORT, SEGMENT_P_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SEGMENT_P_FUNC);
    Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, SEGMENT_P_GPIO, SEGMENT_P_BIT, true);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_P_GPIO, SEGMENT_P_BIT, false);
}

void DigitsTurnOff(void) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, DIGITS_GPIO, DIGITS_MASK);
}

void SegmentsUpdate(uint8_t segments) {
    Chip_GPIO_ClearValue(LPC_GPIO_PORT, SEGMENTS_GPIO, SEGMENTS_MASK);
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_P_GPIO, SEGMENT_P_BIT, false);

    Chip_GPIO_SetValue(LPC_GPIO_PORT, SEGMENTS_GPIO, (segments & SEGMENTS_MASK));
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, SEGMENT_P_GPIO, SEGMENT_P_BIT, (segments & SEGMENT_P));
}

void DigitTurnOn(uint8_t digit) {
    Chip_GPIO_SetValue(LPC_GPIO_PORT, DIGITS_GPIO, ((1 << (3 - digit)) & DIGITS_MASK));
}

/* === Public function definitions ================================================================================= */

board_t BoardCreate() {
    struct board_s* self = malloc(sizeof(struct board_s));

    if (self != NULL) {

        Chip_SCU_PinMuxSet(LED_RGB_BLUE_PORT, LED_RGB_BLUE_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_RGB_BLUE_FUNC);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED_RGB_BLUE_GPIO, LED_RGB_BLUE_BIT, true);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, LED_RGB_BLUE_GPIO, LED_RGB_BLUE_BIT, true);

        Chip_SCU_PinMuxSet(LED_RGB_GREEN_PORT, LED_RGB_GREEN_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_RGB_GREEN_FUNC);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED_RGB_GREEN_GPIO, LED_RGB_GREEN_BIT, true);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, LED_RGB_GREEN_GPIO, LED_RGB_GREEN_BIT, true);

        Chip_SCU_PinMuxSet(LED_RGB_RED_PORT, LED_RGB_RED_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_RGB_RED_FUNC);
        self->led_alarm = DigitalOutputCreate(LED_RGB_RED_GPIO, LED_RGB_RED_BIT, true);

        /******************/
        Chip_SCU_PinMuxSet(LED_R_PORT, LED_R_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_R_FUNC);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED_R_GPIO, LED_R_BIT, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, LED_R_GPIO, LED_R_BIT, true);

        Chip_SCU_PinMuxSet(LED_G_PORT, LED_G_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_G_FUNC);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED_G_GPIO, LED_G_BIT, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, LED_G_GPIO, LED_G_BIT, true);

        Chip_SCU_PinMuxSet(LED_B_PORT, LED_B_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_INACT | LED_B_FUNC);
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED_B_GPIO, LED_B_BIT, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, LED_B_GPIO, LED_B_BIT, true);

        /******************/
        Chip_SCU_PinMuxSet(KEY_F1_PORT, KEY_F1_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F1_FUNC);
        self->key_F1 = DigitalInputCreate(KEY_F1_GPIO, KEY_F1_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F2_PORT, KEY_F2_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F2_FUNC);
        self->key_F2 = DigitalInputCreate(KEY_F2_GPIO, KEY_F2_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F3_PORT, KEY_F3_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F3_FUNC);
        self->key_F3 = DigitalInputCreate(KEY_F3_GPIO, KEY_F3_BIT, false);

        Chip_SCU_PinMuxSet(KEY_F4_PORT, KEY_F4_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_F4_FUNC);
        self->key_F4 = DigitalInputCreate(KEY_F4_GPIO, KEY_F4_BIT, false);

        Chip_SCU_PinMuxSet(KEY_ACCEPT_PORT, KEY_ACCEPT_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_ACCEPT_FUNC);
        self->key_accept = DigitalInputCreate(KEY_ACCEPT_GPIO, KEY_ACCEPT_BIT, false);

        Chip_SCU_PinMuxSet(KEY_CANCEL_PORT, KEY_CANCEL_PIN, SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | KEY_CANCEL_FUNC);
        self->key_cancel = DigitalInputCreate(KEY_CANCEL_GPIO, KEY_CANCEL_BIT, false);

        /******************/
        DigitsInit();
        SegmentsInit();
        DotsInit();
        self->screen = ScreenCreate(4, &screen_driver);
    }

    return self;
}

/* === End of documentation ======================================================================================== */