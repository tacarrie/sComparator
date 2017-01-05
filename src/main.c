/**
 * \file main.c
 * \brief Application comparing the received SALSIS values
 * \details This program implements the comparison of a SALSIS value that is received via two separate serial interface
 * \author tacarrie
 * \version 0.1
 * \date 04.01.2017
 * \pre First initialize the system.
 * \bug Unknown
 * \warning This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * \copyright (C) 2016 by tacarrie.
 */


/* INCLUDES ****************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <sys/ioctl.h>   /* ioctl()                            */

int getCTS(int fd)
{
    int s;
    /* Read terminal status line: Clear To Send */
    ioctl(fd, TIOCMGET, &s);
    return (s & TIOCM_CTS) != 0;
}

unsigned long printTime(int n)
{
	/*time_t current_time;
	char* c_time_str;
	current_time = time(NULL);
	c_time_str = ctime(&current_time);
	printf("%d. Current time is %s\n", n, c_time_str);*/
	struct timeval t1;
	gettimeofday(&t1, NULL);
	//printf("%d. Current time in micro is %lu\n", n, t1.tv_usec);
	return t1.tv_usec;
}

/**
 * Main function
 * @return Result of application execution
 */
int main(int argc, char *argv[]) {
	char ttydev[] = "/dev/ttyUSB0";
	int fd;

	// Check arguments
	int i;
	for (i = 0; i < argc; i++) {
		printf("Argument %d: %s\n", i, argv[i]);
	}

	switch (argc) {
	case 1:
		// no arguments. Use default.
		break;
	case 2:
		strcpy(ttydev, argv[1]);
		break;
	default:
		printf("invalid number of arguments. Exit.\n");
		return EXIT_FAILURE;
		break;
	}
	printf("Using TTY device: %s\n", ttydev);

	fd = open(ttydev, O_RDWR | O_NOCTTY);
	if (fd == 1) {
		fprintf(stderr, "Error in opening %s\n", ttydev);
		return EXIT_FAILURE;
	} else {
		printf("%s opened successfully\n", ttydev);
	}

	/*---------- Setting the Attributes of the serial port using termios structure --------- */

	struct termios SerialPortSettings; /* Create the structure                          */

	tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

	cfsetispeed(&SerialPortSettings, B115200); /* Set Read  Speed as 115200                       */
	cfsetospeed(&SerialPortSettings, B115200); /* Set Write Speed as 115200                       */

	SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
	SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the mask for setting the data size             */
	SerialPortSettings.c_cflag |= CS8; /* Set the data bits = 8                                 */

	SerialPortSettings.c_cflag &= ~CRTSCTS; /* No Hardware flow Control                         */
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

	/* Setting Time outs */
	SerialPortSettings.c_cc[VMIN] = 24; /* Read 10 characters */
	SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinitely would be 0  */

	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable XON/XOFF flow control */
	//SerialPortSettings.c_iflag |= IXON | IXOFF | IXANY; /* Enable XON/XOFF flow control */
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */

	SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

	/* Flush Port, then applies attributes */
	tcflush(fd, TCIFLUSH);

	int result;
	if ((result = tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0) { /* Set the attributes to the termios structure*/
		fprintf(stderr, "ERROR %d in Setting attributes\n", result);
		return EXIT_FAILURE;
	} else {
		printf("  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none\n  FlowCtrl = none\n");
	}

	/*--------- Controlling the RTS pins  of Serial Port --------*/
	int RTS_flag, CTS_flag;
	RTS_flag = TIOCM_RTS;	/* Modem Constant for RTS pin */
	CTS_flag = TIOCM_CTS;



	unsigned int position = 0x000000; // 3 bytes absolute position
	unsigned int speed = 0x0000; //  2 bytes travel speed



	/* Allocate memory for read buffer */

	char parsed_pos[] = "000000";
	char parsed_spd[] = "0000\0";
	char response [256];
	memset (&response, '\0', sizeof response);

	unsigned long start, stop;
	for(i=0; i<10; i++){
		int spot = 0;
		char read_buf = '\0';
		int byte_read = 0;

		printTime(1);

		while(getCTS(fd)){
			usleep(1);
		}
		start = printTime(2);
		//printf("Status of CTS: %d\n", getCTS(fd));
		/*if(!getCTS(fd)){
			do {
				byte_read = read(fd, &read_buf, 1 );
				sprintf( &response[spot], "%c", read_buf );
				spot += byte_read;
			} while (read_buf != '\n' && byte_read > 0);

			tcflush(fd, TCIFLUSH);

			if (byte_read < 0) {
				printf("Error reading: %s\n", strerror(errno));
			} else if (byte_read == 0) {
				printf("Read nothing!\n");
			} else {
				printf("Byte read: %d Response: %s\n", byte_read, response);
				printf("First byte: %d\n", response[0]);
			}*/
/*
			int len = strlen(response);
			if (len > 4){
				strncpy(parsed_pos, response, len - 5);
				parsed_pos[len - 5] = 0;
				printf("Cut response: %s\n", parsed_pos);
				position = (int)strtol(parsed_pos, NULL, 16);
				printf("Parsed response: %d\n", position);
			}*/
		//}
		printTime(3);
		while(!getCTS(fd)){
			usleep(1);
		}
		stop = printTime(4);
		//printf("Status of CTS: %d\n", getCTS(fd));
		printf("Delay in microseconds: %lu\n", stop - start);
	}


	/*
	 char read_buffer[32];
	 int  bytes_read = 0;

	 bytes_read = read(fd,&read_buffer,32);
	 printf("Bytes read: %d\n", bytes_read);
	 */
	close(fd);
	return EXIT_SUCCESS;
}

