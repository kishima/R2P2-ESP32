#include "picoruby-esp32.h"

#include "uart_file_transfer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    fs_proxy_create_task();

    while(1){
      vTaskDelay(pdMS_TO_TICKS(100));
      //check reboot button
      //esp_restart();
    }
  }

  picoruby_esp32();
}
