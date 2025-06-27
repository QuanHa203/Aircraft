#ifndef SOFT_ACCESS_POINT_H
#define SOFT_ACCESS_POINT_H

#include "stdint.h"


namespace aircraft_lib {
    void wifi_nvs_flash_init();
    void wifi_init_softap(const char *ssid, const char *pass, uint8_t channel, uint8_t max_conn);
}
#endif