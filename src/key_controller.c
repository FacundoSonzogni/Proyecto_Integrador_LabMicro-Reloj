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

/** @file key_controller.c
 ** @brief Código fuente del módulo de Gestión del pulsado de las teclas
 **/

/* === Headers files inclusions ==================================================================================== */

#include "key_controller.h"
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

#ifndef KEY_TASK_DELAY_MS
#define KEY_TASK_DELAY_MS 100
#endif

/* === Private data type declarations ============================================================================== */

/*! Estructura de datos que representa un botón de la placa */
struct button_s {
    digital_input_t key;       //!< Entrada digital que representa la tecla
    uint16_t internal_counter; //!< Cuenta interna del tiempo que fue pulsado el botón
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

button_t ButtonCreate(digital_input_t key) {
    button_t self = malloc(sizeof(struct button_s));

    if (self != NULL) {
        self->key = key;
        self->internal_counter = 0;
    }

    return self;
}

bool ButtonWasPressed3secs(button_t self) {
    bool result;

    if (DigitalInputGetIsActive(self->key)) {
        self->internal_counter++;
        if (self->internal_counter >= 3000) {
            result = true;
        }
    } else {
        result = false;
        self->internal_counter = 0;
    }

    return result;
}

bool ButtonWasPressed(button_t self) {
    return DigitalInputWasActivated(self->key);
}

void ButtonPressedTask(void* pointer) {
    button_task_args_t args = pointer;

    while (true) {
        while (DigitalInputGetIsActive(((digital_input_t)args->key)) == false) {
            vTaskDelay(pdMS_TO_TICKS(KEY_TASK_DELAY_MS));
        }

        xEventGroupSetBits(args->event_group, (EventBits_t)args->event_mask);

        while (DigitalInputGetIsActive(((digital_input_t)args->key)) == true) {
            vTaskDelay(pdMS_TO_TICKS(KEY_TASK_DELAY_MS));
        }
    }
}

void ButtonPressed3secsTask(void* pointer) {
    button_task_args_t args = pointer;
    TickType_t initial_ticks;
    TickType_t current_tick;

    while (true) {
        while (DigitalInputGetIsActive(((digital_input_t)args->key)) == false) {
            vTaskDelay(pdMS_TO_TICKS(KEY_TASK_DELAY_MS));
        }

        initial_ticks = xTaskGetTickCount();

        while (DigitalInputGetIsActive(((digital_input_t)args->key)) == true) {
            current_tick = xTaskGetTickCount();
            vTaskDelay(pdMS_TO_TICKS(10));

            if (current_tick - initial_ticks >= pdMS_TO_TICKS(3000)) {
                xEventGroupSetBits(args->event_group, (EventBits_t)args->event_mask);
            }
        }
    }
}

/* === End of documentation ======================================================================================== */
