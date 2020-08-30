#pragma once

#include "macro.h"

# if ON_WINDOWS
#   include <sal.h>                 //_Printf_format_string_
#   define __printflike(fmt, args)  //__printflike
# else
#   define _Printf_format_string_   //_Printf_format_string_
#   include <sys/cdefs.h>           //__printflike
# endif

//if the link order of custom "printf" is higher,
//it will replace the function with same name from "stdio".
int printf(_Printf_format_string_ const char *format, ...) __printflike(1, 2);
