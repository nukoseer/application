@echo off

IF NOT EXIST build mkdir build
pushd build

call vcvarsall.bat x64

set win32_defines=/DWIN32 /DWIN32_LEAN_AND_MEAN

set common_compiler_flags=/TC /Od /MT /Zi /FC /nologo /Wall %win32_defines% /fsanitize=address

REM /clang:-pedantic /clang:-std=c89

set compiler=clang-cl

if %compiler%==clang-cl (
   set linker=lld-link
   set clang_compiler_flags=/clang:-std=c99 /clang:-pedantic /clang:-fdiagnostics-format=clang /clang:-fdiagnostics-absolute-paths /clang:-fno-omit-frame-pointer
) else (
     if %compiler%==cl (
     	set linker=link
     )
)

set common_includes=/I..\source\common\includes\
set win32_includes=/I..\source\win32\includes\
set os_includes=/I..\source\os\includes\

set all_includes=%common_includes% %win32_includes% %os_includes%

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\common\memory.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\os\os_memory.c
%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\os\os_window.c
%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\os\os_timer.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32_timer.c
%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32_memory.c
%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32.c
%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32_main.c

%linker% /debug memory.obj os_memory.obj os_window.obj os_timer.obj win32_timer.obj win32_memory.obj win32.obj win32_main.obj /out:win32_main.exe user32.lib winmm.lib /wholearchive:clang_rt.asan-x86_64.lib

popd
