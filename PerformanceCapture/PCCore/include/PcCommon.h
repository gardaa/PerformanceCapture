#ifndef PCCOMMON_H
#define PCCOMMON_H

#ifndef PCC_USER_DEFINED_TYPES
#define QUEUE(T)        std::queue< T >
#define STRMAP(T)       std::map< std::string, T >
#define STRUMAP(T)      std::unordered_map< std::string, T >
#define VEC(T)          std::vector< T >
#endif
#define VECOFVECS(T)    VEC(VEC(T))

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

#endif // PCCOMMON_H