#include "picoruby-esp32.h"

#include "uart_file_transfer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// FatFs (using picoruby-filesystem-fat implementation)
#include "../components/picoruby-esp32/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source/ff.h"

// Flash disk driver
extern int FLASH_disk_initialize(void);

void app_main(void)
{
  //check gpio
  //if pressed
  {
    // Temporarily enable logging for debugging
    // TODO: Disable this after debugging
    // esp_log_level_set("*", ESP_LOG_NONE);

    // Wait a bit for any pending log output to finish
    vTaskDelay(pdMS_TO_TICKS(100));

    // Initialize flash disk using picoruby-filesystem-fat implementation
    ESP_LOGI("main", "Initializing flash disk...");
    int ret = FLASH_disk_initialize();
    if (ret != 0) {
        ESP_LOGE("main", "Failed to initialize flash disk: %d", ret);
    } else {
        ESP_LOGI("main", "Flash disk initialized successfully");
    }

    // Mount FatFs
    ESP_LOGI("main", "Mounting FatFs...");
    static FATFS fs;
    FRESULT fret = f_mount(&fs, "1:", 1);  // Drive 1 (FLASH), mount immediately
    if (fret != FR_OK) {
        ESP_LOGE("main", "Failed to mount FatFs: %d", fret);
    } else {
        ESP_LOGI("main", "FatFs mounted successfully at 1:");

        // List root directory contents
        DIR dir;
        FILINFO fno;
        fret = f_opendir(&dir, "1:/");
        if (fret == FR_OK) {
            ESP_LOGI("main", "Root directory contents:");
            while (1) {
                fret = f_readdir(&dir, &fno);
                if (fret != FR_OK || fno.fname[0] == 0) break;

                const char *type = (fno.fattrib & AM_DIR) ? "DIR " : "FILE";
                ESP_LOGI("main", "  %s %8lu %s", type, fno.fsize, fno.fname);
            }
            f_closedir(&dir);
        } else {
            ESP_LOGE("main", "Failed to open root directory: %d", fret);
        }
    }

    fs_proxy_create_task();

    while(1){
      vTaskDelay(pdMS_TO_TICKS(100));
      //check reboot button
      //esp_restart();
    }
  }

  picoruby_esp32();
}
