# MultiRoblox Core — Ultimate Optimized Edition

MultiRoblox Core allows multiple Roblox instances to run simultaneously by safely handling the Roblox singleton mutex.

This version is designed to be:
- Extremely lightweight
- Low-end PC friendly
- Zero CPU usage
- Highly compatible with Roblox updates

---

## ✨ Features

- Run multiple Roblox instances at the same time
- Zero background CPU usage (true blocking wait)
- No loops, no polling, no performance impact
- Multi-mutex support for better Roblox compatibility
- Optimized for low-end systems
- Clean and minimal CLI core

---

## 🚀 Recommended Build

**Release | Win32 (x86)**

This build offers:
- Smaller executable size
- Lower memory usage
- Better performance on low-end systems

---

## 🧠 How it works

MultiRoblox Core acquires Roblox singleton mutexes and keeps them alive using an infinite blocking wait, ensuring:
- No CPU wakeups
- No scheduler overhead
- No impact on Roblox performance

---

## ▶️ Usage

1. Build the project in `Release | Win32`
2. Run `MultiRoblox.exe`
3. Launch multiple Roblox instances normally
4. Press `Ctrl + C` to exit and restore default behavior

---

## 📌 Notes

- This project does NOT modify Roblox files
- No injection or memory patching
- Safe, clean and minimal approach

---

## 🔮 Future Plans

- Tray icon support
- GUI-based MultiRoblox Manager
- Instance profiles
- Roblox optimization tools

---

## 📜 License

See LICENSE.txt