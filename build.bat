@echo off
setlocal EnableDelayedExpansion

IF NOT EXIST build mkdir build
pushd build

set win32_defines=/DWIN32_LEAN_AND_MEAN

set debug_compiler_flags=/Od /MTd /Zo /Z7 /RTC1 /fsanitize=address
set release_compiler_flags=/O2 
set common_compiler_flags=/Oi /TC /FC /GS- /nologo /Wall /WX /D_CRT_SECURE_NO_WARNINGS
set object_flags=/c /Fo

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

set common_source=..\source\common\memory_utils.c ..\source\common\random.c ..\source\test\test.c
set win32_source=..\source\win32\win32_graphics.c ..\source\win32\win32_thread.c ..\source\win32\win32_io.c ..\source\win32\win32_time.c ..\source\win32\win32_memory.c ..\source\win32\win32_window.c ..\source\win32\win32.c
set os_source=..\source\os\os_thread.c ..\source\os\os_log.c ..\source\os\os_io.c ..\source\os\os_time.c ..\source\os\os_memory.c ..\source\os\os_window.c ..\source\os\os_graphics.c ..\source\os\os.c
set all_source=%common_source% %win32_source% %os_source%

set all_object=memory_utils.obj random.obj os_thread.obj os_log.obj os_io.obj os_time.obj os_memory.obj os_window.obj os_graphics.obj os.obj win32_graphics.obj win32_thread.obj win32_io.obj win32_time.obj win32_memory.obj win32_window.obj win32.obj

REM Compile vertex shader to a object file
fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fo d3d11_vertex_shader.o /Vn d3d11_vertex_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl

REM Compile pixel shader to a object file
fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fo d3d11_pixel_shader.o /Vn d3d11_pixel_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl

REM Compile .c files to .obj files (no linking).
%compiler% %common_compiler_flags% %clang_compiler_flags% %object_flags% %win32_defines% %all_include% %all_source%

REM Pack .obj files into a .lib.
%ar% %all_object% /out:os.lib

REM Compile and link the application.
%compiler% %common_compiler_flags% %clang_compiler_flags% %os_include% %common_include% %test_include% ..\source\application.c /link %common_linker_flags% test.obj os.lib /out:application.exe 

REM libvcruntime.lib libucrt.lib

IF NOT EXIST binaries mkdir binaries
pushd binaries

del /F /Q *
move ..\*.obj .
move ..\*.lib .

popd

popd

