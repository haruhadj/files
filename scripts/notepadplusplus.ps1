<#
.SYNOPSIS
 Download the latest Notepad++ portable ZIP into the current directory.

.DESCRIPTION
 Uses GitHub Releases API to find the latest release for notepad-plus-plus and
 chooses the portable .zip asset for the detected OS architecture (x64/x86).
 If a checksum asset (sha256, SHA256, or checksums.txt) is available it will
 download that too and attempt to verify the file.

.USAGE
 Save as Get-NotepadPP-Portable.ps1 and run:
   powershell -NoProfile -ExecutionPolicy Bypass -File .\Get-NotepadPP-Portable.ps1
 Or pipe from the web (only from a trusted source):
   irm https://yourhost/Get-NotepadPP-Portable.ps1 | iex

#>

# --- Basic helper / environment ---
$ErrorActionPreference = 'Stop'
$cwd = (Get-Location).ProviderPath
Write-Host "Saving files to: $cwd"

# Detect architecture preference
$archPreference = if ([Environment]::Is64BitOperatingSystem) { 'x64' } else { 'x86' }
Write-Host "Detected OS architecture preference: $archPreference"

# Query GitHub Releases API for latest Notepad++ release
$ghApi = 'https://api.github.com/repos/notepad-plus-plus/notepad-plus-plus/releases/latest'
# GitHub requires a User-Agent header
$headers = @{ 'User-Agent' = 'PowerShell (Get-NotepadPP-Portable script)' }

try {
    $release = Invoke-RestMethod -Uri $ghApi -Headers $headers -UseBasicParsing -TimeoutSec 30
} catch {
    Write-Error "Failed to query GitHub Releases API: $_"
    throw
}

# Sanity check
if (-not $release) {
    throw "Could not retrieve release information from GitHub."
}

Write-Host "Found release: $($release.tag_name) - $($release.name)"

# Helper: find best portable zip asset
# Prefer asset name with 'portable' and architecture; fallback to any 'portable' zip.
$assets = $release.assets
if (-not $assets -or $assets.Count -eq 0) {
    throw "No assets found in release $($release.tag_name)."
}

# Try to pick a portable zip that includes architecture
$asset = $assets |
    Where-Object { $_.name -match 'portable' -and $_.name -match '\.zip$' -and $_.name -match $archPreference } |
    Select-Object -First 1

# Fallback: any portable zip
if (-not $asset) {
    $asset = $assets | Where-Object { $_.name -match 'portable' -and $_.name -match '\.zip$' } | Select-Object -First 1
}

if (-not $asset) {
    throw "Couldn't find a portable .zip asset in the release assets."
}

$downloadUrl = $asset.browser_download_url
$fileName = $asset.name
$outPath = Join-Path -Path $cwd -ChildPath $fileName

Write-Host "Selected asset: $fileName"
Write-Host "Downloading from: $downloadUrl"

# Download the portable zip
try {
    Invoke-WebRequest -Uri $downloadUrl -OutFile $outPath -Headers $headers -UseBasicParsing -TimeoutSec 120
    Write-Host "Downloaded: $outPath"
} catch {
    Write-Error "Download failed: $_"
    throw
}

# Attempt to find a checksum asset (sha256, SHA256, checksums, .sha256)
$checksumAsset = $assets | Where-Object {
    $_.name -match 'sha256' -or $_.name -match 'SHA256' -or $_.name -match 'checksums' -or $_.name -match '\.sha256$'
} | Select-Object -First 1

if ($checksumAsset) {
    $ckUrl = $checksumAsset.browser_download_url
    $ckName = $checksumAsset.name
    $ckPath = Join-Path -Path $cwd -ChildPath $ckName
    Write-Host "Found checksum asset: $ckName — downloading..."
    try {
        Invoke-WebRequest -Uri $ckUrl -OutFile $ckPath -Headers $headers -UseBasicParsing -TimeoutSec 30
        Write-Host "Downloaded checksum file: $ckPath"
    } catch {
        Write-Warning "Failed to download checksum file: $_"
        $ckPath = $null
    }

    # If checksum file downloaded, try to extract expected hash and validate
    if ($ckPath -and (Test-Path $ckPath)) {
        try {
            $ckText = Get-Content -Path $ckPath -Raw
            # try to find a 64-hex sha256 for the downloaded filename
            $pattern = '([a-fA-F0-9]{64}).*' + [regex]::Escape($fileName)
            $m = [regex]::Match($ckText, $pattern)
            if (-not $m.Success) {
                # fallback: first 64-hex in file
                $m = [regex]::Match($ckText, '([a-fA-F0-9]{64})')
            }
            if ($m.Success) {
                $expected = $m.Groups[1].Value.ToLower()
                Write-Host "Expected SHA256 (from checksum file): $expected"
                $actualBytes = Get-FileHash -Path $outPath -Algorithm SHA256
                $actual = $actualBytes.Hash.ToLower()
                Write-Host "Actual SHA256: $actual"
                if ($actual -eq $expected) {
                    Write-Host "SHA256 verification OK."
                } else {
                    Write-Warning "SHA256 mismatch! The downloaded file may be corrupt or tampered with."
                }
            } else {
                Write-Warning "Could not parse a SHA256 hash from the checksum file."
            }
        } catch {
            Write-Warning "Checksum verification failed: $_"
        }
    }
} else {
    Write-Host "No checksum asset found in release assets. Consider verifying the file manually from the official site."
}

Write-Host "Done. File saved in current directory: $outPath"
