# Soccer Robot

4-wheel drive robot controlled with an Xbox gamepad over Bluetooth. Built with an ESP32 and two TB6612FNG motor drivers.

## Hardware

- ESP32-WROOM-32 (38-pin devkit)
- 2x TB6612FNG dual H-bridge motor drivers
- 4x DC motors
- 12V LiPo battery
- LM2596 buck converter (12V → 5V)
- 8A fuse on the 12V rail
- Xbox controller (or any Bluetooth gamepad)

## Pin Wiring

| Motor | PWM | IN1 | IN2 | Side |
|-------|-----|-----|-----|------|
| MOT1  | 4   | 17  | 16  | Left |
| MOT2  | 21  | 18  | 19  | Left |
| MOT3  | 32  | 25  | 33  | Right |
| MOT4  | 14  | 26  | 27  | Right |

STBY pins on both TB6612FNG drivers are wired directly to 5V.

## Project Structure

```
├── include/
│   ├── config.h       # pin definitions, speed limits, timing
│   ├── motors.h       # motor function declarations
│   └── rc.h           # gamepad function declarations
├── src/
│   ├── main.cpp       # setup + main loop
│   ├── motors.cpp     # motor control, soft start, protections
│   └── rc.cpp         # bluetooth gamepad reading, arcade drive
├── test/
│   └── test_motors.cpp  # standalone motor test (no bluetooth)
└── platformio.ini     # build configs
```

## Setting Up the Environment

### 1. Install VS Code

Download and install from [code.visualstudio.com](https://code.visualstudio.com/).

### 2. Install PlatformIO Extension

1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for **"PlatformIO IDE"**
4. Click Install
5. Wait for it to finish — it takes a minute, it installs a bunch of stuff in the background
6. Restart VS Code when it asks

### 3. Open the Project

1. File → Open Folder
2. Navigate to this project folder (`Soccer Robo`)
3. Open it — PlatformIO should detect the `platformio.ini` automatically
4. Wait for PlatformIO to download the ESP32 platform and libraries (first time only, takes a few minutes)

### 4. Connect the ESP32

1. Plug the ESP32 into your PC with a USB cable
2. Make sure you have the right USB driver:
   - CP2102 boards → [CP210x driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
   - CH340 boards → [CH340 driver](http://www.wch-ic.com/downloads/CH341SER_EXE.html)
3. Check that a COM port shows up in Device Manager

## Building and Flashing

### Main firmware (with Bluetooth gamepad control)

Click the **→ (Upload)** button in the PlatformIO toolbar at the bottom of VS Code.

Or use the terminal:
```bash
pio run -e esp32dev -t upload
```

### Motor test only (no Bluetooth needed)

Use this to check if your motors are wired correctly before connecting a gamepad:

```bash
pio run -e test_motors -t upload
```

Then open the serial monitor to see the test output:
```bash
pio device monitor
```

The test will:
1. Spin each motor individually (forward then reverse)
2. Spin all 4 motors together
3. Test the soft-start ramp
4. Simulate arcade drive commands

**Put the robot on a stand before running the test!**

## Serial Monitor

To see debug output:

```bash
pio device monitor
```

Or click the **plug icon** in the PlatformIO toolbar. Baud rate is 115200.

## How It Works

The gamepad left stick controls the robot using arcade drive:
- **Stick up/down** → forward/backward
- **Stick left/right** → turn

The math is simple:
```
left_speed  = Y + X
right_speed = Y - X
```

### Protections built into the firmware

- **Soft start** — speed ramps up/down gradually instead of jumping, avoids current spikes
- **Force-through-zero** — when reversing direction, speed goes to 0 first before going the other way
- **Brake before reverse** — brief 20ms brake when direction flips, lets back-EMF die down
- **Min PWM filter** — duty cycles below 15 are treated as 0 since the motor would just buzz without spinning
- **Watchdog** — if the control loop freezes for 500ms, motors auto-stop
- **Disconnect safety** — if the gamepad disconnects, motors stop immediately

## Tuning

In `config.h`:

| Parameter | Default | What it does |
|-----------|---------|--------------|
| `MAX_PWM` | 230 | top speed limit (out of 255) |
| `STEP_SIZE` | 5 | how fast speed ramps up/down |
| `JOYSTICK_DEADZONE` | 20 | ignore stick drift below this |
| `MIN_PWM` | 15 | minimum duty to actually spin a motor |
| `DIRECTION_BRAKE_MS` | 20 | brake time when reversing direction |
| `WATCHDOG_TIMEOUT_MS` | 500 | how long before watchdog kills motors |
