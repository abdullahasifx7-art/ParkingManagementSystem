# 🏨 Grand Valet Parking Management System

<div align="center">

![C++](https://img.shields.io/badge/Language-C++-blue?style=for-the-badge&logo=cplusplus)
![SFML](https://img.shields.io/badge/GUI-SFML%202.6-green?style=for-the-badge)
![OOP](https://img.shields.io/badge/Paradigm-OOP-gold?style=for-the-badge)
![VS2022](https://img.shields.io/badge/IDE-Visual%20Studio%202022-purple?style=for-the-badge&logo=visualstudio)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)

**A production-grade Parking Lot Management System engineered in C++ with a fully custom SFML GUI.**  
*Luxury hotel aesthetic · Dark navy & gold theme · Real-time slot grid · Complete admin portal*

[Features](#-features) · [Screenshots](#-screenshots) · [Getting Started](#-getting-started) · [Architecture](#-architecture) · [Team](#-team)

</div>

---

## 📌 Overview

This is not a basic console application.

Grand Valet Parking is a fully functional, dual-role parking management system built entirely from scratch in C++ with a hand-crafted SFML graphical interface — no UI frameworks, no shortcuts. Every screen, button, input field, and color was written manually.

The backend is architected using core OOP principles: abstract classes, polymorphism, dynamic memory management, and file-based persistence. The GUI layer sits cleanly on top, calling backend methods directly — zero business logic in the UI files.

Built as a collaborative team project for the Object Oriented Programming course at FAST NUCES, Spring 2026.

---

## ✨ Features

### 👤 User Portal
| Feature | Description |
|---|---|
| Park Vehicle | Select vehicle type (Car / Bike / Truck), enter plate and owner, get assigned slot and Entry ID |
| Exit & Bill | Enter Entry ID, receive formatted receipt with duration and total charge |
| Real-time Slot Grid | Color-coded grid — 🟢 Available · 🔴 Occupied · 🟡 Maintenance |
| Find My Vehicle | Search any parked vehicle instantly by plate number |

### 🔐 Admin Portal
| Feature | Description |
|---|---|
| Secure Login | Username + password authentication from encrypted credentials file |
| Dashboard | Live statistics — total slots, available, occupied, current rates |
| Parked Vehicles | Full table of all currently parked vehicles with entry times |
| Parking History | Complete transaction log with search and filter by plate or Entry ID |
| Revenue Reports | Generate summaries by day / week / month with export to file |
| Rate Management | Update parking rates per vehicle type in real time |
| Slot Management | Add or remove parking slots dynamically at runtime |
| Set Slot Status | Mark any slot as available, occupied, or maintenance |

---

## 🏗 Architecture

```
ParkingManagementSystem/
├── Project/
│   ├── Vehicle.h / .cpp          ← Abstract base class (pure virtual getRate())
│   ├── Car.h / .cpp              ← Derived: Rs 2/min
│   ├── Bike.h / .cpp             ← Derived: Rs 1/min
│   ├── Truck.h / .cpp            ← Derived: Rs 3/min
│   ├── ParkingSlot.h / .cpp      ← Single slot model + status management
│   ├── ParkingLot.h / .cpp       ← Slot array management + assignment logic
│   ├── Bill.h / .cpp             ← Receipt calculation + export
│   ├── Admin.h / .cpp            ← Authentication + management actions
│   ├── FileHandler.h / .cpp      ← All file I/O (slots, history, rates, admin)
│   ├── main_gui.cpp              ← SFML GUI — all 11 screens
│   ├── assets/
│   │   ├── fonts/arial.ttf
│   │   └── images/               ← background, logo, car, bike, truck
│   └── data/
│       ├── slots.txt             ← Persisted slot states
│       ├── history.txt           ← Full transaction log
│       ├── rates.txt             ← Parking rates
│       └── admin.txt             ← Admin credentials
```

### Class Hierarchy
```
Vehicle  (abstract)
├── Car      → getRate() = 2.0f
├── Bike     → getRate() = 1.0f
└── Truck    → getRate() = 3.0f

ParkingLot  →  owns  →  ParkingSlot[]
Bill        →  uses  →  Vehicle*
Admin       →  manages  →  ParkingLot*
FileHandler →  persists  →  everything
```

---

## 🚀 Getting Started

### Prerequisites
- Visual Studio 2022 (Community or higher)
- SFML 2.6.1 — [Download here](https://www.sfml-dev.org/download/sfml/2.6.1/) → choose **Visual C++ 17 (2022) — 64-bit**

### SFML Setup in Visual Studio
1. Extract SFML to `C:\SFML-2.6.1\`
2. Right-click Project → **Properties** → set **All Configurations** + **x64**
3. **C/C++ → Additional Include Directories** → add `C:\SFML-2.6.1\include`
4. **Linker → Additional Library Directories** → add `C:\SFML-2.6.1\lib`
5. **Linker → Input → Additional Dependencies** → add:
   ```
   sfml-graphics-d.lib
   sfml-window-d.lib
   sfml-system-d.lib
   ```
6. Copy all `.dll` files from `C:\SFML-2.6.1\bin\` into `Project\x64\Debug\`

### Build & Run
1. Clone the repository
   ```
   git clone https://github.com/abdullahasifx7-art/ParkingManagementSystem.git
   ```
2. Open `Project.sln` in Visual Studio 2022
3. Right-click each `.cpp` file → Add to Project (if not already added)
4. Create `data/` folder with these files:
   ```
   data/admin.txt     →  admin:admin123
   data/slots.txt     →  (leave empty)
   data/history.txt   →  (leave empty)
   data/rates.txt     →  (leave empty)
   ```
5. **Build → Rebuild Solution** (`Ctrl+Shift+B`)
6. Run with **F5**

### Default Credentials
```
Username: admin
Password: admin123
```

---

## 🎨 GUI Screens

| Screen | Description |
|---|---|
| Welcome | Animated loading screen with progress bar (3 seconds) |
| Role Select | Choose Admin or User with live slot availability stats |
| Admin Login | Secure credential entry with error feedback |
| Admin Dashboard | Live stat cards + mini slot grid overview + quick actions |
| Park Vehicle | Type selector, plate + owner input, success receipt |
| Exit Vehicle | Entry ID lookup, full billing receipt display |
| Slot Grid | Full scrollable color-coded grid of all 50 slots |
| Parked Vehicles | Live table of all occupied slots |
| History | Searchable transaction log with export |
| Revenue Report | Period selector (day/week/month) + totals |
| Rate Management | Per-type rate editor |
| Manage Slots | Add / remove / set status (maintenance / available) |
| Find Vehicle | Plate number search with full vehicle details |
| User Menu | User portal home with 4 quick-access cards |

---

## 🔑 OOP Concepts Used

| Concept | Where Applied |
|---|---|
| Abstract Class | `Vehicle` — pure virtual `getRate()` |
| Inheritance | `Car`, `Bike`, `Truck` extend `Vehicle` |
| Polymorphism | `Vehicle*` array dispatches correct `getRate()` at runtime |
| Encapsulation | All private members with getters/setters |
| Dynamic Memory | `ParkingSlot**` array — manual `new` and `delete` |
| File I/O | `FileHandler` — all persistence in CSV format |
| Static Methods | `FileHandler` — all methods are static |
| Composition | `ParkingLot` owns `ParkingSlot` objects |

---

## 👥 Team

**Team #1 · Section BSE-2A · FAST NUCES · Spring 2026**

| Name |
|---|
| M. Abdullah Asif |
| M. Shaffay Khan |
| Abdul Wadood |
| M. Abdullah Yousaf |
| Farhad Amer |

---

## 📄 License

This project was built for academic purposes as part of the Object Oriented Programming course at FAST NUCES, Spring 2026.

---

<div align="center">
<b>Grand Valet Parking Management System</b><br>
FAST NUCES · BSE-2A · OOP · Spring 2026
</div>
