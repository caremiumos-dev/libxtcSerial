#ifndef XTCSERIAL_HPP
#define XTCSERIAL_HPP

#include "xtcserial_c.h"

#include <cstddef>

namespace xtcserial {

enum class parity : char {
    none = 'N',
    even = 'E',
    odd  = 'O',
};

struct serial_params {
    int   baud      = 9600;
    int   data_bits = 8;
    int   stop_bits = 1;
    parity parity   = parity::none;
};

/* RAII serial-port handle built on top of the xtcs_* C API.  The port is
 * opened in raw mode and configured with the requested baud/parity. */
class __attribute__((visibility("default"))) serial {
public:
    static const int kInvalidFd = -1;

    /* Default-constructed: not open. */
    serial() = default;

    /* Opens (and configures) the given device.  Check good() afterwards. */
    explicit serial(const char* device, const serial_params& p = serial_params());

    /* Closes the port if still open. */
    ~serial();

    serial(const serial&) = delete;
    serial& operator=(const serial&) = delete;

    bool    good() const { return fd_ >= 0; }
    int     get_fd() const { return fd_; }

    ssize_t read(void* buf, size_t count);
    ssize_t write(const void* buf, size_t count);
    void    close();

private:
    int fd_ = kInvalidFd;
};

}  // namespace xtcserial

#endif /* XTCSERIAL_HPP */