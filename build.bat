@echo off
setlocal EnableDelayedExpansion

REM TODO: !!!! MAKE THIS BUILD SCRIPT SIMPLER AND SHORTER !!!!

IF NOT EXIST build mkdir build
pushd build

set object_folder=objects
IF NOT EXIST %object_folder% mkdir %object_folder%
del /F /Q %object_folder%\*

set win32_defines=/DWIN32_LEAN_AND_MEAN

REM /MTd
REM clang-cl: error: invalid argument '/MTd' not allowed with '-fsanitize=address'
REM clang-cl: note: AddressSanitizer doesn't support linking with debug runtime libraries yet
set debug_compiler_flags=/Od /MTd /Zo /Z7 /RTC1 /fsanitize=address
set release_compiler_flags=/O2 
set common_compiler_flags=/Oi /TC /FC /GS- /nologo /Wall /WX /D_CRT_SECURE_NO_WARNINGS
set object_flags=/c /Fo%object_folder%\


set debug_linker_flags=/debug
rem /wholearchive:clang_rt.asan-x86_64.lib
set release_linker_flags=/fixed /opt:icf /opt:ref libvcruntime.lib libucrt.lib
set common_linker_flags=/incremental:no /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

set debug=yes
set compiler=cl

set env_cache=env_cache.cmd

if exist "!env_cache!" (
   call "!env_cache!"
) else (
   REM TODO: Maybe add logic to find the vcvarsall.bat
   call vcvarsall.bat x64

   echo set INCLUDE=!INCLUDE!> "!env_cache!"
   echo set LIB=!LIB!>> "!env_cache!"
   echo set LIBPATH=!LIBPATH!>> "!env_cache!"
   echo set PATH=!PATH!>> "!env_cache!"
)

if %debug%==yes (
   set common_compiler_flags=%common_compiler_flags% %debug_compiler_flags%
   set common_linker_flags=%common_linker_flags% %debug_linker_flags%
) else (
   set common_compiler_flags=%common_compiler_flags% %release_compiler_flags%
   set object_flags=%object_flags%
   set common_linker_flags=%common_linker_flags% %release_linker_flags%
)

if %compiler%==clang-cl (
   set linker=lld-link
   set ar=llvm-lib
   set clang_compiler_flags=/clang:-std=c99 /clang:-pedantic /clang:-fdiagnostics-format=clang /clang:-fdiagnostics-absolute-paths /clang:-fuse-ld=lld-link /clang:-Wno-format-nonliteral /clang:-Wno-cast-qual /clang:-Wno-bad-function-cast /clang:-Wno-c11-extensions
   if %debug%==yes (
   set clang_compiler_flags=!clang_compiler_flags! /clang:-fno-omit-frame-pointer
   )
) else (
     if %compiler%==cl (
     	set linker=link
	set ar=lib
	set common_compiler_flags=%common_compiler_flags% /wd4200 /wd5045 /wd4711 /wd4710 /wd4201
	REM /wd4200 MSVC does not support Flexible Array Members
	REM /wd5045 Spectre mitigation (?) warning
	REM /wd4711 Function selected for automatic inline expansion
	REM /wd4710 Function not inlined
	REM /wd4201 Nonstandard extension used: nameless struct/union
     )
)

set test_include=/I..\source\test\include\
set common_include=/I..\source\common\include\
set win32_include=/I..\source\win32\include\
set os_include=/I..\source\os\include\
set all_include=%common_include% %win32_include% %os_include% %test_include%

set test_source=..\source\test\*.c
set common_source=..\source\common\*.c
set win32_source=..\source\win32\*.c
set os_source=..\source\os\*.c
set all_source=%common_source% %win32_source% %os_source% %test_source%

set all_object=%object_folder%\*.obj

REM Compile vertex shader to a object file
fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fo d3d11_vertex_shader.o /Vn d3d11_vertex_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl

REM Compile pixel shader to a object file
fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fo d3d11_pixel_shader.o /Vn d3d11_pixel_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl

REM Compile .c files to .obj files (no linking).
%compiler% %common_compiler_flags% %clang_compiler_flags% %object_flags% %win32_defines% %all_include% %all_source%

REM Pack .obj files into a .lib.
%ar% %all_object% /out:os.lib

REM Compile and link the application.
%compiler% %common_compiler_flags% %clang_compiler_flags% %os_include% %common_include% %test_include% ..\source\application.c /link %common_linker_flags% %object_folder%\test.obj os.lib /out:application.exe 

REM libvcruntime.lib libucrt.lib

IF NOT EXIST binaries mkdir binaries
pushd binaries

del /F /Q *
move ..\*.obj .
move ..\*.lib .

popd

popd

