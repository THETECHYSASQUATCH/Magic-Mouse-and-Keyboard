# Testing Setup for Magic Mouse and Keyboard Driver

This document provides detailed steps to set up and test the Magic Mouse and Keyboard driver on a machine with Secure Boot enabled.

---

## Prerequisites

1. **Hardware and Devices:**
   - Apple Magic Mouse
   - Apple Magic Keyboard

2. **Software Requirements:**
   - Windows 10 or Windows 11
   - Visual Studio 2022 with Windows Driver Kit (WDK) and Windows SDK installed.
   - Administrator access to the testing machine.

---

## Steps to Load and Test the Driver with Secure Boot

### 1. Generate a Self-Signed Certificate

1. Open PowerShell as Administrator.
2. Run the following command to generate a self-signed certificate:
   ```powershell
   New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=MagicMouseKeyboardDriver" -CertStoreLocation Cert:\LocalMachine\My
   ```
3. Export the certificate:
   - Open `certlm.msc` (Local Machine Certificates).
   - Navigate to `Personal > Certificates`.
   - Find the certificate you just created (look for "MagicMouseKeyboardDriver").
   - Right-click > All Tasks > Export.
   - Export as a `.CER` file.

### 2. Sign the Driver

1. Use `signtool.exe` (part of the Windows SDK) to sign the driver:
   ```cmd
   signtool sign /v /s My /n "MagicMouseKeyboardDriver" /t http://timestamp.digicert.com Path\To\Driver.sys
   ```

### 3. Add the Certificate to Secure Boot

1. Copy the exported `.CER` file to a USB drive.
2. Reboot into your system's UEFI firmware settings.
3. Navigate to the Secure Boot configuration menu.
4. Add the certificate to the `db` or `KEK` database.
5. Save and exit UEFI settings.

### 4. Enable Test Mode (Optional for Development)
If you are testing and do not want to modify Secure Boot databases:

1. Open Command Prompt as Administrator.
2. Run the following command to enable Test Signing Mode:
   ```cmd
   bcdedit /set testsigning on
   ```
3. Restart your computer.

---

## Testing the Driver

1. Install the driver:
   - Use the `.INF` file to manually install the driver.
   - Ensure the driver is loaded without errors.

2. Verify Functionality:
   - Test the Magic Mouse and Keyboard for functionality.
   - Ensure all features (except Touch ID) work as expected.

3. Check for Errors:
   - Use Event Viewer to check for driver-related errors.
   - Use Driver Verifier to test for stability.

---

This document should help you set up and test your driver in a Secure Boot environment. For further assistance, feel free to open an issue in the repository.