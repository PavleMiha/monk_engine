@echo off
setlocal

:: Check if the argument is provided
if "%~1"=="" (
    echo Usage: %0 assets\fs_cubes.sc
    exit /b 1
)

:: Set the input file
set INPUT_FILE=%~1

:: Extract the filename without extension
set FILENAME=%~n1

:: Create the necessary directories if they don't exist
if not exist ".\bin\shaders\dx11\" (
    mkdir ".\bin\shaders\dx11\"
)

:: Run the shader compilation command with dynamic output filename
.\build\win64_vs2022\bin\shadercDebug.exe -f %INPUT_FILE% -o bin\shaders\dx11\%FILENAME%.bin -i bgfx\src\ --platform windows --type fragment -p s_5_0 -O 3

:: End of script
endlocal