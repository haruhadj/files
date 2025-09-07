
# Define paths and URLs
$DestDir = "C:\AnyDeskPortable"
$AnyDeskUrl = "https://download.anydesk.com/AnyDesk.exe"
$AnyDeskExe = Join-Path $DestDir "AnyDesk.exe"

# Ensure TLS 1.2 is enabled for HTTPS
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

# Create folder if it doesn't exist
if (-not (Test-Path $DestDir)) {
    try {
        New-Item -ItemType Directory -Path $DestDir -Force | Out-Null
        Write-Host "Created folder: $DestDir"
    } catch {
        Write-Error "Cannot create folder $DestDir. $_"
        Exit 1
    }
}

# Check if AnyDesk.exe already exists
$Redownload = $false
if (-not (Test-Path $AnyDeskExe)) {
    Write-Host "AnyDesk.exe not found, will download..."
    $Redownload = $true
} else {
    # Check if file is corrupt or size is unexpectedly small (< 1 MB)
    $size = (Get-Item $AnyDeskExe).Length
    if ($size -lt 1000000) {
        Write-Host "Existing AnyDesk.exe seems incomplete. Re-downloading..."
        $Redownload = $true
    } else {
        Write-Host "Valid AnyDesk.exe already exists."
    }
}

# Download if needed
if ($Redownload) {
    try {
        Write-Host "Downloading AnyDesk..."
        Invoke-WebRequest -Uri $AnyDeskUrl -OutFile $AnyDeskExe -UseBasicParsing -ErrorAction Stop
        Unblock-File -Path $AnyDeskExe
        Write-Host "Downloaded successfully to $AnyDeskExe"
    } catch {
        Write-Error "Failed to download AnyDesk. $_"
        Exit 1
    }
}

# Skip if AnyDesk is already running
$running = Get-Process -Name "AnyDesk" -ErrorAction SilentlyContinue
if ($running) {
    Write-Host "AnyDesk is already running (PID: $($running.Id)). Skipping launch."
} else {
    try {
        Write-Host "Launching AnyDesk in portable mode..."
        Start-Process -FilePath $AnyDeskExe -ArgumentList "--portable" -WorkingDirectory $DestDir
    } catch {
        Write-Error "Failed to launch AnyDesk. $_"
    }
}
