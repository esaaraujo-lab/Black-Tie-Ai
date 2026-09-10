# Black Tie AI Producer + Black Tie Live — Windows installer/build
$ErrorActionPreference = 'Stop'
$Root = (Resolve-Path "$PSScriptRoot\..").Path
Write-Host ""
Write-Host "=== BLACK TIE AI PRODUCER / BLACK TIE LIVE ===" -ForegroundColor Cyan
Write-Host "Starting unattended bootstrap and build..." -ForegroundColor Cyan
$py = Get-Command py.exe -ErrorAction SilentlyContinue
if ($py) { & $py.Source "$Root\install.py" @args }
else {
  $python = Get-Command python.exe -ErrorAction SilentlyContinue
  if (-not $python) { throw "Python was not found. The bootstrap requires Python to execute install.py." }
  & $python.Source "$Root\install.py" @args
}
if ($LASTEXITCODE -ne 0) { throw "Black Tie installer failed with exit code $LASTEXITCODE" }
Write-Host ""
Write-Host "INSTALLATION COMPLETE" -ForegroundColor Green
Write-Host "VST3 files: $Root\dist" -ForegroundColor Green
