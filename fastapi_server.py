# This Python file uses the following encoding: utf-8

# pip install fastapi uvicorn pydantic
# python fastapi_server.py


from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn

app = FastAPI()

class WifiInfo(BaseModel):
    ssid: str
    strength: int

@app.post("/wifi/update")
def update_wifi(info: WifiInfo):
    print(f"📡 받은 Wi-Fi 정보: SSID={info.ssid}, 강도={info.strength}")
    status = "Excellent" if info.strength > 80 else "Good" if info.strength > 50 else "Weak"
    recommendation = "Stable" if status != "Weak" else "Move closer to router"
    return {"status": status, "recommendation": recommendation}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)


