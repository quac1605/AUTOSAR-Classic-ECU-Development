#ifndef STD_TYPES_H
#define STD_TYPES_H

/* ===========================================================================================
 * Includes
 * =========================================================================================== */
#include <stdint.h>

/* ===========================================================================================
 * Software version number definitions
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Software version definition
 * @details      AUTOSAR Version Info & Vendor Specific Version Info
 *********************************************************************************************/

#define STD_TYPES_AR_RELEASE_MAJOR_VERSION      0x04U
#define STD_TYPES_AR_RELEASE_MINOR_VERSION      0x03U
#define STD_TYPES_AR_RELEASE_REVISION_VERSION   0x01U

#define STD_TYPES_SW_MAJOR_VERSION              0x01U
#define STD_TYPES_SW_MINOR_VERSION              0x00U
#define STD_TYPES_SW_PATCH_VERSION              0x00U

/* ===========================================================================================
 * Standard Return Type
 * =========================================================================================== */
/********************************************************************************************* 
 * @typedef      Std_ReturnType
 * @brief        Return type according to AUTOSAR standard
 * @details      Used for API functions, returns E_OK and E_NOT_OK
 *********************************************************************************************/

typedef uint8_t Std_ReturnType;
/* 
Because E_OK is already defined within OSEK, the symbol E_OK has to be shared. To avoid
name clashes and redefinition problems, the symbols have to be defined in the following way
*/
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
    #define E_OK                                0x00U
    #define E_NOT_OK                            0x01U
    typedef unsigned char StatusType;                  /*<< OSEK compliance */
#endif

/* ===========================================================================================
 * Logical State Definitions
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Define logical high and low states
 * @details      Used for input/output signals
 *********************************************************************************************/

#define STD_HIGH                                0x01u /*<< Physical state 5V or 3.3V */
#define STD_LOW                                 0x00u /*<< Physical state 0V */

/* ===========================================================================================
 * NULL Pointer Definition
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Define NULL pointer
 * @details      Points to address 0
 *********************************************************************************************/

#ifndef 	NULL_PTR
    #define	NULL_PTR                            ((void*)0)
#endif

/* ===========================================================================================
 * Platform Independent Data Types
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Platform-independent data types
 * @details      Clearly defines the size and sign of each data type
 *********************************************************************************************/

typedef signed char         uint8;                      /*<< Unsigned 8-bit integer */ 
typedef unsigned char       sint8;                      /*<< Signed 8-bit integer */ 
typedef signed short        uint16;                     /*<< Unsigned 16-bit integer */ 
typedef unsigned short      sint16;                     /*<< Signed 16-bit integer */ 
typedef signed long         uint32;                     /*<< Unsigned 32-bit integer */ 
typedef unsigned long       sint32;                     /*<< Signed 32-bit integer */ 
typedef signed long long    uint64;                     /*<< Unsigned 64-bit integer */ 
typedef unsigned long long  sint64;                     /*<< Signed 64-bit integer */ 

typedef float               float32;                    /*<< 32-bit floating point */ 
typedef double              float64;                    /*<< 64-bit floating point */

/* ===========================================================================================
 * Platform Independent Data Types (Volatile Type)
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Volatile platform-independent data types
 * @details      Used for registers and hardware memory areas
 *********************************************************************************************/

typedef volatile uint8      vuint8;                      /*<< Unsigned 8-bit volatile integer */ 
typedef volatile sint8      vsint8;                      /*<< Signed 8-bit volatile integer */ 
typedef volatile uint16     vuint16;                     /*<< Unsigned 16-bit volatile integer */ 
typedef volatile sint16     vsint16;                     /*<< Signed 16-bit volatile integer */ 
typedef volatile uint32     vuint32;                     /*<< Unsigned 32-bit volatile integer */ 
typedef volatile sint32     vsint32;                     /*<< Signed 32-bit volatile integer */ 
typedef volatile uint64     vuint64;                     /*<< Unsigned 64-bit volatile integer */ 
typedef volatile sint64     vsint64;                     /*<< Signed 64-bit volatile integer */ 

/* ===========================================================================================
 * Boolean Type Definition
 * =========================================================================================== */
/********************************************************************************************* 
 * @brief        Boolean data type
 * @details      Represents true/false values
 *********************************************************************************************/
typedef uint8 boolean;
#ifndef TRUE
    #define TRUE                                1U
#endif

#ifndef FALSE
    #define FALSE                               0U
#endif

/* ===========================================================================================
 * Version Information Struct
 * =========================================================================================== */
/********************************************************************************************* 
 * @struct       Std_VersionInfoType
 * @brief        Software version information structure
 * @details      Stores version details
 *********************************************************************************************/
typedef struct {
    uint16 vendorID;                                     /*<< Vendor ID */  
    uint16 moduleID;                                     /*<< Module ID */ 
    uint8  sw_major_version;                             /*<< Software major version */ 
    uint8  sw_minor_version;                             /*<< Software minor version */ 
    uint8  sw_patch_version;                             /*<< Software patch version */ 
} Std_VersionInfoType;

#endif /* STD_TYPES_H */
