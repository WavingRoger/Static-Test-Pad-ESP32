#pragma once

#include "esp_err.h"

/* ================= STATES ================= */

typedef enum {
  STATE_INIT = 0,
  STATE_ESPNOW_INIT,
  STATE_HX711_INIT,
  STATE_CALIBRATE,
  STATE_RUN,
  STATE_ERROR
} app_state_t;

/* ================= MESSAGE STRUCT ================= */

typedef struct {
  float weight;
  uint32_t timestamp_ms;
} message_t;

/* ================= API ================= */

void app_fsm_init(void);
void app_fsm_run(void);
