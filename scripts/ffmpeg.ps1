# Download-FFmpeg.ps1
# Downloads and extracts the latest portable FFmpeg for Windows (64-bit static build)

# Set download variables
$ffmpegUrl = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip"
$zipFile = "$PSScriptRoot\ffmpeg.zip"
$extractPath = "$PSScriptRoot\ffmpeg"

Write-Host "Downloading FFmpeg portable build..."
Invoke-WebRequest -Uri $ffmpegUrl -OutFile $zipFile -UseBasicParsing

Write-Host "Extracting FFmpeg..."
Expand-Archive -Path $zipFile -DestinationPath $extractPath -Force

# Get extracted folder (it usually contains version in its name)
$innerFolder = Get-ChildItem -Path $extractPath | Where-Object { $_.PSIsContainer } | Select-Object -First 1
$binPath = Join-Path $innerFolder.FullName "bin"

# Create a portable shortcut in the same directory
Copy-Item -Path (Join-Path $binPath "ffmpeg.exe") -Destination "$PSScriptRoot\ffmpeg.exe" -Force
Copy-Item -Path (Join-Path $binPath "ffprobe.exe") -Destination "$PSScriptRoot\ffprobe.exe" -Force

Write-Host "`n✅ FFmpeg downloaded and ready to use!"
Write-Host "Run './ffmpeg.exe' or './ffprobe.exe' from this folder."
Write-Host "You can move this folder anywhere — it’s fully portable."

# Clean up ZIP
Remove-Item $zipFile -Force
