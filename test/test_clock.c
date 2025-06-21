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
 ** - Probar que se pudo crear el objeto Reloj
 ** - Probar que el reloj, al iniciar, se encuentra en un estado "no configurado"
 ** - Probar que el reloj se inicializa internamente en 00:00 
 ** - Probar que se puede establecer la hora a un valor válido
 ** - Probar que se pueden establecer los minutos a un valor válido
 ** - Probar que se aceptan horas en el rango de 00 a 23 y minutos en el rango de 00 a 59 durante el ajuste
 ** - Probar que la hora avanza correctamente en un minuto cada vez que se simula un tick de tiempo
 ** - Probar que la hora transiciona correctamente de 23:59 a 00:00
 ** - Probar que el reloj ingresa al modo de ajuste de hora al recibir la señal de inicio de ajuste (señal de "F1" por más de 3 segundos)
 ** - Probar que, en modo de ajuste, se puede incrementar el valor de los minutos mediante la señal de "F4"
 ** - Probar que, en modo de ajuste, se puede disminuir el valor de los minutos mediante la señal de "F3"
 ** - Probar que el incremento  de minutos realizan un ciclo al alcanzar el valor máximo (59 a 00)
 ** - Probar que el decremento de minutos realizan un ciclo al alcanzar el valor mínimo (00 a 59)
 ** - Probar que, al confirmar la configuración de los minutos (señal de "Aceptar"), el ajuste procede a la configuración de la hora
 ** - Probar que el incremento de horas realizan un ciclo al alcanzar el valor máximo (23 a 00) 
 ** - Probar que el decremento de horas realizan un ciclo al alcanzar el valor mínimo (00 a 23) 
 ** - Probar que los cambios pendientes en el ajuste de hora se descartan si no hay interacción de usuario durante un período definido
 ** - Probar que los cambios pendientes se descartan si se recibe la señal de "Cancelar"
 ** - Probar que se puede definir una alarma para una hora y minutos específicos
 ** - Probar que la definición de alarma solo acepta el formato de 24 horas
 ** - Probar que se pueden definir alarmas hasta el límite máximo soportado por el sistema
 ** - Probar que se rechaza la definición de alarmas adicionales si se supera el límite máximo
 ** - Probar que se puede eliminar una alarma existente
 ** - Probar que la alarma se puede habilitar
 ** - Probar que la alarma se puede inhabilitar
 ** - Probar que la alarma se activa mediante la señal de "Aceptar" en modo normal del reloj
 ** - Probar que la alarma se desactiva mediante la señal de "Cancelar" en modo normal del reloj
 ** - Probar que la alarma se activa precisamente a la hora y minuto definidos
 ** - Probar que la alarma no se activa si está inhabilitada
 ** - Probar que si hay múltiples alarmas configuradas para el mismo minuto, todas se activan
 ** - Probar que una alarma de un solo uso se activa una única vez
 ** - Probar que cuando la alarma se activa, una señal de "Aceptar" la pospone por cinco minutos
 ** - Probar que cuando la alarma se activa, una señal de "Cancelar" la silencia hasta el día siguiente a la hora programada
 ** - Probar que el reloj ingresa al modo de ajuste de la alarma al recibir la señal de inicio de ajuste (señal de "F2" por más de 3 segundos)
 ** - Probar que el ajuste de la hora de la alarma sigue el mismo comportamiento lógico que el ajuste de la hora del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function definitions ================================================================================= */

/* === End of documentation ======================================================================================== */