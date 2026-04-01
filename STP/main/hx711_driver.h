#pragma once

#include <stdbool.h>
#include <stdint.h>

/* ================= CONFIG STRUCT ================= */

typedef struct {
  int dout_pin;
  int sck_pin;
  float scale;
} hx711_config_t;

/* ================= INIT ================= */

void hx711_init(const hx711_config_t *config);

/* ================= BASIC ================= */

bool hx711_is_ready(void);
int32_t hx711_read_raw(void);

/* ================= CALIBRATION ================= */

void hx711_tare(uint32_t duration_ms);
void hx711_set_scale(float scale);

/* ================= HIGH LEVEL ================= */

float hx711_get_weight(uint8_t samples);
