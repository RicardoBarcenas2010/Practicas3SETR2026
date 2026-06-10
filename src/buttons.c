#include "buttons.h"
#include "app_config.h"
#include "system_state.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"   
#include "freertos/task.h"        
#include "driver/gpio.h"          
#include "esp_log.h"              

static const char *TAG = "BUTTON";

typedef struct {
    int stable_state;
    int last_raw;
    int count;
} Debounce_t;

#define DEBOUNCE_COUNT 3

static bool debounce_update(uint8_t gpio, Debounce_t *db)
{
    int raw = gpio_get_level((gpio_num_t)gpio);
    bool pressed = false;

    if (raw == db->last_raw) {
        if (db->count < DEBOUNCE_COUNT) db->count++;
        if (db->count >= DEBOUNCE_COUNT && raw != db->stable_state) {
            db->stable_state = raw;
            if (db->stable_state == 0) pressed = true;
        }
    } else {
        db->count = 0;
        db->last_raw = raw;
    }
    return pressed;
}

void button_task(void *pvParameters)
{
    ButtonTaskParams_t *cfg = (ButtonTaskParams_t *)pvParameters;
    Debounce_t db = {1, 1, 0};

    gpio_reset_pin((gpio_num_t)cfg->gpio);
    gpio_set_direction((gpio_num_t)cfg->gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)cfg->gpio, GPIO_PULLUP_ONLY);

    while (1)
    {
        if (debounce_update(cfg->gpio, &db))
        {
            switch (cfg->type)
            {
                case BUTTON_START_PAUSE:
                    g_system.pending_event = MANAGER_EVENT_START_PAUSE;
                    break;
                case BUTTON_DIRECTION:
                    g_system.pending_event = MANAGER_EVENT_DIRECTION;
                    break;
                case BUTTON_SPEED:
                    g_system.pending_event = MANAGER_EVENT_SPEED;
                    break;
            }
            ESP_LOGI(TAG, "%s presionado", cfg->name);
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_MS));
    }
}