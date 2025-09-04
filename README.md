# Apple Magic Mouse and Keyboard Drivers

Windows kernel mode drivers for Apple Magic Mouse and Keyboard devices with **Secure Boot compatibility**.

## Project Structure

- **Drivers/AppleInputFilter** (KMDF HID Upper Filter): Enhanced input filtering with security hardening
- **Drivers/ApplePTPVhf** (KMDF + VHF PTP): Virtual Precision Touchpad with secure input validation  
- **Apple_KB_N_Mouse**: Main HID filter driver with comprehensive security measures
- **Service/AppleInputService** (.NET 8 Worker): User-mode service for driver communication
- **Common/include**: Shared IOCTL headers and security contracts

## Secure Boot Compliance

✅ **Universal Platform Targeting**: All drivers target Universal platform for Secure Boot compatibility  
✅ **Enhanced Signing**: Production certificate signing support with SHA256 algorithms  
✅ **Security Hardening**: Parameter validation, buffer bounds checking, and input sanitization  
✅ **INF Compliance**: Security descriptors, PnP lockdown, and proper service permissions  

## Quick Start

### Prerequisites
- Visual Studio 2022 with Windows Driver Kit (WDK)
- Windows SDK 10.0.22621.0 or later
- PowerShell for build scripts and validation

### Building
1. Open `Apple_KB_N_Mouse.sln` in Visual Studio 2022
2. Right-click solution → **Retarget Projects** (if needed)
3. Build solution for desired platform (x64/ARM64)

### Secure Boot Validation
```powershell
# Validate Secure Boot compliance
pwsh scripts/Validate-SecureBoot.ps1

# Package drivers for distribution
pwsh scripts/Package-Drivers.ps1 -Configuration Release -Platforms x64,ARM64
```

### Production Signing
```powershell
# Build with production certificate
msbuild /p:Configuration=Release /p:Platform=x64 /p:EnableProductionSign=true /p:ProductionCertThumbprint="YourCertThumbprint"
```

## Security Features

- **Parameter Validation**: All entry points validate input parameters
- **Buffer Protection**: IOCTL handlers include bounds checking and overflow prevention
- **Service Security**: Restricted permissions (System/Admin only)
- **Code Integrity**: Enhanced error handling prevents information disclosure
- **Signing Infrastructure**: Production-ready certificate signing support

## Documentation

- **[Secure Boot Compliance Guide](SECURE_BOOT_COMPLIANCE.md)**: Detailed documentation of security improvements
- **Driver Architecture**: See individual driver README files in respective directories
- **Security Validation**: Use `scripts/Validate-SecureBoot.ps1` to verify compliance

---

### SDK/WDK Auto-Targeting
This solution was patched to target:
- Windows SDK: 10.0.22621.0 or later
- Windows Driver Kit: 10.0.22621.0 or later

If your machine has a different version, Right-click the project → **Retarget Projects…**.
