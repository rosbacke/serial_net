/**
 * This file is part of SerialNet.
 *
 *  SerialNet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SerialNet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SerialNet.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * main.cpp
 *
 *  Created on: 30 jun 2016
 *      Author: mikaelr
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <net/if.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

#include <linux/if.h>
#include <linux/if_tun.h>

using std::string;

int
tun_setup(char* dev, int flags)
{

    struct sockaddr_in my_addr;
    struct ifreq ifr;
    int fd, err, s;
    const char* clonedev = "/dev/net/tun";

    // Open clone device file descriptor
    if ((fd = open(clonedev, O_RDWR)) < 0)
    {
        perror("Opening /dev/net/tun");
        return fd;
    }

    // Initialise interface parameters structure
    memset(&ifr, 0, sizeof(ifr));

    // Set up flags
    ifr.ifr_flags = flags;

    // Set up interface name
    if (*dev)
    {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    // Put interface in TUN mode
    if ((err = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0)
    {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    // Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    // Get interface flags
    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
    {
        perror("cannot get interface flags");
        exit(1);
    }

    // Turn on interface
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
    {
        fprintf(stderr, "ifup: failed ");
        perror(ifr.ifr_name);
        exit(1);
    }

    // Set interface address
    bzero((char*)&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(inet_network("192.168.2.1"));
    memcpy(&ifr.ifr_addr, &my_addr, sizeof(struct sockaddr));

    if (ioctl(s, SIOCSIFADDR, &ifr) < 0)
    {
        fprintf(stderr, "Cannot set IP address. ");
        perror(ifr.ifr_name);
        exit(1);
    }

    // Return interface file descriptor
    return fd;
}

int
main()
{
    char name[IFNAMSIZ];
    strcpy(name, "tun77");
    int r = tun_setup(name, 0);
}

#if 0
void* tun_readThreadProc(void* param) {
	struct pollfd fds[1];
	int nread;
	unsigned char buffer[BUFFERSIZE];
	fds[0].fd = tun_fd;
	fds[0].events = POLLIN;

	printf("%s : Entered. tun_fd = %d \n", __FUNCTION__, tun_fd);

	for (;;) {
		printf("%s : Entered loop\n", __FUNCTION__);
		if ((poll(fds, 1, -1)) == -1) {
			perror("poll");
			exit(1);
		}

		printf("%s : Poll sensed something\n", __FUNCTION__);

		if ((nread = read(tun_fd, buffer, BUFFERSIZE)) < 0) {
			perror("read");
			close (tun_fd);
			exit(1);
		}

		printf("%s : Read something : %d bytes\n", __FUNCTION__, nread);
	}
	return 0;
}

#include <net/if.h>
#include <sys/socket.h> // <-- This one
// #include <linux/if_tun.h>
//#include <linux/if.h>
//#include <linux/if_tun.h>

int tun_alloc(char *dev) {
	struct ifreq ifr;
	int fd, err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
	return tun_alloc_old(dev);

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TUN;
	if (*dev)
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if ( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
		close(fd);
		return err;
	}
	strcpy(dev, ifr.ifr_name);
	return fd;
}

int main() {

	/* tunclient.c */
	int tun_fd = -1;
	char tun_name[IFNAMSIZ];

	/* Connect to the device */
	strcpy(tun_name, "tun77");
	tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI); /* tun interface */

	if (tun_fd < 0) {
		perror("Allocating interface");
		exit(1);
	}

	/* Now read data coming from the kernel */
	while (1) {
		/* Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes */
		nread = read(tun_fd, buffer, sizeof(buffer));
		if (nread < 0) {
			perror("Reading from interface");
			close(tun_fd);
			exit(1);
		}

		/* Do whatever with the data */
		printf("Read %d bytes from device %s\n", nread, tun_name);
	}
}
#endif
