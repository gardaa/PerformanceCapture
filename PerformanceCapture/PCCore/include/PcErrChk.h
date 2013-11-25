#ifndef PCERRCHK_H
#define PCERRCHK_H

#include <iostream>
#include <cstdlib>

/// \ingroup PCCORE
/// \brief Shortcut for error checking.
///
/// Checks whether a return value equals it's expected value and
/// prints a message on the standard output upon failure.
/// \param err  the value being checked
/// \param tar  the expected value
/// \param msg  the error message to be printed
#define ERR_CHK(err,tar,msg) \
    do { \
        if ( err != tar ) { \
            std::cout << msg << " : " << err << std::endl; \
            /*std::exit (err);*/ \
        } \
    } while (false)

#endif // PCERRCHK_H
