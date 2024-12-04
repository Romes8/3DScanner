import serial
import cv2
import time

print("Starting the camera app ...")
# Initialize serial connection (adjust 'COM3' and baud rate as needed)
ser = serial.Serial('COM3', 9600, timeout=1)
time.sleep(2)  # Wait for connection to establish

# Initialize camera
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

while True:
    if ser.in_waiting > 0:
        command = ser.readline().decode('utf-8').strip()
        if command == "1":
            ret, frame = cap.read()
            if ret:
                timestamp = time.strftime("%Y%m%d-%H%M%S")
                filename = f'image_{timestamp}.png'
                cv2.imwrite(filename, frame)
                print(f"Image saved as {filename}")
                ser.write(b"2")

            else:
                print("Failed to capture image")
    
    # Optional: Add a small delay
    time.sleep(0.1)

# Release resources
cap.release()
ser.close()