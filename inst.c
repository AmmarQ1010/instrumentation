#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

struct record {
    void *this_fn;
    void *call_site;
    clock_t start_time;
    clock_t end_time;
};

/*
 * Handles function entry and records function call information.
 * @param this_fn: Pointer to the current function.
 * @param call_site: Pointer to the call site.
 */
void __cyg_profile_func_enter(void *this_fn, void *call_site) {
    struct record rec;
    rec.this_fn = this_fn;//stores the current function name in the record
	rec.call_site = call_site;//stores the call site information in the record
	rec.start_time = clock();//records the start time of the function execution
	rec.end_time = 0;//initializes the end time to 0 to be used later
	
	//gets program name from the environment variable "_"
    char *program_name = getenv("_");
    if (program_name == NULL) {
        fprintf(stderr, "unable to get program name\n");
        exit(1);
    }

	//open file
    char calltrace_file[100];//make sure the file can have up to 100 characters in the name
    snprintf(calltrace_file, sizeof(calltrace_file), "%s.calltrace", program_name);
    int fd = open(calltrace_file, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("unable to open calltrace file");
        exit(1);
    }

    //write to file
    ssize_t bytes_written = write(fd, &rec, sizeof(struct record));
    if (bytes_written != sizeof(struct record)) {
        perror("unable to write to calltrace file");
        close(fd);
        exit(1);
    }

    close(fd);
}

/*
 * Handles function exit and updates the end time of the recorded function call.
 * @param this_fn: Pointer to the current function.
 * @param call_site: Pointer to the call site.
 */
void __cyg_profile_func_exit(void *this_fn, void *call_site) {
	//gets program name from the environment variable "_"
    char *program_name = getenv("_");
    if (program_name == NULL) {
        fprintf(stderr, "unable to get program name\n");
        exit(1);
    }

    //open the call trace file for updating end_time
    char calltrace_file[100];
    snprintf(calltrace_file, sizeof(calltrace_file), "%s.calltrace", program_name);
    int fd = open(calltrace_file, O_RDWR);
    if (fd == -1) {
        perror("unable to open calltrace file");
        exit(1);
    }

    //finds the record in the call trace file and updates its end_time
    struct record rec;
    ssize_t bytes_read;//keeps track of number of bytes read
    while ((bytes_read = read(fd, &rec, sizeof(struct record)) == sizeof(struct record))) {
		//checks if our record's this_fn and call_site match the one in the file
        if (rec.this_fn == this_fn && rec.call_site == call_site) {
			//when found, update end_time
            rec.end_time = clock();//find end_time
            lseek(fd, -sizeof(struct record), SEEK_CUR);//updates the file pointer back to the beginning of the record we just found
            ssize_t bytes_written = write(fd, &rec, sizeof(struct record));//updates end_time
            if (bytes_written != sizeof(struct record)) {
                perror("unable to update calltrace file");
            }
            break;
        }
    }

    close(fd);
}

