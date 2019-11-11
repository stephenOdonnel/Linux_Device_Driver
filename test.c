#include <stdio.h>	
#include <sys/types.h>  
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>


#define BUFFER_SIZE 64
/* Use 'k' as magic number */
#define ENSICAEN_READ_BLOCKING _IOW('k', 1, const char *)


int main()
{
  char buffer[BUFFER_SIZE];
  int len;
  int activation = 1;
  
  int fd = open("/dev/EnsiCaen_Device", O_RDWR);
  if (fd == -1) {
    perror("open error");
    return -1;
  }

  ioctl(fd, ENSICAEN_READ_BLOCKING, (const char*)&activation);
  int err = ioctl((int)fd, ENSICAEN_READ_BLOCKING, (const char*)&activation);
  len = read(fd, buffer, BUFFER_SIZE);
  printf("length : %d\n",len);
  printf("buffer : %s\n", buffer);
  
  if (len == -1) {
    perror("read error");
    return -1;
  }
	return 0;	

}
