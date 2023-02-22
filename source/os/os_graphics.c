#include "types.h"
#include "utils.h"

#include "os_window.h"
#include "os_graphics.h"

// typedef struct OSGraphics
// {
    
// } OSGraphics;

#ifdef _WIN32
#include "win32_graphics.h"

// static OSGraphics os_graphics =
// {
    
// };

#else
#error _WIN32 must be defined.
#endif

