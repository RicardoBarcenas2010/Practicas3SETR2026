/**
 * @brief   Función principal del sistema, inicializa el estado y crea tareas
 * @author  Ricardo Bárcenas, Daniela Valle
 */

#include "system_state.h"
#include "app_tasks.h"

void app_main(void)
{
	
	system_state_init();
    app_tasks_create();
}



/*
 * CONCLUSIÓN PRÁCTICA 3: Contador BCD con FreeRTOS
 * Integrantes: Ricardo Gabriel Bárcenas Benítez, Daniela Lendalí Valle Campos
 * 
 * 
 * En esta práctica se implementó un contador BCD ascendente/descendente sobre un ESP32 
 * utilizando FreeRTOS, cumpliendo con todos los requisitos del documento: control de dirección, 
 * velocidad y pausa mediante tres botones físicos, conservación del valor al pausar, 
 * e ignorando los botones de velocidad y dirección cuando el sistema está detenido. 
 * Se aplicaron conceptos fundamentales de FreeRTOS como TaskHandle_t para que el Task Manager suspendiera
 *  y reanudara tareas mediante vTaskSuspend() y vTaskResume(), 
 * pvParameters para parametrizar las tareas de LEDs y botones reutilizando el mismo código, 
 * y eTaskGetState() para monitorear los estados (RUNNING, BLOCKED, SUSPENDED) por UART.
 *
 * 1. IMPLEMENTACIÓN DEL SISTEMA
 * Se implementó un contador BCD de 0 a 9 sobre ESP32 utilizando FreeRTOS.
 * El sistema permite controlar mediante 3 botones físicos:
 *   - Dirección de conteo (ascendente/descendente)
 *   - Velocidad de conteo (500ms / 250ms)
 *   - Start/Pause (inicia o detiene el contador)
 * 
 * La lógica de control se centralizó en un Task Manager, mientras que las
 * tareas de botones y LEDs se parametrizaron usando pvParameters, permitiendo
 * reutilizar la misma función para múltiples instancias.
 * 
 * 2. CONCEPTOS FUNDAMENTALES DE FREERTOS APLICADOS
 * a) Diferencia entre BLOCKED y SUSPENDED:
 *    - BLOCKED: La tarea espera un evento (tiempo, semáforo, cola). Vuelve
 *      automáticamente a READY cuando ocurre el evento.
 *    - SUSPENDED: La tarea fue suspendida explícitamente con vTaskSuspend().
 *      Solo vuelve a RUNNING con vTaskResume().
 *    En la práctica, el contador se SUSPENDE al pausar, no se BLOQUEA.
 * 
 * b) vTaskDelay() y estado BLOCKED:
 *    vTaskDelay() coloca la tarea en estado BLOCKED durante una cantidad de
 *    ticks. El scheduler no la ejecuta hasta que expire el tiempo, liberando
 *    el procesador para otras tareas.
 * 
 * c) vTaskDelay() vs Software Timer:
 *    - vTaskDelay(): Bloquea la tarea que lo llama. Es simple pero consume
 *      el stack de la tarea mientras espera.
 *    - Software Timer: Ejecuta una callback en contexto del timer task.
 *      No bloquea ninguna tarea. Es más preciso para temporizaciones únicas
 *      o periódicas que no requieren una tarea dedicada.
 * 
 * d) Función del Idle Task:
 *    Es la tarea de menor prioridad que ejecuta cuando no hay tareas READY.
 *    Sus funciones principales son:
 *      - Liberar memoria de tareas eliminadas (vTaskDelete)
 *      - Ejecutar hooks definidos por el usuario (vApplicationIdleHook)
 *      - Mantener el sistema operativo funcionando
 * 
 * e) Planificación Round Robin:
 *    Cuando múltiples tareas tienen la MISMA prioridad, FreeRTOS las ejecuta
 *    por turnos durante un tick del sistema cada una (Round Robin). Esto
 *    evita que una tarea acapare el procesador indefinidamente.
 * 
 * f) Ventajas de pvParameters:
 *    Permite pasar datos personalizados a una tarea al crearla. En esta
 *    práctica se usó para:
 *      - Pasar a led_task() el GPIO y la posición de bit de cada LED
 *      - Pasar a button_task() el GPIO y el tipo de evento de cada botón
 *    Esto evita crear funciones separadas para cada LED/botón.
 * 
 * g) Ventajas de TaskHandle_t:
 *    Permite controlar una tarea desde otra, incluyendo:
 *      - Consultar su estado (eTaskGetState)
 *      - Suspenderla (vTaskSuspend)
 *      - Reanudarla (vTaskResume)
 *    En la práctica, el Task Manager usa handles para suspender/reanudar
 *    el contador y los botones según el estado de pausa.
 * 
 * h) ¿Por qué NO se usaron variables globales para el contador?
 *    Aunque g_system es global, está encapsulada en un módulo y se accede
 *    mediante funciones. Si fuera solo variable global sin control:
 *      - Habría condiciones de carrera (varias tareas modificando)
 *      - No se podría pausar/reanudar limpiamente
 *      - Sería imposible implementar la regla de ignorar botones en pausa
 * 
 */