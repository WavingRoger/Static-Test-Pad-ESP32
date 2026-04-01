#include "hx711_driver.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "HX711";

/* ================= INTERNAL STATE ================= */

static int DOUT;
static int SCK;

static float scale = 1.0f;
static int32_t offset = 0;

/* ================= INIT ================= */

void hx711_init(const hx711_config_t *config) {
  DOUT = config->dout_pin;
  SCK = config->sck_pin;
  scale = config->scale;

  gpio_set_direction(DOUT, GPIO_MODE_INPUT);
  gpio_set_direction(SCK, GPIO_MODE_OUTPUT);

  gpio_set_level(SCK, 0);

  ESP_LOGI(TAG, "HX711 initialized (DOUT=%d, SCK=%d)", DOUT, SCK);
}

/* ================= READY CHECK ================= */

bool hx711_is_ready(void) { return gpio_get_level(DOUT) == 0; }

/* ================= RAW READ ================= */

int32_t hx711_read_raw(void) {
  int32_t data = 0;

  /* wait until ready */
  while (!hx711_is_ready())
    ;

  for (int i = 0; i < 24; i++) {
    gpio_set_level(SCK, 1);
    data <<= 1;
    gpio_set_level(SCK, 0);

    if (gpio_get_level(DOUT))
      data++;
  }

  /* extra pulse (gain = 128) */
  gpio_set_level(SCK, 1);
  gpio_set_level(SCK, 0);

  /* sign extend */
  if (data & 0x800000)
    data |= 0xFF000000;

  return data;
}

/* ================= CALIBRATION ================= */

void hx711_tare(uint32_t duration_ms) {
  ESP_LOGI(TAG, "Taring... keep load cell empty");

  int64_t sum = 0;
  int samples = 0;

  int64_t start = esp_timer_get_time();

  while ((esp_timer_get_time() - start) < (duration_ms * 1000)) {
    sum += hx711_read_raw();
    samples++;
  }

  offset = sum / samples;

  ESP_LOGI(TAG, "Offset = %ld (samples=%d)", offset, samples);
}

void hx711_set_scale(float new_scale) { scale = new_scale; }

/* ================= HIGH LEVEL ================= */

float hx711_get_weight(uint8_t samples) {
  float sum = 0;

  for (int i = 0; i < samples; i++) {
    int32_t raw = hx711_read_raw();
    sum += (raw - offset) / scale;
  }

  return sum / samples;
}
