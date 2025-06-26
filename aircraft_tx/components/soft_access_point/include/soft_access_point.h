#ifndef SOFT_ACCESS_POINT_H
#define SOFT_ACCESS_POINT_H

#include "stdint.h"

void wifi_nvs_flash_init();
void wifi_init_softap(const char *ssid, const char *pass, uint8_t channel = 1, uint8_t max_conn = 1);
#endif