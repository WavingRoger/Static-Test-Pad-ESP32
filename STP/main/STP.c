#include "esp_log.h"
#include "nvs_flash.h"

#include "app_fsm.h"

static const char *TAG = "MAIN";

void app_main(void) {
  ESP_LOGI(TAG, "System Boot");

  /* ================= NVS INIT ================= */
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGW(TAG, "NVS issue, erasing...");
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  /* ================= FSM INIT ================= */
  app_fsm_init();

  /* ================= RUN FSM ================= */
  app_fsm_run();
}
