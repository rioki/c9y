
set CONFIG=%1

xcopy /Y c9y\%CONFIG%\pikazo.dll %C9Y_DIR%\bin 
xcopy /Y c9y\%CONFIG%\pikazo.dll %C9Y_DIR%\lib
xcopy /Y c9y\*.h %C9Y_DIR%\include\c9y
