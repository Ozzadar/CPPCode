#include <iostream>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

char *portName = "/dev/rfcomm0";
char buf [256];

using namespace std;


int main()
{
        int fileDescriptor;

        // Open the file descriptor in non-blocking mode
        fileDescriptor = open(portName, O_RDWR | O_NOCTTY);

        //Set up the control structure
        struct termios tOptions;

        //Get currently set options for the tty
        tcgetattr(fileDescriptor, &tOptions);

        //Set Custom Options

        //9600 Baud
        cfsetispeed(&tOptions, B9600);
        cfsetospeed(&tOptions, B9600);

        //8 bits, no parity, no stop bits
        tOptions.c_cflag &= ~PARENB;
        tOptions.c_cflag &= ~CSTOPB;
        tOptions.c_cflag &= ~CSIZE;
        tOptions.c_cflag |= CS8;

        //no hardware flow control
        tOptions.c_cflag &= ~CRTSCTS;

        //enable receiver, ignore status lines
        tOptions.c_cflag |= CREAD | CLOCAL;

        //diable input/output flow control, disable restart chars
        tOptions.c_iflag &= ~(IXON | IXOFF | IXANY);

        //disable canonical input, disable echo
        //disable visually erase chars
        //disable terminal generated signals
        tOptions.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        //disable output processing
        tOptions.c_oflag &= ~OPOST;

        //wait for 24 characters to come in before read returns
        tOptions.c_cc[VMIN] = 29;
        //no minimum time to wait before read returns
        tOptions.c_cc[VTIME] = 0;

        //commit the options

        tcsetattr(fileDescriptor, TCSANOW, &tOptions);

        //wait for arduino to reset
        usleep(1000*1000);

        //flush anything already in the serial buffer
        tcflush(fileDescriptor, TCIFLUSH);


        //read up to 128 bytes from the fd
        int n = read(fileDescriptor, buf, 29);

        while (n != -1) {

            printf("%s\0", buf);

            n = read(fileDescriptor, buf, 29);
        }

        int exit = close(fileDescriptor);
        return exit;
}
