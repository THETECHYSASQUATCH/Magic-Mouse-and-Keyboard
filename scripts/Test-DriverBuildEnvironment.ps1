# Apple driver build diagnostics (read-only)
$ErrorActionPreference = 'Stop'
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (!(Test-Path $vswhere)) { Write-Host '[ERROR] Visual Studio Installer / vswhere missing'; exit 1 }
$vs = & $vswhere -latest -products * -property installationPath
if (!$vs) { Write-Host '[ERROR] Visual Studio not found'; exit 1 }
Write-Host "[OK] Visual Studio: $vs"
$kits = Join-Path "${env:ProgramFiles(x86)}" 'Windows Kits\10\Include'
if (!(Test-Path $kits)) { Write-Host '[ERROR] Windows SDK/WDK headers missing'; exit 1 }
$headers = Get-ChildItem $kits -Directory | ForEach-Object { Join-Path $_.FullName 'km\ntddk.h' } | Where-Object { Test-Path $_ }
if (!$headers) { Write-Host '[ERROR] ntddk.h missing: install Windows Driver Kit (WDK) matching Windows SDK'; exit 1 }
$headers | ForEach-Object { Write-Host "[OK] $_" }
$wdf = Get-ChildItem $kits -Recurse -Filter wdf.h -ErrorAction SilentlyContinue | Select-Object -First 1
if (!$wdf) { Write-Host '[ERROR] wdf.h missing: install WDK and Visual Studio driver tools'; exit 1 }
Write-Host "[OK] $($wdf.FullName)"
Write-Host '[OK] Prerequisites detected. Build from a VS Developer PowerShell using MSBuild.'
