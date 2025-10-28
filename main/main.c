#include "picoruby-esp32.h"

#include "uart_file_transfer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
  //check gpio
  //if pressed
  {
    esp_log_level_set("*", ESP_LOG_NONE);
    fs_proxy_create_task();
    while(1){
      vTaskDelay(pdMS_TO_TICKS(100));
      //check reboot button 
      //esp_restart();
    }
  }

  picoruby_esp32();
}
