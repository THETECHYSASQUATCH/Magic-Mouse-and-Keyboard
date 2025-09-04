# Magic Mouse and Keyboard Driver

## Purpose
This project aims to create replacement drivers and supporting services for Apple's Magic Mouse and Magic Keyboard on Windows, enabling enhanced functionality and compatibility.

## Supported Devices
- Magic Mouse
- Magic Keyboard

> **Note:** The Touch ID button on Magic Keyboards is not supported and will not be mapped in this driver.

---

## Project Structure

The repository is organized as follows:

- **Drivers/AppleInputFilter (UMDF 2 Upper Filter):**
   - Contains files for the upper filter driver, written in C++.
   - Includes the driver `.INF` file and project files (`.vcxproj`).

- **Drivers/ApplePTPVhf (KMDF + VHF PTP):**
   - Contains files for the Kernel-Mode Driver Framework (KMDF) driver, written in C.
   - Includes Virtual HID Framework (VHF) for Precision Touchpad (PTP) functionality.
   - Includes the driver `.INF` file and project files (`.vcxproj`).

- **Service/AppleInputService (.NET 8 Worker):**
   - Contains files for a Windows service that supports or enhances the drivers.
   - Written in .NET 8, with `csproj` and `Program.cs`.

- **Common/include:**
   - Shared headers for Input/Output Control (IOCTL) functionality.

- **Solution File:**
   - `Apple_KB_N_Mouse.sln` for building the project in Visual Studio 2022.

---

## Build Instructions

1. Open the solution file (`Apple_KB_N_Mouse.sln`) in Visual Studio 2022.
2. Retarget the SDK/WDK versions to match your local environment, if necessary:
   - Right-click the project → **Retarget Projects…**
3. Build the solution.

---

## Contribution Guidelines

We welcome contributions to improve this project! Here are some ways you can help:

- Report issues or bugs.
- Suggest enhancements or new features.
- Submit pull requests for code improvements.

For detailed contribution guidelines, please refer to the `CONTRIBUTING.md` file (if available) or open an issue to discuss your ideas.

---

Thank you for supporting the Magic Mouse and Keyboard Driver project!
