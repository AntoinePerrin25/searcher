@echo off
echo Building CSV Search GUI application...

REM Check if raylib exists, download if not
if not exist lib\raylib (
    echo Raylib not found, downloading and building...
    nob.exe -dl-raylib
)

REM Build the GUI application
nob.exe -gui

REM Check if build was successful
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo.
    echo Run the GUI with: "gui_search.exe [csv_file] [has-header|no-header] [separator]"
    echo Example: "gui_search.exe testh.csv has-header ,"
)