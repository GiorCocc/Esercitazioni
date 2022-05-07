#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>



int main(void)
{
  struct stat statbuf;
  int file;

  /* apre il file test.dat dell'esercizio 4 */
  file = open("test.dat", O_RDONLY);
  if (file == -1)
    perror("file opening error");

  /* ottiene informazioni sul file */
  fstat(file, &statbuf);
  close(file);

  /* mostra le informazioni ottenute */
  if (statbuf.st_mode & S_IFCHR) 
    printf("Handle refers to a device.\n");
  if (statbuf.st_mode & S_IFREG)
    printf("Handle refers to an ordinary file.\n");
  if (statbuf.st_mode & S_IREAD)
    printf("User has read permission on file.\n");
  if (statbuf.st_mode & S_IWRITE)
    printf("User has write permission on file.\n");

  printf("Size of file in bytes: %ld\n", statbuf.st_size);
  printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime));

  return 0;
}
