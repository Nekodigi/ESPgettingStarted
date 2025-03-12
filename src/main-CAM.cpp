#include <Arduino.h>
#include <WiFi.h>
#include <esp_camera.h>
#include <esp_http_server.h>

// #define WIFI_DIRECT // ＜＜---　STAモードにするなら、ここをコメントアウトしてください。

#ifdef WIFI_DIRECT
#define SSID "ESP32_CAM"                  // APの名前
#define PASSWORD "12345678"               // APのパスワード（８文字以上）
const IPAddress ip(192, 168, 7, 1);       // IPアドレス
const IPAddress subnet(255, 255, 255, 0); // サブネットマスク
#else
#define SSID "D821DA9C0A95-2G"    // WiFiルータのSSID
#define PASSWORD "abtxxcsdpb2s37" // WiFiルータのパスワード
#endif

volatile bool wificonnected = false;
bool oldconnected = false;

void IRAM_ATTR WiFiEvent(WiFiEvent_t event)
{ // 割り込み内で Serial.Print してはいけない。書き換える変数にはVolatile属性を付ける。
  switch (event)
  {
  case WIFI_EVENT_STA_CONNECTED:
    wificonnected = true;
    break;
  case WIFI_EVENT_STA_DISCONNECTED:
    wificonnected = false;
    break;
  case WIFI_EVENT_AP_STACONNECTED:
    wificonnected = true;
    break;
  case WIFI_EVENT_AP_STADISCONNECTED:
    wificonnected = false;
    break;
  }
}

void wifi_start() // WiFiを [APモード] または [STAモード] で開始する
{
#ifdef WIFI_DIRECT // WIFI_DIRECTが定義されていたらAPモード

  WiFi.mode(WIFI_AP); // Wifi AP モード
  if (PASSWORD != "")
    WiFi.softAP(SSID, PASSWORD);
  else
    WiFi.softAP(SSID);
  WiFi.softAPConfig(ip, ip, subnet);

#else
  delay(3000);
  WiFi.mode(WIFI_STA); // Wifi STA モード
  WiFi.begin(SSID, PASSWORD);
  Serial.print("\nWifi Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi connected");
  Serial.printf("Go to: http://%s\n", WiFi.localIP().toString().c_str()); // IPアドレスを表示

#endif

  WiFi.setTxPower(WIFI_POWER_19_5dBm); // 出力パワー設定
  WiFi.onEvent(WiFiEvent);             // WiFiイベント処理指定
  wificonnected = false;
}

void cam_init()
{ // カメラ初期化
  camera_config_t config = {
      .pin_pwdn = -1,
      .pin_reset = -1,
      .pin_xclk = 10,
      .pin_sscb_sda = 40,
      .pin_sscb_scl = 39,
      .pin_d7 = 48,
      .pin_d6 = 11,
      .pin_d5 = 12,
      .pin_d4 = 14,
      .pin_d3 = 16,
      .pin_d2 = 18,
      .pin_d1 = 17,
      .pin_d0 = 15,
      .pin_vsync = 38,
      .pin_href = 47,
      .pin_pclk = 13,
      .xclk_freq_hz = 20000000,       // 20MHz
      .ledc_timer = LEDC_TIMER_1,     // 1番のタイマー使用
      .ledc_channel = LEDC_CHANNEL_0, // 0番のチャネル使用
      .pixel_format = PIXFORMAT_JPEG, // JPEG
      .frame_size = FRAMESIZE_SVGA,   // 解像度(800x600)
      .jpeg_quality = 12,             // JPGE画質（小さいほど高画質）
      .fb_count = 2,                  // フレームバッファ数（増やすと滑らかになるが遅延が増す）
      .fb_location = CAMERA_FB_IN_PSRAM,
      .grab_mode = CAMERA_GRAB_LATEST};

  esp_err_t result = esp_camera_init(&config);
  if (result != ESP_OK)
  {
    Serial.printf("esp_camera_init error 0x%x\n", result);
    return;
  }
  sensor_t *s = esp_camera_sensor_get(); // 鮮度やコントラストなど変更する場合
  s->set_vflip(s, 0);                    // 垂直反転 [0:無　1:反転]
                                         //  s->set_hmirror(s, 1);					// 左右反転	[0:無　1:反転]
                                         //  s->set_saturation(s, 2);              	// 彩度 -2 - 2
                                         //  s->set_brightness(s, -1);             	// 輝度 -2 - 2
                                         //  s->set_contrast(s, 1);                	// コントラスト -2 - 2
  s->set_denoise(s, 1);                  // ノイズ除去
}

