#include <fcntl.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <errno.h>
 #include <string.h>
 #include <dirent.h>
 #include <sys/stat.h>



int output(char* , char , void* );
int input(char* , char , void* );
int clean();
void __swap(char *, char *);
// Function to reverse `buffer[i…j]`
char* __reverse(char *, int , int );
// Iterative function to implement `itoa()` function in C
char* __itoa(int , char* , int );