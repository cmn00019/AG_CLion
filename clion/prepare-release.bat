@echo off

:: Move to the folder where this script is located
cd /d %~dp0

set VCPKG_DIR=C:/vcpkg/
set TRIPLET=x64-windows
set OUTPUT_DIR=cmake-build-release/Release/

set DLL_LIST="glew32.dll" "glfw3.dll" "assimp-vc143-mt.dll" "zlib1.dll" "poly2tri.dll" "minizip.dll" "pugixml.dll"

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

echo Copying selected .dll files from vcpkg to %OUTPUT_DIR%...
for %%D in (%DLL_LIST%) do (
    if exist "%VCPKG_DIR%\installed\%TRIPLET%\bin\%%~D" (
        echo Copying %%~D to %OUTPUT_DIR%
        copy "%VCPKG_DIR%\installed\%TRIPLET%\bin\%%~D" "%OUTPUT_DIR%" >nul
    ) else (
        echo WARNING: %%~D not found in %VCPKG_DIR%\installed\%TRIPLET%\bin
    )
)

echo Done. Selected DLLs have been copied to %OUTPUT_DIR%.

echo Copying Assets folder to %OUTPUT_DIR%...
xcopy /E /I /H /Y "..\vs\Source\Assets" "%OUTPUT_DIR%\Assets"

pause
