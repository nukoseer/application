#ifndef H_WIN32_IO_H

struct OSDateTime;
struct MemoryArena;
struct OSIOFileContent;
struct OSIOFileFound;
struct OSIOFileTime;

u32  win32_io_console_write(const char* str, u32 length);
uptr win32_io_file_create(const char* file_name, i32 access_mode);
uptr win32_io_file_open(const char* file_name, i32 access_mode);
b32  win32_io_file_close(uptr file);
b32  win32_io_file_delete(const char* file_name);
memory_size win32_io_file_write(uptr file, const char* buffer, memory_size size);
memory_size win32_io_file_read_by_size(uptr file, char* buffer, memory_size size);
struct OSIOFileContent win32_io_file_read_by_name(struct MemoryArena* arena, const char* file_name);
memory_size win32_io_file_size(uptr file);
void win32_io_file_find(struct OSIOFileFound* file_found, const char* file_name);
u32  win32_io_file_pointer_move(uptr file, i32 distance, i32 offset);
u32  win32_io_file_pointer_reset(uptr file);
u32  win32_io_file_pointer_get(uptr file);
b32 win32_io_file_get_time(uptr file, struct OSIOFileTime* file_time);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
