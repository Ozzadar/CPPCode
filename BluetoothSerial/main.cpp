#include <iostream>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

char *portName = "/dev/rfcomm0";
char buf [256];

using namespace std;


//basic 3space vector
struct vec3 {
    double x;
    double y;
    double z;

};

int readline ();
int readGyro ();
int readAccel ();
void parseInput();

int input = 0;
int fileDescriptor;

vec3 recentGyro;
vec3 recentAccel;

int main()
{

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

        while (readline() != 0) {

            cin >> input;

            cout << input << '\n';
            switch(input) {
                case 1: {
                    //readGyro(vec3 &output);
                    cout << "gx: " << recentGyro.x << "\n gy: " << recentGyro.y << "\n gz: " << recentGyro.z << "\n";
                    break;
                }
                case 2: {
                    //readAccel(vec3 &output);
                    cout << "ax: " << recentAccel.x << "\n ay: " << recentAccel.y << "\n az: " << recentAccel.z << "\n";
                    break;
                }
                case 3: {
                    //calibrate;
                    break;
                }
                default: {

                            //wait for arduino to reset
                    usleep(1000*1000);
                    tcflush(fileDescriptor, TCIFLUSH);
                    return close(fileDescriptor);
                }



            }
            usleep (1000);

        }
}

int readline() {

   int n = read(fileDescriptor, buf, 29);
   parseInput();
   return n;

}

void parseInput() {


    char s[2] = ",";
    char *token;
    double inputs[6];
    int loopNumber = 0;

    token = strtok(buf, s);
    inputs[loopNumber] = atof(token);

    loopNumber ++;
    while (token != NULL) {

        token = strtok(NULL, s);
        if (token != NULL) {
            inputs[loopNumber] = atof(token);
        }

        loopNumber++;
    }

    recentAccel.x = inputs[0];
    recentAccel.y = inputs[1];
    recentAccel.z = inputs[2];
    recentGyro.x = inputs[3];
    recentGyro.y = inputs[4];
    recentGyro.z = inputs[5];


}
