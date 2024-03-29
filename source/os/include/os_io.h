#ifndef H_OS_IO_H

#define OS_IO_MAX_OUTPUT_LENGTH 512
#define OS_IO_FILE_MAX_PATH_LENGTH 256

typedef uptr OSIOFile;

typedef struct OSIOFileContent
{
    u8* data;
    memory_size size;
} OSIOFileContent;

typedef struct OSIOFileFound
{
    char file_names[64][OS_IO_FILE_MAX_PATH_LENGTH];
    u32 file_count;
} OSIOFileFound;

typedef struct OSIOFileTime
{
    u64 create_time;
    u64 last_access_time;
    u64 last_write_time;
} OSIOFileTime;

typedef enum OSIOFileAccessMode
{
    OS_IO_FILE_ACCESS_MODE_NULL,
    OS_IO_FILE_ACCESS_MODE_READ,
    OS_IO_FILE_ACCESS_MODE_WRITE,
} OSIOFileAccessMode;

typedef enum OSIOFilePointerOffset
{
    OS_IO_FILE_POINTER_OFFSET_NULL,
    OS_IO_FILE_POINTER_OFFSET_START,
    OS_IO_FILE_POINTER_OFFSET_CURRENT,
    OS_IO_FILE_POINTER_OFFSET_END,
} OSIOFilePointerOffset;

struct MemoryArena;

memory_size os_io_console_write(OSIOFile file, const char* fmt, ...);
OSIOFile os_io_console_init(void);
OSIOFile os_io_file_create(const char* file_name, i32 access_mode);
OSIOFile os_io_file_open(const char* file_name, i32 access_mode);
b32 os_io_file_close(OSIOFile file);
b32 os_io_file_delete(const char* file_name);
memory_size os_io_file_write(OSIOFile file, const char* buffer, memory_size size);
memory_size os_io_file_read(OSIOFile file, char* buffer);
memory_size os_io_file_read_by_size(OSIOFile file, char* buffer, memory_size size);
OSIOFileContent os_io_file_read_by_name(struct MemoryArena* arena, const char* file_name);
memory_size os_io_file_size(OSIOFile file);
void os_io_file_find(OSIOFileFound* file_found, const char* file_name);
u32 os_io_file_pointer_move(OSIOFile file, i32 distance, OSIOFilePointerOffset offset);
u32 os_io_file_pointer_reset(OSIOFile file);
u32 os_io_file_pointer_get(OSIOFile file);
b32 os_io_file_get_time(OSIOFile file, OSIOFileTime* file_time);

#define H_OS_IO_H
#endif
