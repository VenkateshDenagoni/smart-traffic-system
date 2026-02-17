# How to Interpret the System Output

When you run `traffic_system`, a visualization window will appear. Here is how to read the real-time data displayed on the dashboard and the terminal.

## 1. The Visual Dashboard (Top-Left HUD)

| Field | Meaning | Why it matters |
| :--- | :--- | :--- |
| **Mode** | `ADAPTIVE` or `FIXED` | Shows if the AI is making decisions based on traffic (`ADAPTIVE`) or just following a timer (`FIXED`). |
| **Current Lane** | `0, 1, 2, or 3` | Identifies which lane currently has the right of way. |
| **Signal State** | `GREEN`, `YELLOW`, or `RED` | **GREEN**: Traffic is moving.<br>**YELLOW**: Transitioning (3s mandatory).<br>**RED**: All other lanes are stopped. |
| **Timer** | `(XXs)` | The countdown until the current state *must* end (Max Green Time) or until the light changes. |
| **Latency** | `XX ms` | The time it takes for the computer to process one frame. Lower is better (e.g., <100ms). |
| **FPS** | `XX.X` | Frames Per Second. Shows how "smooth" the video is. 10+ is good for traffic sensing. |

## 2. Lane Visualization (Overlays)

- **Cyan Rectangles**: These are the **Regions of Interest (ROIs)** for each lane. The system only "counts" vehicles inside these boxes.
- **Green Rectangles**: These are **Detections**. If a car is outlined in green, the AI has successfully seen it.
- **Labels (e.g., North: 30%)**:
    - The number is the **Vehicle Count**.
    - The percentage is the **Density**. If it hits 100%, the lane is considered "Full."

## 3. Terminal Logs (The "Brain" Output)

In your VS Code terminal, you will see lines like this:
`Frame 45 | Latency: 88.0ms | Lane 1: GREEN (45s) | N:10% S:80% E:0% W:5%`

### How the AI Thinks (Adaptive Mode):
1. **Max-Pressure**: If the AI sees `S:80%` (South) and all other lanes are `<10%`, it will hold the Green light on the South lane to clear the jam.
2. **Anti-Starvation**: If a lane (e.g., East) has been `0%` for a long time, the AI won't ignore it forever. Once the "Starvation Clock" hits 120s, it will force a Green light for the East lane even if it's empty, just to ensure fairness.
3. **Yellow Safety**: You will notice the system *always* goes to Yellow for 3 seconds. This is a hard-coded safety rule that the AI cannot skip.

## 4. Performance Benchmarks

- **Ideal**: Latency < 50ms, FPS > 20.
- **Acceptable**: Latency < 100ms, FPS > 10.
- **Warning**: Latency > 200ms. The system is lagging; the "Real-Time" timers will feel slow.