#define BOUNDARY_KEY "===="                   // 区切り用のランダム文字列
#define BOUNDARY "\r\n--" BOUNDARY_KEY "\r\n" // 区切り
httpd_handle_t stream_httpd = NULL;

static esp_err_t stream_handler(httpd_req_t *req) // 動画送信 [ MJPEG over HTTP ]
{
  char part_buf[64];
  size_t part_len;

  Serial.println("Start stream");
  digitalWrite(LED_BUILTIN, LOW); // LED ON

  esp_err_t res = httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=" BOUNDARY_KEY); //  httpタイプを指定： サーバーが任意のタイミングで複数の文書を返し、 紙芝居的にレンダリングを切り替えさせる
  if (res == ESP_OK)
  {
    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*"); // レスポンスヘッダーに追加。どのオリジンからも叩けるように指定する。
    if (res == ESP_OK)
    {
      res = httpd_resp_set_hdr(req, "X-Framerate", "60"); // レスポンスヘッダーに追加。
      if (res == ESP_OK)
      {
        uint64_t st = millis();
        int fps = 0;

        while (res == ESP_OK)
        {                                        // 繰り返し画像を送信（ MJPG ）レスポンスエラーが起きない限り、ここを無限ループ
          camera_fb_t *fb = esp_camera_fb_get(); // カメラ映像を得る
          if (fb)
          {
            part_len = snprintf(part_buf, sizeof(part_buf), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);

            res = httpd_resp_send_chunk(req, BOUNDARY, strlen(BOUNDARY)); // 区切り送信
            if (res == ESP_OK)
            {
              res = httpd_resp_send_chunk(req, part_buf, part_len); // PART部分の送信
              if (res == ESP_OK)
              {
                res = httpd_resp_send_chunk(req, (char *)fb->buf, fb->len); // JPEGデータの送信
              }
            }
            esp_camera_fb_return(fb); // カメラリソース開放

            //------ FPS表示（必要ないときはコメントアウトして下さい）----------
            fps++; // FPS計算
            uint64_t ct = millis();
            if (ct - st > 1000)
            {
              Serial.printf("fps = %d\n", fps); // 1SごとにFPS表示
              fps = 0;
              st = ct;
            }
            //----------------------------------------------------------------
          }
          else
          {
            Serial.println("Camera capture failed");
            httpd_resp_send_500(req);
            res = ESP_FAIL;
            break;
          }
        }
      }
    }
  }
  Serial.printf("Stop stream. HTTP Response = %X\n", res);
  digitalWrite(LED_BUILTIN, HIGH); // LED OFF
  return res;
}

void startCameraServer()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // デフォルト設定
  config.server_port = 80;

  httpd_uri_t index_uri = {
      // リクエストが来た時の処理
      .uri = "/",                // URI
      .method = HTTP_GET,        // GETメソッド
      .handler = stream_handler, // カメラの画像データを送るハンドラ
      .user_ctx = NULL,
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK)
  {                                                       // HTTPサーバ開始
    httpd_register_uri_handler(stream_httpd, &index_uri); // ストリーミング用ハンドラ登録
    Serial.println("Start Camera server");
  }
  else
  {
    Serial.println("Start Camera server Error");
  }
}

void stopCameraServer()
{
  if (stream_httpd)
  {
    httpd_stop(stream_httpd);
    stream_httpd = NULL;
  }
}

void cam_start()
{
#ifdef WIFI_DIRECT
  if (!oldconnected && wificonnected)
  { // WiFiに接続されたら
    oldconnected = true;
    startCameraServer(); // カメラストリーミング開始
  }
  else if (oldconnected && !wificonnected)
  {
    oldconnected = false;
    stopCameraServer(); // カメラストリーミング停止
  }
#else
  if (!wificonnected && WiFi.status() == WL_CONNECTED)
  {
    wificonnected = true;
    startCameraServer(); // カメラストリーミング開始
  }
  else if (wificonnected && WiFi.status() == WL_DISCONNECTED)
  {
    wificonnected = false;
    stopCameraServer(); // カメラストリーミング停止
  }
  if (!wificonnected)
    wifi_start(); // WiFi切れたら再接続
#endif
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED Off

  cam_init();   // カメラ初期化
  wifi_start(); // WiFi開始
}

/*************  ✨ Codeium Command ⭐  *************/
/**
 * @brief Loop function
 *
 * This function is called repeatedly from the Arduino framework.
 * It starts the camera streaming server and waits 1 second.
 */
/******  2253728f-486b-4f3f-aa4e-be56471faaa9  *******/ void loop()
{
  cam_start(); // カメラストリーミング開始
  delay(1000);
}