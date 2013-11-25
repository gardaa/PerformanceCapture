#ifndef PCCOMMON_H
#define PCCOMMON_H

/// \defgroup   USERTYPES User-defined types.
///             
/// \brief  A set of macros that define user-types.
///
/// By redefining macros whithin this group, users can tell the PCCore Library to 
/// use customized Vectors, Queues, Ordered string maps and Unordered string maps.
/// \{
#ifndef PCC_USER_DEFINED_TYPES
/// \def    QUEUE(T) std::queue< T >
///
/// \brief  A macro that defines the queue type.
///         Defaults to std::queue< T >.
///
/// \param  T   The type contained by the queue.
#define QUEUE(T)        std::queue< T >

/// \def    STRMAP(T) std::map< std::string, T >
///
/// \brief  A macro that defines a binary tree map whose keys are strings.
///         Defaults to std::map< std::string, T >
///
/// \param  T   The type of the values contained in the map.
#define STRMAP(T)       std::map< std::string, T >

/// \def    STRUMAP(T) std::unordered_map< std::string, T >
///
/// \brief  Defines an unordered hash map with std::string keys.
///         std::unordered_map< std::string, T >
///
/// \param  T   The void to process.
#define STRUMAP(T)      std::unordered_map< std::string, T >

/// \def    VEC(T) std::vector< T >
///
/// \brief  Defines the Vector type.
///         Defaults to std::vector< T >
///
/// \param  T   The type contained in the vector.
#define VEC(T)          std::vector< T >

/// \def    VECOFVECS(T) VEC(VEC(T))
///
/// \brief  Defines the Vector of Vectors type.
///         Defaults to std::vector< std::vector< T > >
///
/// \param  T   The type contained in the vectors.
#define VECOFVECS(T)    VEC(VEC(T))
#endif
/// \}

/// \defgroup MEM_MGMT Memory management macros.
/// 
/// \brief  Defines a set of macros to de-allocate dinamically allocated memory.
///         
/// Defines a set of macros to automatically check for NULL value and call the correct
/// operation to de-allocate dinamically allocated memory blocks.
/// \{

/// \def    PCC_OBJ_FREE(ptr)
///
/// \brief  De-allocates memory used by an object allocated with the new operator.
///
/// \param  ptr The pointer to the memory to be freed.
#define PCC_OBJ_FREE(ptr)\
    do { \
        if ( ptr ) { \
            delete ptr; \
            ptr = 0x0; \
        } \
    } while ( false )


/// \def    PCC_ARR_FREE(ptr)
///
/// \brief  De-allocates memory used by an array allocated with the new[] operator.
///
/// \param  ptr The pointer to the memory to be freed.
#define PCC_ARR_FREE(ptr)\
    do { \
        if ( ptr ) { \
            delete[] ptr; \
            ptr = 0x0; \
        } \
    } while ( false )

/// \def    PCC_MEM_FREE(ptr)
///
/// \brief  De-allocates memory allocated by a malloc call.
///
/// \param  ptr The pointer to the memory to be freed.
#define PCC_MEM_FREE(ptr)\
    do { \
        if ( ptr ) { \
            free ( ptr ); \
            ptr = 0x0; \
        } \
    } while ( false )

/// \}

#endif // PCCOMMON_H
