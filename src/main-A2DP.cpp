#include "nvs_flash.h"

#include "BluetoothA2DPSink.h"
BluetoothA2DPSink a2dp_sink;

void setup()
{
  Serial.begin(115200);

  // NVSの初期化
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();
    nvs_flash_init();
  }

  // set i2s pin
  i2s_pin_config_t my_pin_config = {
      .bck_io_num = 0,
      .ws_io_num = 2,
      .data_out_num = 1,
      .data_in_num = I2S_PIN_NO_CHANGE};
  // set i2s mode
  static i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100, // updated automatically by A2DP
      .bits_per_sample = (i2s_bits_per_sample_t)32,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = true,
      .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
  };

  a2dp_sink.set_pin_config(my_pin_config);
  // a2dp_sink.set_i2s_config(i2s_config);
  //  a2dp_sink.set_auto_reconnect(true);
  //   set device name
  a2dp_sink.start("BT_Audio");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
