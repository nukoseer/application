@echo off
setlocal EnableDelayedExpansion

IF NOT EXIST build mkdir build
pushd build

set win32_defines=/DWIN32 /DWIN32_LEAN_AND_MEAN

set debug_compiler_flags=/Od /Zi /RTC1 /fsanitize=address
set release_compiler_flags=/O2
set common_compiler_flags=/TC /MT /FC /nologo /Wall /WX /D_CRT_SECURE_NO_WARNINGS
set object_flags=/c /Fo

set debug_linker_flags=/debug
rem /wholearchive:clang_rt.asan-x86_64.lib
set release_linker_flags=/fixed /opt:icf /opt:ref
set common_linker_flags=/incremental:no /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

set debug=yes
set compiler=clang-cl

set env_cache=env_cache.cmd

if %compiler%==cl if exist "!env_cache!" (
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
   set object_flags=%object_flags% /GS-
   set common_linker_flags=%common_linker_flags% %release_linker_flags%
)

if %compiler%==clang-cl (
   set linker=lld-link
   set ar=llvm-lib
   set clang_compiler_flags=/clang:-std=c99 /clang:-pedantic /clang:-fdiagnostics-format=clang /clang:-fdiagnostics-absolute-paths /clang:-fno-omit-frame-pointer /clang:-fuse-ld=lld-link
) else (
     if %compiler%==cl (
     	set linker=link
	set ar=lib
	set common_compiler_flags=%common_compiler_flags% /wd4200 /wd5045 /wd4711 /wd4710
	REM /wd4200 MSVC does not support Flexible Array Members
	REM /wd5045 Spectre mitigation (?) warning
	REM /wd4711 Function selected for automatic inline expansion
	Rem /wd4710 Function not inlined
     )
)

set test_include=/I..\source\test\include\
set common_include=/I..\source\common\include\
set win32_include=/I..\source\win32\include\
set os_include=/I..\source\os\include\
set all_include=%common_include% %win32_include% %os_include%

%compiler% %common_compiler_flags% %clang_compiler_flags% %object_flags% %os_include% %test_include% %common_include% ..\source\common\mem.c ..\source\test\test.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %object_flags% %win32_defines% %all_include% ..\source\os\os_memory.c ..\source\os\os_window.c ..\source\os\os_timer.c ..\source\os\os.c ..\source\win32\win32_timer.c ..\source\win32\win32_memory.c ..\source\win32\win32_window.c ..\source\win32\win32.c

%ar% mem.obj os_memory.obj os_window.obj os_timer.obj os.obj win32_timer.obj win32_memory.obj win32_window.obj win32.obj /out:os.lib

%compiler% %common_compiler_flags% %clang_compiler_flags% %os_include% %common_include% %test_include% ..\source\application.c /link %common_linker_flags% /out:application.exe mem.obj test.obj os.lib Kernel32.lib User32.lib Winmm.lib Synchronization.lib

REM libvcruntime.lib libucrt.lib

IF NOT EXIST binaries mkdir binaries
pushd binaries

del /F /Q *
move ..\*.obj .
move ..\*.lib .

popd

popd
