Apple_KB_N_Mouse replacement (generated 2025-08-25)

Layout mirrors your repo:
- Drivers/AppleInputFilter (UMDF 2 Upper Filter): INF + C++ skeleton + .vcxproj
- Drivers/ApplePTPVhf (KMDF + VHF PTP): INF + C skeleton + .vcxproj
- Service/AppleInputService (.NET 8 Worker): csproj + Program.cs
- Common/include: shared IOCTL headers

Open Apple_KB_N_Mouse.sln in VS 2022 + WDK, retarget SDK/WDK, and build.

---
### SDK/WDK Auto-Targeting
This solution was patched to target:
- Windows SDK: 10.0.22621.0
- Windows Driver Kit: 10.0.22621.0

If your machine has a different version, Right-click the project → **Retarget Projects…**.
