"""
Exercise 04: Real-Time IMU Visualization

Complete all TODO sections.
"""

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# =====================
# TODO (1): Set correct serial port
# =====================
# HINT: Find your board's COM port (Windows) or /dev/ttyUSB* (Linux/Mac)
# Common Windows ports: COM3, COM4, COM5, COM6
PORT = 'COM5'   # TODO: Change to your board's port
BAUD = 115200

WINDOW_SIZE = 100

# =====================
# TODO (2): Initialize serial connection
# =====================
# HINT: Use serial.Serial(PORT, BAUD) to create connection
ser = None


# =====================
# TODO (3): Create buffers for ax, ay, az, gyrX, gyrY, gyrZ
# =====================
# HINT: Use deque(maxlen=WINDOW_SIZE) for each sensor axis
ax_data = None
ay_data = None
az_data = None
gyrX_data = None
gyrY_data = None
gyrZ_data = None


# =====================
# TODO (4): Initialize text variables
# =====================
# HINT: Store orientation and both gesture types
orientation_text = ""
fsm_gesture_text = ""
gyro_gesture_text = ""


# =====================
# TODO (5): Setup plot with dual subplots
# =====================
# HINT: Create 2 vertical subplots, 3 lines per subplot, text displays, and configure axes
# HINT: Accelerometer subplot: y-range [-15, 15], title "Accelerometer Data (m/s²)"
# HINT: Gyroscope subplot: y-range [-500, 500], title "Gyroscope Data (°/s)"
fig = None
ax1 = None
ax2 = None
line_ax = None
line_ay = None
line_az = None
line_gyrX = None
line_gyrY = None
line_gyrZ = None
text_display1 = None
text_display2 = None


# =====================
# TODO (6): Parse serial line
# =====================
# HINT: New serial format from Arduino:
# "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
#
def parse_line(line):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    try:
        # HINT: Serial format: "ax: X | ay: Y | az: Z | gyrX: X | gyrY: Y | gyrZ: Z | Orientation: ... | FSM: ... | Gyro: ..."
        # TODO: Split by '|' and extract 6 sensor values (ax, ay, az, gyrX, gyrY, gyrZ)
        # TODO: Extract 3 text strings (orientation_text, fsm_gesture_text, gyro_gesture_text)
        # TODO: Return tuple (ax_val, ay_val, az_val, gyrX_val, gyrY_val, gyrZ_val)
        
        return None

    except:
        return None


# =====================
# TODO (7): Update function
# =====================
def update(frame):
    global orientation_text, fsm_gesture_text, gyro_gesture_text

    # TODO: Read all available serial data, parse, and append to buffers
    # TODO: Update all 6 line objects (3 accel + 3 gyro)
    # TODO: Update both text displays
    # TODO: Return all plot objects to redraw
    
    return []


# =====================
# TODO (8): Create animation
# =====================
# TODO: Create animation with FuncAnimation(fig, update, interval=50)
# TODO: Call plt.tight_layout() for proper spacing

plt.show()
