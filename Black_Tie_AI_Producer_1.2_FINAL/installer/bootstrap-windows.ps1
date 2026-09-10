# Black Tie AI Producer + Black Tie Live — Windows unattended bootstrap/build
$ErrorActionPreference = 'Stop'
$Root = (Resolve-Path "$PSScriptRoot\..").Path
function Ensure-Command($Name, $WingetId) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        if (-not (Get-Command winget -ErrorAction SilentlyContinue)) { throw "winget is required to install $Name automatically." }
        winget install --id $WingetId -e --silent --accept-source-agreements --accept-package-agreements
        $env:PATH = [Environment]::GetEnvironmentVariable('PATH','Machine') + ';' + [Environment]::GetEnvironmentVariable('PATH','User')
    }
}
Ensure-Command git Git.Git
Ensure-Command cmake Kitware.CMake
Ensure-Command python Python.Python.3.13
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue) -and -not (Get-Command vswhere.exe -ErrorAction SilentlyContinue)) {
    winget install --id Microsoft.VisualStudio.2022.BuildTools -e --silent --accept-source-agreements --accept-package-agreements --override '--wait --passive --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended'
}
$Juce = "$Root\third_party\JUCE"
New-Item -ItemType Directory -Force "$Root\third_party" | Out-Null
if (-not (Test-Path "$Juce\CMakeLists.txt")) { git clone --depth 1 --branch 9.0.1 https://github.com/juce-framework/JUCE.git $Juce }
& python "$Root\scripts\install_all.py"
Write-Host "Done. VST3 artifacts are in $Root\dist"
