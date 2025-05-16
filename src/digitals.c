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

/** @file digitals.c
 ** @brief Código fuente del módulo de Gestión de Entradas y Salidas Digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digitals.h"
#include "chip.h"
#include <stdint.h>
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/*! Estructura de datos que representa una Salida Digital GPIO */
struct digital_output_s {
    uint8_t gpio_port; //!< Puerto GPIO al que pertenece la salida
    uint8_t gpio_bit;  //!< Bit del puerto GPIO al que pertenece la salida
};

/*! Estructura de datos que representa una Entrada Digital GPIO */
struct digital_input_s {
    uint8_t gpio_port;   //!< Puerto GPIO al que pertenece la salida
    uint8_t gpio_bit;    //!< Bit del puerto GPIO al que pertenece la salida
    bool inverted_logic; //!< Determina si la entrada tiene lógica inversa o no
    bool last_state;     //!< Último estado de la entrada digital
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

digital_output_t DigitalOutputCreate(uint8_t gpio_port, uint8_t gpio_bit) {
    digital_output_t self = malloc(sizeof(struct digital_output_s));

    if (self != NULL) {
        self->gpio_port = gpio_port;
        self->gpio_bit = gpio_bit;
    }

    return self;
}

void DigitalOutputActivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio_port, self->gpio_bit, true);
}

void DigitalOutputDeactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->gpio_port, self->gpio_bit, false);
}

void DigitalOutputToggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->gpio_port, self->gpio_bit);
}

digital_input_t DigitalInputCreate(uint8_t gpio_port, uint8_t gpio_bit, bool inverted_logic) {
    digital_input_t self = malloc(sizeof(struct digital_input_s));

    if (self != NULL) {
        self->gpio_port = gpio_port;
        self->gpio_bit = gpio_bit;
        self->inverted_logic = inverted_logic;
        self->last_state = DigitalInputGetIsActive(self);
    }

    return self;
}

bool DigitalInputGetIsActive(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->gpio_port, self->gpio_bit); // Devuelve TRUE si está en ALTO

    if ((self->inverted_logic) = true) {
        state = !state;
    }

    return state;
}

digital_input_state_t DigitalInputHasChanged(digital_input_t self) {
    digital_input_state_t result = DIGITAL_INPUT_NO_CHANGE;

    bool current_state = DigitalInputGetIsActive(self);

    if ((self->last_state == false) && (current_state == true)) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if ((self->last_state == true) && (current_state == false)) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }

    self->last_state = current_state;

    return result;
}

bool DigitalInputWasActivated(digital_input_t self) {
    bool result = false;

    if (DigitalInputHasChanged(self) == DIGITAL_INPUT_WAS_ACTIVATED) {
        result = true;
        self->last_state = true;
    } else {
    }

    return result;
}

bool DigitalInputWasDeactivated(digital_input_t self) {
    bool result = false;

    if (DigitalInputHasChanged(self) == DIGITAL_INPUT_WAS_DEACTIVATED) {
        result = true;
        self->last_state = false;
    }

    return result;
}

/* === Public function definitions ================================================================================= */

/* === End of documentation ======================================================================================== */