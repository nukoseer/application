@echo off

IF NOT EXIST build mkdir build
pushd build

call vcvarsall.bat x64

set win32_defines=/DWIN32 /DWIN32_LEAN_AND_MEAN

set common_compiler_flags=/TC /Od /Zi /MT /c /Fo /FC /nologo /Wall /WX %win32_defines% /fsanitize=address

set compiler=clang-cl

if %compiler%==clang-cl (
   set linker=lld-link
   set ar=llvm-lib
   set clang_compiler_flags=/clang:-std=c99 /clang:-pedantic /clang:-fdiagnostics-format=clang /clang:-fdiagnostics-absolute-paths /clang:-fno-omit-frame-pointer
) else (
     if %compiler%==cl (
     	set linker=link
	set ar=lib
	set common_compiler_flags=%common_compiler_flags% 
	REM /wd4200 MSVC does not support Flexible Array Members
     )
)

set test_include=/I..\source\test\include\
set common_include=/I..\source\common\include\
set win32_include=/I..\source\win32\include\
set os_include=/I..\source\os\include\

set all_include=%common_include% %win32_include% %os_include%

%compiler% %common_compiler_flags% %clang_compiler_flags% %os_include% %test_include% %common_include% ..\source\application.c ..\source\common\mem.c ..\source\test\test.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_include% ..\source\os\os_memory.c ..\source\os\os_window.c ..\source\os\os_timer.c ..\source\os\os.c ..\source\win32\win32_timer.c ..\source\win32\win32_memory.c ..\source\win32\win32_window.c ..\source\win32\win32.c

%ar% mem.obj os_memory.obj os_window.obj os_timer.obj os.obj win32_timer.obj win32_memory.obj win32_window.obj win32.obj /out:os.lib

%linker% /debug application.obj mem.obj test.obj /out:application.exe os.lib User32.lib Winmm.lib /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /wholearchive:clang_rt.asan-x86_64.lib

IF NOT EXIST binaries mkdir binaries
pushd binaries

del /F /Q *
move ..\*.obj .
move ..\*.lib .

popd

popd
