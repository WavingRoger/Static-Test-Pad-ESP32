#include "espnow_driver.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"

#include <string.h>

static const char *TAG = "ESPNOW";

/* ================= RECEIVER MAC ================= */
static uint8_t receiver_mac[] = {0x94, 0x54, 0xC5, 0xAF, 0x0F, 0x28};

/* ================= WIFI INIT (PRIVATE) ================= */
static esp_err_t wifi_init_internal(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  return esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}

/* ================= SEND CALLBACK ================= */
static void send_cb(const esp_now_send_info_t *tx_info,
                    esp_now_send_status_t status) {
  ESP_LOGI(TAG, "Send: %s", status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

/* ================= PUBLIC INIT ================= */
esp_err_t espnow_init_driver(void) {
  ESP_LOGI(TAG, "Initializing WiFi + ESP-NOW");

  ESP_ERROR_CHECK(wifi_init_internal());

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));

  esp_now_peer_info_t peer = {0};
  memcpy(peer.peer_addr, receiver_mac, 6);
  peer.channel = 1;
  peer.encrypt = false;

  ESP_ERROR_CHECK(esp_now_add_peer(&peer));

  ESP_LOGI(TAG, "ESP-NOW Ready");

  return ESP_OK;
}

/* ================= SEND ================= */
void espnow_send_data(uint8_t *data, size_t len) {
  esp_now_send(receiver_mac, data, len);
}
