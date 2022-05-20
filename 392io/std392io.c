#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


// from https://www.techiedelight.com/implement-itoa-function-in-c/// Function to swap two numbers
void __swap(char *x, char *y)
{
    char t = *x;
    *x = *y;
    *y = t;
}

// Function to reverse `buffer[i…j]`
char *__reverse(char *buffer, int i, int j)
{
    while (i < j)
    {
        __swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

// Iterative function to implement `itoa()` function in C
char *__itoa(int value, char *buffer)
{
    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % 10;

        if (r >= 10)
        {
            buffer[i++] = 65 + (r - 10);
        }
        else
        {
            buffer[i++] = 48 + r;
        }

        n = n / 10;
    }

    // if the number is 0
    if (i == 0)
    {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0)
    {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return __reverse(buffer, 0, i - 1);
}

int output(char *filename, char format, void *data)
{

    if (!data)
    {
        errno = EIO;
        return -1;
    }

    char *output;

    // convert data to char
    if (format == 's')
    {
        // output string
        output = (char *)data;
        // if base address is null
    }

    else if (format == 'd')
    {
        // output integer
        int *buffer = (int *)data;
        int temp = *buffer;
        char temp2[40]; // over allocated?
        // if base address is null

        // convert integer to string
        output = __itoa(temp, temp2);
    }
    else
    {
        // invalid format
        errno = EIO;
        return -1;
    }

    // if file name is empty, print the data to terminal
    if (strlen(filename) == 0)
    {
        write(1, output, strlen(output));
        write(1, "\n", strlen("\n"));
        return 0;
    }

    // else
    //  if the file exists, append data to the end of the file

    //  if file doesnt exist create a new file with rw-r----- permission, and write to that file.

    // https://stackoverflow.com/questions/1563168/example-of-realpath-function-in-c

    char *path = (char *)malloc(2000);
    realpath(filename, path);

    int fd = open(path,
                  O_CREAT | O_WRONLY | O_APPEND,
                  S_IRUSR | S_IWUSR | S_IRGRP);

    // write only
    write(fd, output, strlen(output));
    write(fd, "\n", strlen("\n"));
    free(path);
    return 0;
}

int input(char *filename, char format, void *data)
{

    if (!data)
    {
        return 0;
    }

    int fd;
    char tmp;

    // if file name is empty, read data from keyboard
    if (strlen(filename) == 0)
    {
        // check format
        fd = 0;
    }

    // https://www.delftstack.com/howto/c/c-check-if-file-exists/#:~:text=file's%20full%20path.-,access()%20Function%20to%20Check%20if%20a%20File%20Exists%20in,and%20X_OK%20to%20execute%20permission.
    //  check if file exists
    else if (access(filename, F_OK) == -1)
    { // prioritize
        // when file doesnt exist
        errno = ENOENT;
        return -1;
    }
    else
    {
        char *path = (char *)malloc(2000);
        realpath(filename, path);
        // if file exists: check opened or not
        // get the inode number for input filename
        // inode of filename
        struct stat temp;
        // get full path to file name
        // realpath
        stat(path, &temp);
        ino_t inode1 = temp.st_ino;

        // To see if a file is opened, first get all file descriptors from the current process. Then
        // compare their inode numbers with the inode number of the file you want to open;

        // https://stackoverflow.com/questions/1563168/example-of-realpath-function-in-c

        char pid[1000];
        __itoa(getpid(), pid);
        // allocate space for null chars
        char *dir = (char *)malloc(5 + strlen("/proc/") + strlen(pid) + strlen("/fd/"));

        strcpy(dir, "/proc/");
        strcat(dir, pid);
        strcat(dir, "/fd/");

        // go through every file in fd. get inode number
        DIR *dp = opendir(dir);

        int opened = 0;

        // open file
        struct dirent *dirp;
        while ((dirp = readdir(dp)) != NULL)
        {
            if (!strcmp(dirp->d_name, ".") == 0 && !strcmp(dirp->d_name, "..") == 0)
            {
                // out of range
                if (atoi(dirp->d_name) > 2)
                {
                    if (atoi(dirp->d_name) == 1024)
                    {
                        break;
                    }

                    struct stat fi;
                    fstat(atoi(dirp->d_name), &fi);
                    ino_t inode = fi.st_ino;

                    if (inode == inode1)
                    {
                        // same inode number means that file is opened already
                        // get current position
                        fd = atoi(dirp->d_name);
                        opened = 1;
                        break; // exit loop
                    }
                }
            }
        }

        // free

        closedir(dp);
        free(dir);

        if (opened == 0)
        {
            // if file is not opened open file first
            fd = open(path, O_RDONLY);
        }
        free(path);

        // check if reached end of file
        if (read(fd, &tmp, 1) == 0)
        {
            // no need to set errno
            return -1;
        }
        // go back one char
        lseek(fd, -1, SEEK_CUR);
    }

    char *buffer = malloc(128);
    int index = 0;
    int curr_size = 128;
    // read from terminal
    // allcoate more space each time
    memset(buffer, 0, curr_size);

//https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
    while (read(fd, &tmp, 1) != 0)
    {
        // end if end of line
        if (tmp == '\n')
            break;

        // need to allocate new buffer
        if (index == curr_size)
        {
            char *newbuf = malloc(curr_size + 128);
            memset(newbuf, 0, curr_size);
            // copy characters over
            for (size_t j = 0; j < curr_size; j++)
            {
                newbuf[j] = buffer[j];
            }

            free(buffer);
            // swap
            buffer = newbuf;
            curr_size += 128;
        }
        // add current scan to buffer
        buffer[index] = tmp;

        index++;
    }

    //before processing data: clean up data 

    int i = 0;
    while (((char *)data)[i] != 0)
    {
        ((char *)data)[i] = 0;
        i++;
    }

    if (format != 'd' && format != 's')
    {

        free(buffer);
        errno = EIO;
        return -1;
    }

    if (format == 's')
    {

        // clean the data

        for (int i = 0; i < index; i++)
        {
            ((char *)data)[i] = buffer[i];
        }

        free(buffer);

        return 0;
    }

    // write(1,buffer,strlen(buffer));
    // write(1,"end of buffer",strlen("end of buffer"));
    *(int *)data = atoi(buffer);
    free(buffer);
    return 0;
}

int clean()
{
    char pid[1000];
    __itoa(getpid(), pid);
    // allocate space for null chars
    char *dir = (char *)malloc(5 + strlen("/proc/") + strlen(pid) + strlen("/fd/"));

    strcpy(dir, "/proc/");
    strcat(dir, pid);
    strcat(dir, "/fd/");

    // go through every file in fd. get inode number
    DIR *dp = opendir(dir);

    // open file
    struct dirent *dirp;

    while ((dirp = readdir(dp)) != NULL)
    {
        if (!strcmp(dirp->d_name, ".") == 0 &&
            !strcmp(dirp->d_name, "..") == 0)
        {
            int fd = atoi(dirp->d_name);
            // dont close stdin stdout and stderr
            if (fd > 2)
            {

                if (fd == 1024)
                {
                    break;
                }

                if (close(fd) == -1)
                {
                    errno = EIO;
                    closedir(dp);
                    free(dir);
                    return -1;
                }
                close(fd);
            }
        }
    }

    closedir(dp);
    free(dir);
    return 0;
}