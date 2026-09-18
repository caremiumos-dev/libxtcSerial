#include "xtcserial.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

namespace {

int to_speed(int baud)
{
    switch (baud) {
    case 50: return B50;
    case 75: return B75;
    case 110: return B110;
    case 134: return B134;
    case 150: return B150;
    case 200: return B200;
    case 300: return B300;
    case 600: return B600;
    case 1200: return B1200;
    case 1800: return B1800;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    case 460800: return B460800;
    case 500000: return B500000;
    case 576000: return B576000;
    case 921600: return B921600;
    case 1000000: return B1000000;
    case 1152000: return B1152000;
    case 1500000: return B1500000;
    case 2000000: return B2000000;
    default: return B0;
    }
}

int to_csize(int bits)
{
    switch (bits) {
    case 5: return CS5;
    case 6: return CS6;
    case 7: return CS7;
    default: return CS8;
    }
}

int apply_termios(int fd, int baud, int data_bits, int stop_bits, char parity)
{
    struct termios tio;
    if (tcgetattr(fd, &tio) != 0) {
        return XTCS_ERROR;
    }

    speed_t speed = to_speed(baud);
    if (speed == B0) {
        speed = B9600;
    }
    cfsetispeed(&tio, speed);
    cfsetospeed(&tio, speed);

    tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio.c_oflag &= ~(OPOST);

    tio.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CREAD | CLOCAL);
    tio.c_cflag |= CREAD | CLOCAL | to_csize(data_bits);
    if (stop_bits == 2) {
        tio.c_cflag |= CSTOPB;
    }
    switch (parity) {
    case 'E':
    case 'e':
        tio.c_cflag |= PARENB;
        break;
    case 'O':
    case 'o':
        tio.c_cflag |= PARENB | PARODD;
        break;
    default:
        break;
    }

    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    return tcsetattr(fd, TCSANOW, &tio);
}

}  // namespace

int xtcs_open(const char* device, int baud, int data_bits, int stop_bits, char parity)
{
    if (device == nullptr) {
        return XTCS_ERROR;
    }

    int fd = open(device, O_RDWR | O_NOCTTY | O_DSYNC | O_LARGEFILE);
    if (fd < 0) {
        return XTCS_ERROR;
    }

    if (apply_termios(fd, baud, data_bits, stop_bits, parity) != XTCS_OK) {
        close(fd);
        return XTCS_ERROR;
    }

    return fd;
}

ssize_t xtcs_read(int fd, void* buf, size_t count)
{
    return read(fd, buf, count);
}

ssize_t xtcs_write(int fd, const void* buf, size_t count)
{
    return write(fd, buf, count);
}

int xtcs_close(int fd)
{
    return close(fd);
}

namespace xtcserial {

serial::serial(const char* device, const serial_params& p)
    : fd_(xtcs_open(device, p.baud, p.data_bits, p.stop_bits, static_cast<char>(p.parity)))
{
}

serial::~serial()
{
    close();
}

ssize_t serial::read(void* buf, size_t count)
{
    return ::read(fd_, buf, count);
}

ssize_t serial::write(const void* buf, size_t count)
{
    return ::write(fd_, buf, count);
}

void serial::close()
{
    if (fd_ != kInvalidFd) {
        ::close(fd_);
        fd_ = kInvalidFd;
    }
}

}  // namespace xtcserial