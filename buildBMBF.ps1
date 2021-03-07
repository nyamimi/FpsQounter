# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libfps-qounter.so", "./libs/arm64-v8a/libbeatsaber-hook_1_1_4.so", "./bmbfmod.json" -DestinationPath "./fps-qounter_v1.0.0.zip" -Update
}
