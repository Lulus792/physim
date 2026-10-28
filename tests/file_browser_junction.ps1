param(
    [Parameter(Mandatory=$true)][string]$Root,
    [Parameter(Mandatory=$true)][string]$Target,
    [Parameter(Mandatory=$true)][string]$Link
)
$ErrorActionPreference = 'Stop'
$rootPath = [IO.Path]::GetFullPath($Root).TrimEnd('\')
$targetPath = [IO.Path]::GetFullPath($Target)
$linkPath = [IO.Path]::GetFullPath($Link)
foreach ($candidate in @($targetPath, $linkPath)) {
    if (-not $candidate.StartsWith($rootPath + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Junction fixture escapes its explicit test root'
    }
}
if (-not (Get-Item -LiteralPath $rootPath).PSIsContainer -or
    -not (Get-Item -LiteralPath $targetPath).PSIsContainer) { throw 'Fixture directories missing' }
if ($null -ne (Get-Item -LiteralPath $linkPath -Force -ErrorAction SilentlyContinue)) { throw 'Link already exists' }
# NTFS junction creation does not require enabling Developer Mode or changing
# the machine's symbolic-link policy. The target is another owned test folder.
New-Item -ItemType Junction -Path $linkPath -Target $targetPath | Out-Null
if (-not ((Get-Item -LiteralPath $linkPath -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
    throw 'Native junction was not created'
}
