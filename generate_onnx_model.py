# This Python file uses the following encoding: utf-8

import torch
import torch.nn as nn

class SimpleModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.fc = nn.Sequential(
            nn.Linear(10, 32),
            nn.ReLU(),
            nn.Linear(32, 1)
        )
    def forward(self, x):
        return self.fc(x)

# 모델 생성 및 export
model = SimpleModel()
dummy_input = torch.randn(1, 10)
torch.onnx.export(
    model, dummy_input, "wifi_predictor.onnx",
    input_names=["signal_history"],
    output_names=["predicted_quality"],
    opset_version=11
)

# 0. pip install torch numpy onnx

# 1. ONNX 모델 생성 (한 번만)
# python export_wifi_predictor.py

# 2. FastAPI 서버 실행
# python fastapi_server.py

# 3. Qt 프로젝트 빌드 및 실행
# cmake -B build
# cmake --build build
# ./build/WiFiOptimizerProject
