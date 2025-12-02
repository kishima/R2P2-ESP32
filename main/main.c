#include "picoruby-esp32.h"

#include "uart_file_transfer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 37  // M5StickC Plus2 ButtonA
static void check_uart_file_transfer_mode(void)
{
  // GPIO37 is input-only on ESP32, no internal pull-up/down available
  // Assumes external pull-up resistor on M5StickC Plus2
  gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << BUTTON_GPIO),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);

  // Wait a bit for GPIO to stabilize
  vTaskDelay(pdMS_TO_TICKS(100));

  // Read button state (active low: 0 = pressed, 1 = not pressed)
  int button_state = gpio_get_level(BUTTON_GPIO);

  //debug
  //button_state = 0;

  if (button_state == 0)
  {
    //esp_log_level_set("*", ESP_LOG_NONE);
   
    // Wait a bit for any pending log output to finish
    vTaskDelay(pdMS_TO_TICKS(100));

    fs_proxy_create_task();

    while(1){
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }

}

void app_main(void)
{
  check_uart_file_transfer_mode();
  picoruby_esp32();
}