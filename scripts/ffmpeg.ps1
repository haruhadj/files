# Download-FFmpeg.ps1
# Downloads and extracts the latest FFmpeg portable build for Windows (64-bit)
# Works both when saved as a file OR when run via: irm "<url>" | iex

# Determine the current folder (works with irm/iex and direct run)
$CurrentPath = (Get-Location).Path

# Define paths
$ffmpegUrl = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip"
$zipFile = Join-Path $CurrentPath "ffmpeg.zip"
$extractPath = Join-Path $CurrentPath "ffmpeg"

Write-Host "📦 Downloading FFmpeg portable build..."
Invoke-WebRequest -Uri $ffmpegUrl -OutFile $zipFile -UseBasicParsing

Write-Host "🧩 Extracting FFmpeg..."
Expand-Archive -Path $zipFile -DestinationPath $extractPath -Force

# Get extracted version folder
$innerFolder = Get-ChildItem -Path $extractPath | Where-Object { $_.PSIsContainer } | Select-Object -First 1
$binPath = Join-Path $innerFolder.FullName "bin"

# Copy portable binaries to current folder
Copy-Item -Path (Join-Path $binPath "ffmpeg.exe") -Destination (Join-Path $CurrentPath "ffmpeg.exe") -Force
Copy-Item -Path (Join-Path $binPath "ffprobe.exe") -Destination (Join-Path $CurrentPath "ffprobe.exe") -Force

Write-Host "`n✅ FFmpeg is ready!"
Write-Host "Run './ffmpeg.exe' or './ffprobe.exe' directly from this folder."
Write-Host "You can move this folder anywhere — it’s fully portable."

# Clean up
Remove-Item $zipFile -Force
