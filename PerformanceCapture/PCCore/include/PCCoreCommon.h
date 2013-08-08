#ifndef PCCORECOMMON_H
#define PCCORECOMMON_H

#define PCC_OBJ_FREE(ptr)\
    do { \
        if ( ptr ) { \
            delete ptr; \
            ptr = 0x0; \
        } \
    } while ( false )

#define PCC_ARR_FREE(ptr)\
    do { \
        if ( ptr ) { \
            delete[] ptr; \
            ptr = 0x0; \
        } \
    } while ( false )

#define PCC_MEM_FREE(ptr)\
    do { \
        if ( ptr ) { \
            free ( ptr ); \
            ptr = 0x0; \
        } \
    } while ( false )

#endif // PCCORECOMMON_H