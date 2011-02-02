
set TARGET_DIR=%1
set SIGC_DIR=%2

copy /Y "%SIGC_DIR%\bin\*.dll" %TARGET_DIR%

%TARGET_DIR%\c9y_unit_test.exe
