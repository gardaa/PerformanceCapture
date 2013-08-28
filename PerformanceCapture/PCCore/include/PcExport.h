#ifndef PCEXPORT_H
#define PCEXPORT_H

#if defined(WIN32)
    #ifdef PCCORE_LIBRARY
        #define PCCORE_EXPORT __declspec(dllexport)
    #else
        #define PCCORE_EXPORT __declspec(dllimport)
    #endif
#else
    #define PCCORE_EXPORT
#endif

#endif // PCEXPORT_H
