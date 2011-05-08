
set CONFIG=%1

xcopy /Y ..\c9y\%CONFIG%\*.dll %CONFIG%
xcopy /Y %SIGC_DIR%\bin\*.dll %CONFIG%

%CONFIG%\c9y_test.exe
