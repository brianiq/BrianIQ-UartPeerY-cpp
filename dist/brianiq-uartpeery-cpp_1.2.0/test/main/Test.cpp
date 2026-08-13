#include "BrianIQ-UartPeerY.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <cstdint>
#include <format>
#include <string>

int8_t  Test01 () {
        BrianIQ::UartPeerY  AE05;
        std::string    AE10;
        int8_t AE15 =  AE05.init (AE10, 2, 21, 23, 4, 8, 'N', 1, 9600, 256, 256);
        if (AE15 != 0) {
                std::string BE05 = std::format ("Init failed [{}]", AE10.c_str());
                ESP_LOGE ("Test01", "%s", BE05.c_str());
                vTaskDelay (pdMS_TO_TICKS(1000));
                esp_restart ();
        }
        while (true) {
                ESP_LOGI ("Test01", "Iteration progresng");
                std::string BE05 = std::format ("Hello world!");
                std::vector<uint8_t> BE10 (BE05.begin (), BE05.end ());
                std::string BE11;
                int8_t BE15=AE05.wrtt (BE11, BE10);
                if (BE15 != 0) {
                        std::string CE05 = std::format ("Wrtt failed [{}]", BE11.c_str());
                        ESP_LOGE ("Test01", "%s", CE05.c_str());
                }
                ESP_LOGI ("Test01", "Iteration completed");
                vTaskDelay (pdMS_TO_TICKS(1000));
        }
return  0;
}

extern "C" void app_main () {
        Test01 ();
}
