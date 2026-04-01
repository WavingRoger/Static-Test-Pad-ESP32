#pragma once
#include "esp_err.h"
#include <stdint.h>

esp_err_t espnow_init_driver(void);
void espnow_send_data(uint8_t *data, size_t len);
