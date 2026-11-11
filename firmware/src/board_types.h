/* ************************************************************************** */
/** @file board_types.h
 *  @brief Board type enumeration for Microchip Blue Wedge platform.
 *
 *  @company
 *      Microchip Technology Inc.
 *
 *  @summary
 *      Defines board identifiers for fixed IP addressing and MQTT indexing.
 *
 *  @description
 *      This header file provides the BOARD_TYPES enumeration, which assigns a
 *      unique identifier to each supported board on the Microchip Blue Wedge
 *      platform. These identifiers are used to:
 *        - Assign fixed IP addresses to each board, starting from the base
 *          address 192.168.0.100 (T1S network). The enum value is used as an
 *          offset from the base address.
 *        - Serve as indexes for MQTT topics or client identification.
 *      The network gateway uses the fixed IP address 192.168.0.5.
 *      For example, SMART_SENSOR_BOARD (0) maps to 192.168.0.100,
 *      SMART_HMI_BOARD (1) maps to 192.168.0.101, and so on.
 */
/* ************************************************************************** */

#ifndef _BOARD_TYPES_H    /* Guard against multiple inclusion */
#define _BOARD_TYPES_H

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */

/* No additional files are included in this header at this time. */
/* Add #include directives here if dependencies are introduced in the future. */

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* Section: Data Types                                                        */
/* ************************************************************************** */

/**
 * @enum BOARD_TYPES
 * @brief Enumeration of Blue Wedge board types for IP and MQTT indexing.
 *
 * Each value in this enumeration uniquely identifies a board type.
 * The enumeration value is used as:
 *   - An offset from the base IP address 192.168.0.100 (T1S network).
 *   - An index or identifier for MQTT topics or clients.
 *
 * | Enum Value          | Board Name         | IP Address        | MQTT Index |
 * |---------------------|--------------------|-------------------|------------|
 * | SMART_SENSOR_BOARD  | Smart Sensor Board | 192.168.0.100     | 0          |
 * | SMART_HMI_BOARD     | Smart HMI Board    | 192.168.0.101     | 1          |
 * | SMART_IO_BOARD      | Smart IO Board     | 192.168.0.102     | 2          |
 * | TEST_ME_BOARD       | Test Me Board      | 192.168.0.103     | 3          |
 * | DEMO_BOARD          | Demo Board         | 192.168.0.104     | 4          |
 * | SMART_MOTOR_BOARD   | Smart Motor Board  | 192.168.0.105     | 5          |
 *
 * @note The network gateway uses the fixed IP address 192.168.0.5.
 */
typedef enum
{
    SMART_SENSOR_BOARD,   /*< Smart Sensor Board: 192.168.0.100, MQTT index 0 */
    SMART_HMI_BOARD,      /*< Smart HMI Board:    192.168.0.101, MQTT index 1 */
    SMART_IO_BOARD,       /*< Smart IO Board:     192.168.0.102, MQTT index 2 */
    TEST_ME_BOARD,        /*< Test Me Board:      192.168.0.103, MQTT index 3 */
    DEMO_BOARD,           /*< Demo Board:         192.168.0.104, MQTT index 4 */
    SMART_MOTOR_BOARD     /*< Smart Motor Board:  192.168.0.105, MQTT index 5 */
} BOARD_TYPES;

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BOARD_TYPES_H */

/* *****************************************************************************
 * End of File
 */
