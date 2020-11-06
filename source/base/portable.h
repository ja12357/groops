/***********************************************/
/**
* @file portable.h
*
* @brief Define standard types.
*
* This is important to account for different systems.
* e.g. 'int' have some times 16 Bit or 32 Bit size.
*
* @author Torsten Mayer-Guerr
* @date 2004-10-25
*
*/
/***********************************************/

#ifndef __GROOPS_PORTABLE__
#define __GROOPS_PORTABLE__

#include <cstdint>

/***** DEFINES *********************************/

/// Disable external libraries
// #define NOLIB_ERFA     // compile without ERFA library
// #define NOLIB_Z        // compile without Z library
// #define NOLIB_NETCDF   // compile without NETCDF library

/***** TYPES ***********************************/

/** @brief Standard types.
* This is important to account for different systems.
* e.g. 'int' have some times 16 Bit or 32 Bit size.
*/
//@{
typedef float             Float;
typedef double            Double;
typedef long double       LongDouble;
typedef int               Int;
typedef std::int32_t      Int32;
typedef std::int64_t      Int64;
typedef std::size_t       UInt;
typedef std::uint16_t     UInt16;
typedef std::uint32_t     UInt32;
typedef std::uint64_t     UInt64;
typedef char              Byte;
typedef bool              Bool;
typedef char              Char;
//@}

/***** CONST ***********************************/

#ifndef TRUE
constexpr Bool FALSE = false; //!< boolean true
constexpr Bool TRUE  = true;  //!< boolean false
#endif

/***********************************************/

#endif /* __GROOPS_PORTAB__ */
