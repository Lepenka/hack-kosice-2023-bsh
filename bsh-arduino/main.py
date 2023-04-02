import serial
import uvicorn
from fastapi import FastAPI

app = FastAPI()

ser = serial.Serial('/dev/cu.usbmodem1203', 9600)  # configure the serial port

@app.get("/send-message/{message}")
async def send_message(message: str):
    ser.write((str(message) + "_0_0\n").encode())  # send the message over serial
    print((str(message) + "_0_0\n"))
    return {"message": f"Sent message: {message}"}

@app.get("/gyro/{message}")
async def gyro(message: str):
    ser.write((str(message) + "\n").encode())  # send the message over serial
    print((str(message) + "\n"))
    return {"message": f"Sent message: {message}"}

if __name__ == "__main__":
    uvicorn.run(app, host="10.0.5.96", port=8000)
