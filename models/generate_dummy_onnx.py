import torch
import torch.nn as nn
import torch.onnx

class DummyModel(nn.Module):
    def __init__(self):
        super(DummyModel, self).__init__()
        self.conv = nn.Conv2d(3, 1, kernel_size=3, stride=1, padding=1)

    def forward(self, x):
        return self.conv(x)

def main():
    model = DummyModel()
    dummy_input = torch.randn(1, 3, 640, 640)
    torch.onnx.export(model, dummy_input, "dummy.onnx", verbose=False)
    print("Dummy ONNX model generated successfully.")

if __name__ == "__main__":
    main()
