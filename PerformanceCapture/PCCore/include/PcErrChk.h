#ifndef ERRCHK_H
#define ERRCHK_H

#include <iostream>
#include <cstdlib>

#define ERR_CHK(err,tar,msg) \
    do { \
        if ( err != tar ) { \
            std::cout << msg << " : " << err << std::endl; \
            /*std::exit (err);*/ \
        } \
    } while (false)

#endif // ERRCHK_H
