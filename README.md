# Fitman™ – AI-Controlled Arduino Runner Game

![Fitman Logo](Media/photos/fitmanlogo.png) <!-- Optional -->

## 🕹️ Overview
**Fitman™** is a side-scroller runner game created using the Arduino UNO R4 and a 16x2 LCD. The player is controlled using AI that interprets body poses — jumping when the player stands and running while crouched. It combines embedded electronics, custom LCD graphics, and machine learning for human interaction.

---

## 🧰 Materials Used
- Arduino UNO R4
- 16x2 LCD Screen
- Breadboard
- Jumper Wires
- Laptop (for programming and running AI input system)

---

## 🔌 Final Circuit Design

**LCD Pin Configuration:**

| LCD Pin | Connection      |
|---------|-----------------|
| RS      | Pin 11 (Arduino)|
| RW      | GND             |
| E       | Pin 10          |
| D4      | Pin 9           |
| D5      | Pin 8           |
| D6      | Pin 4           |
| D7      | Pin X (update!) |
| VSS     | GND             |
| VDD     | 5V              |
| VO      | GND             |
| A       | 5V              |
| K       | GND             |

*Note: Pin X should be replaced with the correct D7 value.*

---

## 🧠 Game Mechanics

- **Display:** LCD shows terrain on top/bottom row.
- **Character Movement:** Auto-run, jump on player input.
- **Score:** Distance counter increases over time.
- **Collision Detection:** Game ends if player hits obstacle.

### 🖼️ Graphics
- Custom sprite blocks for:
  - `run1`, `run2`
  - `jump` (upper/lower)
  - `solid terrain`, `edge blocks`
- Sprites are drawn using CGRAM on the LCD.

### 🔁 Game Loop
- Move terrain each cycle
- Animate run/jump sprites
- Update screen + score
- Detect and respond to collisions

### 🎮 Input Handling
- **Standing Pose:** AI sends `jump` signal to Arduino
- **Crouching Pose:** Character continues running

---

## ⚙️ Challenges Faced

| Issue | Solution |
|-------|----------|
| LCD wouldn’t display with 220Ω resistor | Switched to RW pin fix (no resistor) |
| AI input lag | Switched from `if()` to `while()` for faster updates |
| Integration of Teachable Machine to Arduino | Required manual input parsing and timing control |

---

## 🧪 Improvements & Next Steps
- 3D-printed parts for neat assembly
- Smooth animation handling
- More stable, readable game logic
- High score saving logic (EEPROM)
- Add difficulty levels & power-ups

---

## 🗂️ Project Files

- **Final Presentation (Interactive)** – [View on Genially](https://view.genially.com/6846401c180c2c8579636b23/presentation-fitman)

---

## 📸 Demo and Media


  
