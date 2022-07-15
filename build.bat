@echo off

IF NOT EXIST build mkdir build
pushd build

call vcvarsall.bat x64

set common_compiler_flags=/TC /Od /MT /Zi /FC /nologo /W4 /WX /DWIN32 /fsanitize=address
REM /clang:-pedantic /clang:-std=c89

set compiler=clang-cl

if %compiler%==clang-cl (
   set linker=lld-link
   set clang_compiler_flags=/clang:-fdiagnostics-format=clang /clang:-fdiagnostics-absolute-paths /clang:-fno-omit-frame-pointer
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

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\os\os.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32_memory.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32.c

%compiler% %common_compiler_flags% %clang_compiler_flags% %all_includes% /c /Fo ..\source\win32\win32_main.c

%linker% /debug memory.obj os_memory.obj os.obj win32_memory.obj win32.obj win32_main.obj /out:win32_main.exe user32.lib /wholearchive:clang_rt.asan-x86_64.lib

popd
