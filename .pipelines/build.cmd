rem Called by the release pipeline to do the actual builds.

SETLOCAL
SET DEPS_DIR=C:\arr.arrt.dependencies
rem SET Qt5_DIR=%DEPS_DIR%\Qt\5.13.1\msvc2017_64

rem Qt6_DIR only has to exist, it isn't used
rem SET Qt6_DIR=%DEPS_DIR%\Qt

REM Set developer environment for VS 2019
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64 -winsdk=10.0.16299.0
echo on
pushd C:\source

REM this is read by the CMakeLists.txt file to skip nuget restore
SET NUGET_RESTORE=false

REM SET ARRT_VERSION=v%CDP_PACKAGE_VERSION_SEMANTIC%

set VCPKG_PATH=C:\arr.arrt.dependencies\vcpkg

cmake --version
cmake -G "Visual Studio 16 2019" -A x64 "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake" "-DUSE_NEW_AZURE_STORAGE_SDK:BOOL=ON"
if %errorlevel% neq 0 (
    echo Failed to generate solution %errorlevel%
    goto errorExit
)

msbuild /m /p:Configuration=Debug /p:Platform=x64 Arrt.sln

if %errorlevel% neq 0 (
    echo Failed to build debug solution. %errorlevel%
    goto errorExit
)

msbuild /m /p:Configuration=Release /p:Platform=x64 Arrt.sln

if %errorlevel% neq 0 (
    echo Failed to build release solution. %errorlevel%
    goto errorExit
)

popd
exit /B 0

:errorExit
popd
exit /B 1

