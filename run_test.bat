
set CONFIG=%1

xcopy /Y %SIGC_DIR%\bin\*.dll %CONFIG%

%CONFIG%\c9y_test.exe
