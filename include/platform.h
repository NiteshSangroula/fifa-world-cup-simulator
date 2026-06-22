#ifndef PLATFORM_H
#define PLATFORM_H

// Define POSIX feature test macro before any includes
#define _POSIX_C_SOURCE 199309L // For nanosleep()

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <time.h>

// Use nanosleep instead of usleep (more modern)
static inline void sleep_ms(int ms)
{
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
#endif

#endif
