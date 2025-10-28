#include "picoruby-esp32.h"

#include "uart_file_transfer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP VFS FATFS
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"

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

    // Mount FATFS using ESP-IDF VFS
    ESP_LOGI("main", "Mounting FATFS on flash partition...");

    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(
        "/", "storage", &mount_config, &wl_handle);

    if (err != ESP_OK) {
        ESP_LOGE("main", "Failed to mount FATFS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI("main", "FATFS mounted successfully at /");
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
