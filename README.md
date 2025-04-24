# 🦠 Pandemic: The RTOS Game  
> ELEC-H-410 | Real-Time Systems Project | 2021–2022

![Platform](https://img.shields.io/badge/platform-PSoC%20-blue)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green)
![Language](https://img.shields.io/badge/language-C-blue)
![Status](https://img.shields.io/badge/status-Completed-success)

## 🧩 Project Overview

**Pandemic: The RTOS Game** is a real-time programming challenge simulating the spread of a global pandemic. The goal? **Stop the infection and develop a vaccine before it's too late** — all under strict real-time constraints and using the FreeRTOS environment on a PSoC.

This project is part of the Real-Time Systems course at ULB, ELEC-H-410.

---

## 🎮 Game Objective

You must **automatically react to real-time events** triggered by the gameTask and take strategic actions using your own FreeRTOS tasks. The aim is to **research a cure, produce medicine, and quarantine infected individuals** before the pandemic wipes out the population.

### 🧠 Core Gameplay Elements

- **Vaccine Research:**  
  Every 3 seconds, clues are released. Respond within 3s to increase the `vaccineCntr`.

- **Disease Spread:**  
  Every 5 seconds, healthy population decreases by `5 - medicineCntr`. Medicine pills are consumed per use.

- **Random Contamination:**  
  Quarantine must be activated within **10ms** or 20% of the population gets infected instantly.

### 🎯 Win/Lose Conditions

- ✅ **Win** if `vaccineCntr` reaches 100%  
- ❌ **Lose** if `populationCntr` reaches 0%

---
