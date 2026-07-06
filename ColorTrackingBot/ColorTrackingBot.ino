#include <Arduino.h>
#include "esp_camera.h"
#include "CameraPins.h"
#include "MotorControl.h"

// ======== HSV TUNING FOR YOUR GREEN BALL ==========
const int TARGET_H = 95;          // from your measurement
const int H_TOL = 10;             // ±10 range (85–105)

const int MIN_S = 120;            // eliminate ambient green
const int MIN_V = 140;

const int MIN_PIXELS = 300;       // must be >300 to be considered real
const int STOP_CLOSE_PIXELS = 2000;  // if pixel count >2000 → ball is close → stop


// ======== CAMERA SETTINGS ==========
const framesize_t FRAME_SIZE = FRAMESIZE_QQVGA;

// Convert RGB565 → RGB888
static inline void rgb565_to_rgb888(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = ((rgb565 >> 11) & 0x1F) * 255 / 31;
  g = ((rgb565 >> 5) & 0x3F) * 255 / 63;
  b = (rgb565 & 0x1F) * 255 / 31;
}

// RGB888 → HSV (H:0-179 like OpenCV)
static void rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b, int &h, int &s, int &v) {
  float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;
  float mx = max(fr, max(fg, fb));
  float mn = min(fr, min(fg, fb));
  float d = mx - mn;

  v = mx * 255;
  s = (mx == 0 ? 0 : (d / mx) * 255);

  float hue = 0;
  if (d != 0) {
    if (mx == fr) hue = (fg - fb) / d;
    else if (mx == fg) hue = 2 + (fb - fr) / d;
    else hue = 4 + (fr - fg) / d;
    hue *= 60;
    if (hue < 0) hue += 360;
  }
  h = (hue / 360.0) * 179.0;
}

// Hue check with wrapping
bool inHueRange(int h) {
  int low = TARGET_H - H_TOL;
  int high = TARGET_H + H_TOL;

  if (low < 0) return (h >= 180 + low) || (h <= high);
  if (high >= 180) return (h >= low) || (h <= (high - 180));

  return (h >= low && h <= high);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // ---- CAMERA INIT ----
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAME_SIZE;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    while (1);
  }

  motorInit();
  stopMotors();
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return;

  long sumX = 0, sumY = 0, count = 0;

  for (int i = 0; i < fb->width * fb->height; i += 2) {  // step=2 faster
    uint16_t pix = fb->buf[i * 2] | (fb->buf[i * 2 + 1] << 8);

    uint8_t r, g, b;
    rgb565_to_rgb888(pix, r, g, b);

    int h, s, v;
    rgb_to_hsv(r, g, b, h, s, v);

    if (s >= MIN_S && v >= MIN_V && inHueRange(h)) {
      int x = i % fb->width;
      int y = i / fb->width;
      sumX += x;
      sumY += y;
      count++;
    }
  }

  esp_camera_fb_return(fb);

  // ===== NO OBJECT DETECTED =====
  if (count < MIN_PIXELS) {
    Serial.println("NO BALL → STOPPING");
    stopMotors();
    return;
  }

  // ===== COMPUTE CENTROID =====
  int cx = sumX / count;
  int center = fb->width / 2;
  int err = cx - center;

  Serial.printf("GREEN | size=%ld | cx=%d | err=%d\n", count, cx, err);

  // ===== BALL TOO CLOSE =====
  if (count > STOP_CLOSE_PIXELS) {
    Serial.println("BALL CLOSE → STOP");
    stopMotors();
    return;
  }

  // ===== TRACK NORMAL =====
  if (abs(err) < 10) {
    moveForward(150);
    Serial.println("FORWARD");
  } else if (err < 0) {
    turnLeft(120);
    Serial.println("LEFT");
  } else {
    turnRight(120);
    Serial.println("RIGHT");
  }
}
