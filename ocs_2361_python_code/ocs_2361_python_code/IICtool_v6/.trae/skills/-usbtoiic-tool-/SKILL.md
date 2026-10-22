---
name: "USBTOIIC-tool"
description: "Provides SPI communication functionality using USB2UARTSPIIICDLL library. Invoke when user needs SPI/I2C communication, USB device control, or embedded system interface operations."
---

# SPI Tool Skill

This skill provides comprehensive SPI (Serial Peripheral Interface) and I2C communication capabilities using the USB2UARTSPIIICDLL dynamic link library. It supports both Windows and Linux platforms with various architecture versions.

## Library Overview

The USB2UARTSPIIICDLL library provides:

### Core Functions
- **Device Management**: Open/Close USB devices, get device information
- **SPI Communication**: Master/Slave mode SPI operations
- **I2C Communication**: Master/Slave mode I2C operations
- **Additional Features**: ADC reading, PWM output, GPIO control, internal list operations

### Platform Support
- **Windows**: 32-bit/64-bit versions with ASCII, Unicode, and C# variants
- **Linux**: x86 and ARM architectures (32-bit/64-bit)

## Key Function Categories

### 1. Device Management
- `OpenUSBDevice()` - Open USB device connection
- `CloseUSBDevice()` - Close USB device connection
- `GetUSBDeviceInfo()` - Get device information

### 2. SPI Master Mode
- `ConfigSPIParam()` - Configure SPI parameters (clock rate, mode, etc.)
- `SPISendAndRcvData()` - Send and receive SPI data
- `SPISendData()` - Send SPI data only
- `SPIRcvData()` - Receive SPI data only

### 3. SPI Slave Mode
- `ConfigSPIParamSlave()` - Configure SPI slave parameters
- `SPISlavePreloadData()` - Preload data for slave mode
- `SPISlaveRcvData()` - Receive data in slave mode

### 4. I2C Master Mode
- `ConfigIICParam()` - Configure I2C parameters
- `IICSendAndRcvData()` - Send and receive I2C data
- `IICRegisterSend()` - Send data to I2C register
- `IICRegisterRead()` - Read data from I2C register

### 5. Additional Features
- **ADC**: `GetADCVal()` - Read analog values
- **PWM**: `PWMOut()`, `PWMClose()` - PWM signal generation
- **GPIO**: `IOSetAndRead()` - GPIO pin control
- **Internal List**: Sequence operations for complex protocols

## Usage Guidelines

### When to Use This Skill
1. **SPI Communication**: When you need to communicate with SPI devices
2. **I2C Communication**: When working with I2C bus devices
3. **USB Device Control**: When controlling USB-based SPI/I2C adapters
4. **Embedded Systems**: When interfacing with microcontrollers and sensors
5. **Protocol Development**: When developing custom communication protocols

### Common Use Cases
- Reading/writing to SPI flash memory
- Communicating with I2C sensors (temperature, humidity, etc.)
- Controlling GPIO pins on embedded devices
- Generating PWM signals for motor control
- Reading analog values from ADC inputs

## Implementation Notes

### Library Integration
- Use appropriate DLL/SO file based on platform and architecture
- Handle error codes returned by library functions
- Implement proper device initialization and cleanup
- Consider multi-threading for concurrent operations

### Best Practices
- Always check device connection status before operations
- Implement proper error handling and logging
- Use appropriate timeout values for communication
- Validate input parameters before calling library functions

## Example Code Structure

```python
# Example SPI communication
import ctypes

# Load the DLL
spi_dll = ctypes.WinDLL("USB2UARTSPIIICDLL.dll")

# Open device
device_handle = spi_dll.OpenUSBDevice()

# Configure SPI parameters
spi_dll.ConfigSPIParam(1000000, 0, 0, 0, device_handle)

# Send and receive data
send_data = (ctypes.c_ubyte * 4)(0x01, 0x02, 0x03, 0x04)
receive_data = (ctypes.c_ubyte * 4)()
spi_dll.SPISendAndRcvData(0, 0, 0, 0, send_data, receive_data, 4, 4, device_handle)

# Close device
spi_dll.CloseUSBDevice(device_handle)
```

This skill provides a comprehensive interface for SPI and I2C communication tasks using the USB2UARTSPIIICDLL library.