rem
rem c9y - concurrency
rem Copyright 2011 Sean Farell
rem
rem This file is part of c9y.
rem
rem c9y is free software: you can redistribute it and/or modify
rem it under the terms of the GNU Lesser General Public License as published by
rem the Free Software Foundation, either version 3 of the License, or
rem (at your option) any later version.
rem
rem c9y is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU Lesser General Public License
rem along with c9y. If not, see <http://www.gnu.org/licenses/>.
rem

set TARGET_DIR=%1
set SIGC_DIR=%2

copy /Y "%SIGC_DIR%\bin\*.dll" %TARGET_DIR%

%TARGET_DIR%\c9y_unit_test.exe
