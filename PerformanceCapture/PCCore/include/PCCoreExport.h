#ifndef PCCOREEXPORT_H
#define PCCOREEXPORT_H

#if defined(WIN32)
    #ifdef PCCORE_LIBRARY
        #define PCCORE_EXPORT __declspec(dllexport)
    #else
        #define PCCORE_EXPORT __declspec(dllimport)
    #endif
#else
    #define PCCORE_EXPORT
#endif

#endif // PCCOREEXPORT_H
