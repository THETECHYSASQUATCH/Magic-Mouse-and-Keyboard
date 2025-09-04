# SecureBootDriverSigning.ps1
# This script generates a self-signed certificate, signs a driver,
# and adds the certificate to the Secure Boot database.

# Function to create a self-signed certificate
function Create-SelfSignedCertificate {
    param(
        [string]$certName = "SecureBootCert"
    )
    $cert = New-SelfSignedCertificate -CertStoreLocation Cert:\LocalMachine\My -DnsName $certName
    return $cert
}

# Function to sign a driver
function Sign-Driver {
    param(
        [string]$driverPath,
        [string]$certThumbprint
    )
    SignTool sign /f Cert:\LocalMachine\My\$certThumbprint /pfxpassword "YourPfxPassword" $driverPath
}

# Function to add certificate to Secure Boot database
function Add-CertificateToSecureBoot {
    param(
        [string]$certThumbprint
    )
    bcdedit /set {bootmgr} path \EFI\Microsoft\Boot\SecureBoot\SecureBootCerts\certs.bin
    bcdedit /set {bootmgr} path \EFI\Microsoft\Boot\SecureBoot\SecureBootCerts\$certThumbprint
}

# Main script execution
$cert = Create-SelfSignedCertificate "SecureBootCert"
$driverPath = "C:\Path\To\Your\Driver.sys" # Update this path
Sign-Driver -driverPath $driverPath -certThumbprint $cert.Thumbprint
Add-CertificateToSecureBoot -certThumbprint $cert.Thumbprint

Write-Host "Driver signed and certificate added to Secure Boot."