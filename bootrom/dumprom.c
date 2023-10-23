/*
 * dumprom.c
 * Tom Trebisky  12-14-2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

off_t target = 0xffff0000;
int size = 32 * 1024;
  
int main(int argc, char **argv)
{
    int mfd;
    int ofd;
    void *map_base;

    mfd = open("/dev/mem", O_RDWR | O_SYNC);
    if ( mfd < 0 ) {
    	printf ("Cannot open /dev/mem\n" );
	exit -1;
    }
    
    map_base = mmap ( 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, target );
    if ( map_base == (void *) -1 ) {
    	printf ("mmap fails\n" );
	exit -1;
    }

    // printf ( "Mapped OK\n" );

    ofd = open ( "rom.bin", O_WRONLY | O_CREAT, 00644 );
    if ( ofd < 0 ) {
    	printf ("Cannot create output file\n" );
	exit -1;
    }

    write ( ofd, map_base, size );
    close ( ofd );
	
    munmap ( map_base, size );
    close(mfd);
    return 0;
}

/* THE END */
