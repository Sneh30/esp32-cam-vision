# ESP32-CAM Vision Suite

A collection of three ESP32-CAM projects for computer vision and edge AI — all powered by the **AI-Thinker ESP32-CAM** module.

This suite was built as part of a locally-run **Visual LLM** system on ESP32 hardware.

---

## Projects

### 1. 📸 Collect_Images_for_EdgeImpulse

Web-based image capture server for building ML training datasets.

- Captures 240×240 square images (face resolution)
- Serves a web UI for remote image collection
- Designed for **Edge Impulse** image classification / object detection workflows
- Uses the `eloquent_esp32cam` library

**Files:** `Collect_Images_for_EdgeImpulse/`

### 2. 🟢 ColorTrackingBot

Autonomous color-tracking robot that detects and follows a **green ball** using real-time HSV filtering on the ESP32-CAM.

- RGB565 → RGB888 → HSV conversion on-device
- Centroid calculation for target position
- L298N motor driver control (forward, left, right, stop)
- Configurable HSV thresholds, min/max pixel counts

**Files:** `ColorTrackingBot/`

### 3. 🎥 ESPCameraWebServer

Full-featured MJPEG video streaming and camera control web server.

- Live MJPEG streaming at up to 60 FPS
- Still image capture (JPEG and BMP)
- Adjustable: resolution, quality, brightness, contrast, saturation, WB, gain, exposure
- Special effects, mirror, flip, AE level
- Advanced: register-level access, PLL tuning, sensor windowing
- OTA-ready flash partition layout
- Supports 15+ camera board models (AI-Thinker, M5Stack, ESP-EYE, ESP32S3-EYE, etc.)

**Files:** `ESPCameraWebServer/`

---

## Hardware Requirements

- **ESP32-CAM (AI-Thinker)** — the primary target (PSRAM recommended)
- FTDI programmer (or ESP32-CAM programmer) for flashing
- For ColorTrackingBot: L298N motor driver + DC motors + power supply

Other supported boards (ESPCameraWebServer): WROVER Kit, ESP-EYE, M5Stack variants, TTGO T-Journal, XIAO ESP32S3, DFRobot boards

---

## Getting Started

### WiFi Setup

1. Copy `wifi_config.h.example` into each project directory that needs it:
   ```bash
   cp wifi_config.h.example Collect_Images_for_EdgeImpulse/wifi_config.h
   cp wifi_config.h.example ESPCameraWebServer/wifi_config.h
   ```
2. Edit each `wifi_config.h` with your WiFi SSID and password.

⚠️ `wifi_config.h` is gitignored — your credentials will never be committed.

### Option A: PlatformIO (recommended)

```bash
# Install PlatformIO, then build and upload any project:
pio run -d Collect_Images_for_EdgeImpulse -t upload
pio run -d ColorTrackingBot -t upload
pio run -d ESPCameraWebServer -t upload
```

Each project has its own `platformio.ini`.

### Option B: Arduino IDE

1. Install the ESP32 board package (Tools → Board → Boards Manager → `esp32`)
2. Install required libraries (see `platformio.ini` in each project for dependencies)
3. Select **AI Thinker ESP32-CAM** as the board
4. Set **Partition Scheme** → `Custom (3MB APP/1MB SPIFFS)` for ESPCameraWebServer
5. Set **Core Debug Level** → `Info` for Collect_Images_for_EdgeImpulse
6. Open the `.ino` file, compile and upload

---

## Partition Scheme

Only ESPCameraWebServer needs the custom partition table (`partitions.csv`).

| Partition | Size | Purpose |
|---|---|---|
| nvs | 20 KB | Non-volatile storage |
| otadata | 8 KB | OTA boot metadata |
| app0 | 3.75 MB | Main firmware (OTA slot 0) |
| fr | 128 KB | Frame buffer |
| coredump | 64 KB | Crash dump |

---

## License

Apache 2.0 (ESPCameraWebServer) — see file headers for details.
