# Post-build gate: prove the binary can run on CPUs without AVX/AVX2.
# Fails the build if (1) first-party code uses 256/512-bit intrinsics, or
# (2) the recorded compiler commands contain AVX arch flags.
# Runs under Windows PowerShell 5.1 (powershell.exe), no extra modules.
param(
    [Parameter(Mandatory = $true)][string]$ProjectDir,
    [Parameter(Mandatory = $true)][string]$IntDir
)

$failed = $false

# 1. Intrinsics in first-party code. project\external is excluded on purpose:
#    xorstr.hpp selects its SSE2 path via JM_XORSTR_DISABLE_AVX_INTRINSICS.
$srcRoots = @('project\core', 'project\utilities', 'project\pch', 'project\protection') |
    ForEach-Object { Join-Path $ProjectDir $_ }
$srcHits = Get-ChildItem -Path $srcRoots -Recurse -Include '*.cpp', '*.hpp', '*.h' -ErrorAction SilentlyContinue |
    Select-String -Pattern '_mm256_|_mm512_' -CaseSensitive
if ($srcHits) {
    Write-Output 'ERROR: AVX intrinsics in first-party code (non-AVX2 CPUs would crash):'
    $srcHits | ForEach-Object { Write-Output ("  {0}:{1}: {2}" -f $_.Path, $_.LineNumber, $_.Line.Trim()) }
    $failed = $true
}

# 2. Arch flags in the actual compile commands of this build.
$tlog = Join-Path $IntDir 'aetheris.tlog\clang-cl.command.1.tlog'
if (Test-Path -LiteralPath $tlog) {
    $flagHits = Select-String -LiteralPath $tlog -Pattern '-mavx', '-march=' -CaseSensitive -SimpleMatch
    if ($flagHits) {
        Write-Output 'ERROR: AVX arch flags in compile commands:'
        $flagHits | Select-Object -First 3 | ForEach-Object { Write-Output ("  line {0}" -f $_.LineNumber) }
        $failed = $true
    }
}
else {
    Write-Output ("WARNING: build tracking log not found, skipping flag check: {0}" -f $tlog)
}

if ($failed) { exit 1 }
Write-Output 'check_no_avx: PASS (no AVX intrinsics, no AVX arch flags)'
exit 0
