#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_log.h"

static const char *TAG = "RX";

/* ================= MESSAGE STRUCT ================= */
typedef struct {
  float weight;
  uint32_t timestamp_ms;
} message_t;

/* ================= WIFI INIT ================= */
static void wifi_init(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
}

/* ================= RECEIVE CALLBACK ================= */
static void recv_cb(const esp_now_recv_info_t *info, const uint8_t *data,
                    int len) {
  if (len != sizeof(message_t)) {
    ESP_LOGW(TAG, "Invalid packet size: %d", len);
    return;
  }

  message_t msg;
  memcpy(&msg, data, sizeof(msg));

  ESP_LOGI(TAG, "Time: %lu ms | Weight: %.2f kg", msg.timestamp_ms, msg.weight);
  printf("%lu,%.3f\n", msg.timestamp_ms, msg.weight);
}

/* ================= MAIN ================= */
void app_main(void) {
  ESP_LOGI(TAG, "Receiver Boot");

  ESP_ERROR_CHECK(nvs_flash_init());

  wifi_init();

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_cb));

  ESP_LOGI(TAG, "Receiver Ready");
}
