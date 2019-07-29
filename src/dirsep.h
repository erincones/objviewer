#ifndef __DIR_SEP_HPP_
#define __DIR_SEP_HPP_

/** Set the platform-specific directory path separator */
#if defined(_WIN32)
    #define DIR_SEP '\\'
#else
    #define DIR_SEP '/'
#endif

#endif // __DIR_SEP_HPP_