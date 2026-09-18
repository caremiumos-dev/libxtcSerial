#ifndef XTCSERIAL_C_H
#define XTCSERIAL_C_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ANDROID__) && defined(__GNUC__)
#define XTCS_API __attribute__((visibility("default")))
#else
#define XTCS_API
#endif

#define XTCS_OK    0
#define XTCS_ERROR (-1)

/* Opens and configures a serial port.
 *
 *   device    path to the port, e.g. "/dev/ttyS0" or "/dev/ttyUSB0"
 *   baud      bits per second, e.g. 9600, 115200 (0/unknown -> 9600)
 *   data_bits 5, 6, 7 or 8                        (default 8)
 *   stop_bits 1 or 2                              (default 1)
 *   parity    'N' (none), 'E' (even) or 'O' (odd)  (default 'N')
 *
 * Returns a file descriptor >= 0 on success, or XTCS_ERROR (-1).
 * The port is put in raw mode (no canonical processing).
 */
XTCS_API int xtcs_open(const char* device, int baud, int data_bits, int stop_bits, char parity);

/* Reads up to count bytes.  Returns the number of bytes read, or -1. */
XTCS_API ssize_t xtcs_read(int fd, void* buf, size_t count);

/* Writes count bytes.  Returns the number of bytes written, or -1. */
XTCS_API ssize_t xtcs_write(int fd, const void* buf, size_t count);

/* Closes the port.  Returns XTCS_OK or XTCS_ERROR. */
XTCS_API int xtcs_close(int fd);

#ifdef __cplusplus
}
#endif

#endif /* XTCSERIAL_C_H */