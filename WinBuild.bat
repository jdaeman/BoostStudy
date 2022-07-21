@echo off

if "%1"=="" (
	echo [Error] Need to boost root
	exit /b 1
)

CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

SET BOOST_ROOT=%1
SET BOOST_LIBRARYDIR=%BOOST_ROOT%\lib64-msvc-14.1

MKDIR build
CD build

ECHO calling cmake ....
cmake .. ^
-G "Visual Studio 15 2017" ^
-DBOOST_ROOT=%BOOST_ROOT% ^
-DBoost_USE_STATIC_LIBS=ON ^
-DCMAKE_GENERATOR_PLATFORM=x64

CD ..
