$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$thirdParty = Join-Path $projectRoot 'third_party'
$archive = Join-Path $thirdParty 'SDL3-devel.zip'
$sdlConfig = Join-Path $thirdParty 'SDL3-3.2.30/cmake/SDL3Config.cmake'
if (Test-Path -LiteralPath $sdlConfig) {
    Write-Host 'SDL3 3.2.30 is already available.'
    exit 0
}
New-Item -ItemType Directory -Force -Path $thirdParty | Out-Null
Invoke-WebRequest -Uri 'https://github.com/libsdl-org/SDL/releases/download/release-3.2.30/SDL3-devel-3.2.30-VC.zip' -OutFile $archive
$expectedHash = '3A93C2182DDAF64692A8907928E6A54467F0F7C1CADDF3019CFA69FBF1DFE3F2'
if ((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $expectedHash) {
    throw 'SDL3 archive SHA-256 mismatch; archive was not extracted.'
}
Expand-Archive -LiteralPath $archive -DestinationPath $thirdParty -Force
Write-Host 'SDL3 3.2.30 downloaded and verified.'
