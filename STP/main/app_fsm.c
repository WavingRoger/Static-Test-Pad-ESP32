#include "app_fsm.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Drivers */
#include "espnow_driver.h"
#include "hx711_driver.h"

static const char *TAG = "APP_FSM";

/* ================= INTERNAL STATE ================= */

static app_state_t current_state = STATE_INIT;

/* HX711 config */
static hx711_config_t hx_config = {
    .dout_pin = 4, .sck_pin = 5, .scale = 2280.0f};

/* ================= BUZZER ================= */

#define BUZZER_PIN 15
static volatile int buzzer_run_mode = 0;

void buzzer_task(void *arg) {
  while (1) {
    if (buzzer_run_mode) {
      // 🔊 short beep every 1 second
      gpio_set_level(BUZZER_PIN, 1);
      vTaskDelay(pdMS_TO_TICKS(100));
      gpio_set_level(BUZZER_PIN, 0);

      vTaskDelay(pdMS_TO_TICKS(900)); // total ≈ 1 sec cycle
    } else {
      gpio_set_level(BUZZER_PIN, 0);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}
static void buzzer_init(void) {
  gpio_reset_pin(BUZZER_PIN);
  gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(BUZZER_PIN, 0);
}

static void buzzer_beep(int duration_ms) {
  gpio_set_level(BUZZER_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(duration_ms));
  gpio_set_level(BUZZER_PIN, 0);
}

/* ================= INIT ================= */

void app_fsm_init(void) { current_state = STATE_INIT; }

/* ================= FSM RUN ================= */

void app_fsm_run(void) {
  message_t msg;

  while (1) {
    switch (current_state) {
    /* ================= INIT ================= */
    case STATE_INIT:
      ESP_LOGI(TAG, "STATE_INIT");
      buzzer_init();
      xTaskCreate(buzzer_task, "buzzer_task", 2048, NULL, 5, NULL);
      buzzer_beep(100);
      current_state = STATE_ESPNOW_INIT;
      break;

    /* ================= ESP-NOW ================= */
    case STATE_ESPNOW_INIT:
      ESP_LOGI(TAG, "STATE_ESPNOW_INIT");

      if (espnow_init_driver() == ESP_OK) {
        current_state = STATE_HX711_INIT;
      } else {
        current_state = STATE_ERROR;
      }
      break;

    /* ================= HX711 INIT ================= */
    case STATE_HX711_INIT:
      ESP_LOGI(TAG, "STATE_HX711_INIT");

      hx711_init(&hx_config);

      /* warm-up */
      vTaskDelay(pdMS_TO_TICKS(500));

      current_state = STATE_CALIBRATE;
      break;

    /* ================= CALIBRATION ================= */
    case STATE_CALIBRATE:
      ESP_LOGI(TAG, "STATE_CALIBRATE");

      hx711_tare(1000); // 1 second tare
      buzzer_beep(100);
      vTaskDelay(pdMS_TO_TICKS(100));
      buzzer_beep(100);

      current_state = STATE_RUN;
      break;

    /* ================= MAIN LOOP ================= */
    case STATE_RUN:
      buzzer_run_mode = 1;
      msg.weight = hx711_get_weight(1);
      msg.timestamp_ms = esp_timer_get_time() / 1000;
      espnow_send_data((uint8_t *)&msg, sizeof(msg));

      ESP_LOGI(TAG, "Weight: %.2f kg ", msg.weight);

      vTaskDelay(pdMS_TO_TICKS(1)); // 20 Hz
      break;

    /* ================= ERROR ================= */
    case STATE_ERROR:
      ESP_LOGE(TAG, "STATE_ERROR");

      buzzer_run_mode = 0;
      for (int i = 0; i < 10; i++) {
        buzzer_beep(100);
        vTaskDelay(pdMS_TO_TICKS(100));
      }
      /* stay here or implement recovery */
      vTaskDelay(pdMS_TO_TICKS(1000));
      break;
    }
  }
}
