#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

struct record {
    void *this_fn;
    void *call_site;
    clock_t start_time;
    clock_t end_time;
};

/*
 * Main function for analyzing calltrace files.
 * @param argc: The number of command-line arguments.
 * @param argv: An array of strings containing command-line arguments.
 * @return 0 on success.
 */
int main(int argc, char *argv[]) {
    char *calltrace_file;

    //check if we have a commmand line argument
    if (argc == 2) {
        calltrace_file = argv[1];
    } else {
        //if there was no command line argument, check for a .calltrace file in the current directory
        struct stat st;//part of posix to get information about a file
        if (stat(".", &st) == 0 && S_ISDIR(st.st_mode)) {//returns 0 if successful, makes sure its a valid path
            struct dirent *entry;
            DIR *dir = opendir(".");//current directory
            while ((entry = readdir(dir))) {
				//check if the file name ends with .calltrace
                if (strcmp(entry->d_name + strlen(entry->d_name) - 10, ".calltrace") == 0) {
                    calltrace_file = entry->d_name;
                    break;
                }
            }
            closedir(dir);
        }
    }
	
    if (calltrace_file == NULL) {
        fprintf(stderr, "there was no calltrace file argument provided or unable to find calltrace file\n");
        exit(1);
    }

    //opens the call trace file for reading
    int fd = open(calltrace_file, O_RDONLY);//returns -1 if there was an error
    if (fd == -1) {
        perror("unable to open calltrace file");
        exit(1);
    }

    struct record rec;
    ssize_t bytes_read;
	//if we find a record
    while ((bytes_read = read(fd, &rec, sizeof(struct record)) == sizeof(struct record))) {
		//print what file its from
        printf("File: %s\n", calltrace_file);
		//print memory address of where the function was called from, and its start and end time
        printf("this_fn: %p, call_site: %p, start_time: %ld, end_time: %ld\n",
               rec.this_fn, rec.call_site, rec.start_time, rec.end_time);
    }

    if (bytes_read == -1) {
        perror("unable to read calltrace file");
    }

    close(fd);
    return 0;
}
