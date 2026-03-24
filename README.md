# BBB SPI Driver

Simple SPI driver for BeagleBone Black.

## 📖 Description

This project implements a basic SPI driver for an IMU (Inertial Measurement Unit) on BeagleBone Black using Embedded Linux.
It focuses on low-level SPI communication, device interaction, and kernel-level configuration via Device Tree Overlays.

The driver serves as a foundation for working with IMU sensors (e.g. accelerometer, gyroscope) in embedded Linux environments.

Additionally, a custom Device Tree Overlay is included to enable and configure the SPI interface on the board.

---

## ⚙️ Features

* SPI interface initialization
* Basic data transmission (send/receive)
* Custom Device Tree Overlay for SPI

---

## ✅ Done

* [x] IMU initialization
* [x] Basic data transmission
* [x] Custom Device Tree Overlay
* [x] Basic accelerometr support

---

## 🔧 TODO

* [ ] Runtime configuration update
* [ ] Add gyroscope support
* [ ] Create example application

## 🧩 Device Tree Overlay Setup

Follow these steps to upload and enable the SPI overlay on BeagleBone Black.

### 1. Transfer overlay to the board

```bash
scp overlay/BBB-IMU-CUSTOM.dtbo debian@192.168.7.2:/home/debian/drivers
```

### 2. Move overlay to firmware directory (on BBB)

```bash
cp /home/debian/drivers/BBB-IMU-CUSTOM.dtbo /lib/firmware/
```

### 3. Modify `/boot/uEnv.txt`

Add the following line:

```bash
uboot_overlay_addr4=/lib/firmware/BBB-IMU-CUSTOM.dtbo
```

### 4. Reboot the board

```bash
reboot
```

---

## 🚀 Tests

<img width="857" height="472" alt="image" src="https://github.com/user-attachments/assets/4d99e4ff-eeb5-43a8-8c3f-e821adf9381b" />

<img width="849" height="130" alt="image" src="https://github.com/user-attachments/assets/3b342861-bf13-4c5a-a3c6-b663cd94f127" />



---
