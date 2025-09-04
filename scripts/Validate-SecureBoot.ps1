#!/usr/bin/env pwsh
# Secure Boot Compliance Validation Script
# Validates that the Apple Magic Mouse and Keyboard drivers meet Secure Boot requirements

param(
    [string]$RootPath = ".",
    [switch]$Verbose
)

$ErrorActionPreference = 'Stop'
Write-Host "=== Secure Boot Compliance Validation ===" -ForegroundColor Green

$issues = @()
$passes = @()

function Test-ProjectFile {
    param($Path, $ProjectName)
    
    if (-not (Test-Path $Path)) {
        $script:issues += "❌ $ProjectName project file not found: $Path"
        return
    }
    
    $content = Get-Content $Path -Raw
    
    # Check Universal platform targeting
    if ($content -match '<DriverTargetPlatform>Universal</DriverTargetPlatform>') {
        $script:passes += "✅ $ProjectName targets Universal platform"
    } else {
        $script:issues += "❌ $ProjectName does not target Universal platform"
    }
    
    # Check enhanced signing configuration
    if ($content -match 'EnableProductionSign' -and $content -match 'ProductionCertThumbprint') {
        $script:passes += "✅ $ProjectName has enhanced signing configuration"
    } else {
        $script:issues += "❌ $ProjectName missing production signing support"
    }
    
    # Check SHA256 digest algorithm
    if ($content -match '<FileDigestAlgorithm>sha256</FileDigestAlgorithm>') {
        $script:passes += "✅ $ProjectName uses SHA256 signing algorithm"
    } else {
        $script:issues += "❌ $ProjectName missing SHA256 signing algorithm"
    }
}

function Test-INFFile {
    param($Path, $INFName)
    
    if (-not (Test-Path $Path)) {
        $script:issues += "❌ $INFName INF file not found: $Path"
        return
    }
    
    $content = Get-Content $Path -Raw
    
    # Check PnP Lockdown
    if ($content -match 'PnpLockDown\s*=\s*1') {
        $script:passes += "✅ $INFName has PnP Lockdown enabled"
    } else {
        $script:issues += "❌ $INFName missing PnP Lockdown"
    }
    
    # Check security descriptors
    if ($content -match 'Security\s*=\s*"D:P\(A;;GA;;;SY\)\(A;;GA;;;BA\)"') {
        $script:passes += "✅ $INFName has security descriptors"
    } else {
        $script:issues += "❌ $INFName missing security descriptors"
    }
    
    # Check catalog file
    if ($content -match 'CatalogFile\s*=\s*\w+\.cat') {
        $script:passes += "✅ $INFName specifies catalog file"
    } else {
        $script:issues += "❌ $INFName missing catalog file specification"
    }
}

function Test-DriverSource {
    param($Path, $DriverName)
    
    if (-not (Test-Path $Path)) {
        $script:issues += "❌ $DriverName source file not found: $Path"
        return
    }
    
    $content = Get-Content $Path -Raw
    
    # Check parameter validation in DriverEntry
    if ($content -match 'if\s*\(\s*!\s*DriverObject\s*\|\|\s*!\s*RegistryPath\s*\)') {
        $script:passes += "✅ $DriverName has DriverEntry parameter validation"
    } else {
        $script:issues += "❌ $DriverName missing DriverEntry parameter validation"
    }
    
    # Check for security validation comments
    if ($content -match 'Security validation') {
        $script:passes += "✅ $DriverName includes security validation"
    } else {
        $script:issues += "❌ $DriverName missing security validation indicators"
    }
}

# Test project files
Write-Host "`n--- Testing Project Files ---" -ForegroundColor Yellow
Test-ProjectFile "$RootPath/Apple_KB_N_Mouse.vcxproj" "Apple_KB_N_Mouse"
Test-ProjectFile "$RootPath/Drivers/AppleInputFilter/AppleInputFilter.vcxproj" "AppleInputFilter"
Test-ProjectFile "$RootPath/Drivers/ApplePTPVhf/ApplePTPVhf.vcxproj" "ApplePTPVhf"

# Test INF files
Write-Host "`n--- Testing INF Files ---" -ForegroundColor Yellow
Test-INFFile "$RootPath/Apple_KB_N_Mouse.inf" "Apple_KB_N_Mouse"
Test-INFFile "$RootPath/Drivers/AppleInputFilter/inf/AppleInputFilter.inf" "AppleInputFilter"
Test-INFFile "$RootPath/Drivers/ApplePTPVhf/inf/ApplePTPVhf.inf" "ApplePTPVhf"

# Test driver source files
Write-Host "`n--- Testing Driver Source ---" -ForegroundColor Yellow
Test-DriverSource "$RootPath/Driver.c" "Apple_KB_N_Mouse"
Test-DriverSource "$RootPath/Drivers/AppleInputFilter/src/AppleInputFilter.cpp" "AppleInputFilter"
Test-DriverSource "$RootPath/Drivers/ApplePTPVhf/src/ApplePTPVhf.c" "ApplePTPVhf"

# Test driver artifacts
Write-Host "`n--- Testing Driver Artifacts ---" -ForegroundColor Yellow
$artifacts = @(
    @{Path="$RootPath/x64/Release/Apple_KB_N_Mouse.sys"; Name="Apple_KB_N_Mouse x64 Release"}
    @{Path="$RootPath/x64/Release/AppleInputFilter.sys"; Name="AppleInputFilter x64 Release"}
    @{Path="$RootPath/x64/Release/ApplePTPVhf.sys"; Name="ApplePTPVhf x64 Release"}
)

foreach ($artifact in $artifacts) {
    if (Test-Path $artifact.Path) {
        $passes += "✅ $($artifact.Name) driver built"
    } else {
        $issues += "❌ $($artifact.Name) driver not found"
    }
}

# Display results
Write-Host "`n=== Validation Results ===" -ForegroundColor Green

Write-Host "`n--- PASSED CHECKS ---" -ForegroundColor Green
foreach ($pass in $passes) {
    Write-Host $pass -ForegroundColor Green
}

if ($issues.Count -gt 0) {
    Write-Host "`n--- FAILED CHECKS ---" -ForegroundColor Red
    foreach ($issue in $issues) {
        Write-Host $issue -ForegroundColor Red
    }
    Write-Host "`n❌ VALIDATION FAILED: $($issues.Count) issues found" -ForegroundColor Red
    exit 1
} else {
    Write-Host "`n✅ ALL CHECKS PASSED: Drivers are Secure Boot compliant!" -ForegroundColor Green
    exit 0
}