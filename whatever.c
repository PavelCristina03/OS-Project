#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
 
int checkArguments(int argc) {
 
    if(argc < 2) {
        perror("Incorrect number of arguments! Usage: ./a.out <file/directory/link> ...\n");
        exit(EXIT_FAILURE);
    }
    return 0;
 
}
 
void menu_RegularFiles() {
    printf("\n----  MENU ----\n");
    printf("-n: name\n-d: size\n-h: hard link count\n-m: time of last modification\n-a: access rights\n-l: create symbolic link\n");
    printf("\nPlease, enter your options: ");
}
 
void menu_Directory() {
    printf("\n----  MENU ----\n");
    printf("-n: name\n-d: size\n-a: access rights\n-c: total number of files with .c extension\n");
    printf("\nPlease, enter your options: ");
}
 
void menu_SymbolicLink() {
    printf("\n----  MENU ----\n");
    printf("-n: name\n-l: delete symbolic link\n-d: size of symbolic link\n-t: size of target file\n-a: access rights\n");
    printf("\nPlease, enter your options: ");
}
 
void waitForChildren() {
 
    pid_t awaited_child;
    int status;
 
    awaited_child = wait(&status);
 
    while(awaited_child > 0) {
        if(WIFEXITED(status)) {
            printf("(Child process with PID %d has terminated normally with code %d).\n\n", awaited_child, WEXITSTATUS(status));
        }
        else{
            printf("(Child process with PID %d has terminated abnormally with code %d).\n\n", awaited_child, WEXITSTATUS(status));
        }
        awaited_child = wait(&status);
    }
 
}
 
void printAccessRights(struct stat st) {
 
    printf("\nAccess rights:\n");
 
    printf("\nUSER:\n-read: %s\n-write: %s\n-execute: %s\n", 
           (st.st_mode & S_IRUSR) ? "yes" : "no",
           (st.st_mode & S_IWUSR) ? "yes" : "no",
           (st.st_mode & S_IXUSR) ? "yes" : "no");
 
    printf("\nGROUP:\n-read: %s\n-write: %s\n-execute: %s\n", 
           (st.st_mode & S_IRGRP) ? "yes" : "no",
           (st.st_mode & S_IWGRP) ? "yes" : "no",
           (st.st_mode & S_IXGRP) ? "yes" : "no");
 
    printf("\nOTHERS:\n-read: %s\n-write: %s\n-execute: %s\n", 
           (st.st_mode & S_IROTH) ? "yes" : "no",
           (st.st_mode & S_IWOTH) ? "yes" : "no",
           (st.st_mode & S_IXOTH) ? "yes" : "no");
 
}
 
int countLines(char* filepath) {
 
    char file[1024];
    snprintf(file, sizeof(file), "wc -l %s", filepath);
 
    FILE *fp = popen(file, "r");
 
    if (!fp) {
        perror("Failed to execute file");
        return -1;
    }
 
    char output[1024];
 
    if (!fgets(output, sizeof(output), fp)) {
        perror("Failed to read file output");
        pclose(fp);
        return -1;
    }
 
    pclose(fp);
 
    char* p = strtok(output, " ");
    if (p == NULL) {
        fprintf(stderr, "Invalid file output: %s\n", output);
        return -1;
    }
 
    int lines = atoi(p);
    return lines;
 
}
 
void createNewTxtFile(char* dirpath, struct stat st) {
 
    pid_t pid;
 
    if((pid = fork()) < 0) {
        perror("Child process creation failed.\n");
        exit(EXIT_FAILURE);
    }
 
    if(pid == 0) {
        char filename[1024];
        snprintf(filename, sizeof(filename), "%s/%s_file.txt", dirpath, dirpath);
 
        int fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1) {
            perror("Error creating file.\n");
            exit(EXIT_FAILURE);
        }   
        close(fd);
 
        printf("\n------------------------------------\n");
        printf("\nThe provided argument is a directory. In addition, a corresponding txt file will be created:");
        printf("\nFile created: %s\n", filename);
 
        exit(EXIT_SUCCESS);
    }
 
    else if(pid > 0) {
        waitForChildren();
    }
 
}
 
void changePermissions(char* linkpath, struct stat st) {
 
    pid_t pid;
 
    if((pid = fork()) < 0) {
        perror("Child process creation failed.\n");
        exit(EXIT_FAILURE);
    }
 
    if(pid == 0) {
        int check;
        check = chmod(linkpath, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP);
 
        if(check == -1) {
            perror(strerror(errno));
            exit(errno);
        }
 
        printf("The new acces rights are:\n");
        printAccessRights(st);
 
        exit(EXIT_SUCCESS);
    }
 
    else if(pid > 0) {
        waitForChildren();
    }
 
}
 
void compileCFile(char* filepath, struct stat st){
 
    int pfd[2];
    pid_t pid;
    char string[1024];
    int num_warnings = 0, num_errors = 0;
    int hasCExtension = strstr(filepath, ".c") != NULL;
 
 
    if (pipe(pfd) < 0) {
        perror("Pipe creation failed.\n");
        exit(EXIT_FAILURE);
    }
 
    if ((pid = fork()) < 0) {
        perror("Child process creation failed.\n");
        exit(EXIT_FAILURE);
    }
 
    if (pid == 0) {
        if (hasCExtension) {
            close(pfd[0]); 
            dup2(pfd[1], 1); 
            execlp("./script.sh", "./script.sh", filepath, NULL);
            perror("Error calling exec.\n");
            exit(EXIT_FAILURE);
        }
        else {
            printf("The number of lines in this file is: %d.\n", countLines(filepath));
        }
    }
 
    else if (pid > 0) {
 
        close(pfd[1]); 
        FILE *stream = fdopen(pfd[0], "r");
 
        while (fscanf(stream, "%s", string) == 1) {
            if (strcmp(string, "warning:") == 0) {
                num_warnings++;
            }
            if (strcmp(string, "error:") == 0){
                num_errors++;
            }
        }
 
        close(pfd[0]); 
        int status;
        waitpid(pid, &status, 0);
        waitForChildren();
 
        int score = 0;
        char filename[1024];
        snprintf(filename, sizeof(filename), "%s", filepath);
 
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0) {
                if(num_errors == 0 && num_warnings == 0){
                    score = 10;
                }
                if(num_errors >=1){
                    score = 1;
                }
                if(num_errors == 0 && num_warnings > 10){
                    score = 2;
                }
                if(num_errors == 0 && num_warnings <= 10){
                    score = 2 + 8 * (10 - num_warnings) / 10;
                }
            }
        }
 
        int fd = open("grades.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 
        if (fd == -1) {
            perror("Error opening file.\n");
            exit(EXIT_FAILURE);
        }
 
        dprintf(fd, "%s: %d\n", filename, score);
        close(fd);
    }
 
}
 
void printRegularFileInfo(char *filepath) {
 
    struct stat st;
 
    char options[20];
    int numberOfOptions, flag;
 
    do {
        if(scanf("%20s", options) != 1) {
            perror("Scanf failed.\n");
        }
 
        flag = 1;
        numberOfOptions = strlen(options);
 
        if (options[0] != '-' || numberOfOptions < 2) {
            printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
            menu_RegularFiles();
            flag = 0;
        }
        else {
            for (int i = 1; i < numberOfOptions; i++) {
                if (!strchr("nmahdl", options[i])) {
                    printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
                    flag = 0;
                    menu_RegularFiles();
                    break;
                }
            }
        }
    }while(!flag);
 
    if(lstat(filepath, &st) == -1) {
        printf("Error: %s\n", strerror(errno));
        return;
    }
 
    for(int i = 1; i < numberOfOptions; i++) {
 
        switch (options[i]) {
 
            case 'n':
                printf("\nName: %s\n", filepath);
                break;
 
            case 'm':
                printf("\nTime of last modification: %s", ctime(&st.st_mtime));
                break;
 
            case 'a':
                printAccessRights(st);
                break;
 
            case 'h':
                printf("\nHard link count: %ld\n", st.st_nlink);
                break;
 
            case 'd':
                printf("\nSize: %ld bytes\n", st.st_size);
                break;
 
            case 'l':
                printf("\nEnter name of symbolic link: ");
                char linkname[256];
                scanf("%s", linkname);
 
                if (symlink(filepath, linkname) == -1) {
                    printf("\nError creating symbolic link: %s\n", strerror(errno));
                } 
                else {
                    printf("\nSymbolic link created: %s -> %s\n", linkname, filepath);
                }
 
            default:
                break;
        }
    }
    compileCFile(filepath, st);
}
 
void printSymbolicLinkInfo(char *linkpath) {
 
    struct stat st;
    struct stat targetstat;
 
    char options[20];
    int numberOfOptions, flag;
 
    do {
        if(scanf("%20s", options) != 1) {
            perror("Scanf failed.\n");
        }
        flag = 1;
        numberOfOptions = strlen(options);
 
        if (options[0] != '-' || numberOfOptions < 2) {
            printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
            menu_SymbolicLink();
            flag = 0;
        }
        else {
            for (int i = 1; i < numberOfOptions; i++) {
                if (!strchr("ndtal", options[i])) {
                    printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
                    flag = 0;
                    menu_SymbolicLink();
                    break;
                }
            }
        }
    }while(!flag);
 
    if(lstat(linkpath, &st) == -1) {
        printf("Error: %s\n", strerror(errno));
        return;
    }
 
    for(int i = 1; i < numberOfOptions; i++) {
 
        switch (options[i]) {
 
        case 'n':
            printf("\nName: %s\n", linkpath);
            break;
 
        case 'd':
            printf("\nSize of symbolic link: %ld bytes\n", st.st_size);
            break;
 
        case 't':
            if(stat(linkpath, &targetstat) == -1) {
                printf("Error: %s\n", strerror(errno));
                return;
            }
            if(S_ISREG(targetstat.st_mode)) {
                printf("\nSize of target file: %ld bytes\n", targetstat.st_size);
            } else {
                printf("\nTarget file is not a regular file.\n");
            }
            break;
 
        case 'a':
            printAccessRights(st);
            break;
 
        case 'l':
            if(unlink(linkpath) == -1) {
                printf("\nError: %s\n", strerror(errno));
            } 
            else {
                printf("\nSymbolic link deleted. Other following options cannot be provided.\n");
                return;
            }
            break;
 
        default:
            break;
        }
    }
    changePermissions(linkpath, st);
}
 
void printDirectoryInfo(char *dirpath) {
 
    char options[20];
    int numberOfOptions, flag;
    struct stat st;
 
    do {
        if(scanf("%20s", options) != 1) {
            perror("Scanf failed.\n");
        }
        flag = 1;
        numberOfOptions = strlen(options);
 
        if (options[0] != '-' || numberOfOptions < 2) {
            printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
            menu_Directory();
            flag = 0;
        }
        else {
            for (int i = 1; i < numberOfOptions; i++) {
                if (!strchr("ndac", options[i])) {
                    printf("The input you provided is not valid! Please give '-', followed by a string consisting of options. Ex: -nd\n");
                    flag = 0;
                    menu_Directory();
                    break;
                }
            }
        }
    }while(!flag);
 
    DIR *dir = opendir(dirpath);
 
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
 
    for(int i = 1; i < numberOfOptions; i++) {
 
        switch (options[i]) {
 
            case 'n':
                printf("\nName: %s\n", dirpath);
                break;
 
            case 'd':
                if (stat(dirpath, &st) == -1) {
                    perror("Error getting file st");
                    return;
                }
                printf("\nSize: %ld bytes\n", st.st_size);
                break;
 
            case 'a':
                printAccessRights(st);
                break;
 
            case 'c': ;
                int count = 0;
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    char path[1024];
                    snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
                    if (stat(path, &st) == -1) {
                        perror("Error getting file st");
                        return;
                    }
                    if (S_ISREG(st.st_mode) && strstr(entry->d_name, ".c") != NULL) {
                        count++;
                    }
                }
                printf("\nTotal number of files with .c extension: %d\n", count);
                break;
 
            default:
                break;
        }
    }
    createNewTxtFile(dirpath, st);
}
 
void printArgumentsInfo(char *path) {
 
    struct stat st;
 
    printf("\n%s", path);
 
    if(lstat(path, &st) == -1) {
        printf("Error: %s\n", strerror(errno));
        return;
    }
 
    switch (st.st_mode & S_IFMT) {
 
        case S_IFREG:
            printf("- REGULAR FILE\n");
            menu_RegularFiles();
            printRegularFileInfo(path);
            break;
 
        case S_IFDIR:
            printf(" - DIRECTORY\n");
            menu_Directory();
            printDirectoryInfo(path);
            break;
 
        case S_IFLNK:
            printf(" - SYMBOLIC LINK\n");
            menu_SymbolicLink();
            printSymbolicLinkInfo(path);
            break;
 
        default:
            printf("File type not supported.\n");
            break;
 
    }
 
}
 
 
int main(int argc, char *argv[]) {
 
    checkArguments(argc);
 
    for(int i = 1; i < argc; i++) {
 
        pid_t pid = fork();
 
        if(pid < 0) {
            perror("Fork failed.");
            exit(EXIT_FAILURE);
        } 
 
        else if(pid == 0) {
            printArgumentsInfo(argv[i]);
            printf("\n");
            exit(EXIT_SUCCESS);
        }
 
        else if(pid > 0) {
            waitForChildren();
            //waitpid(pid, &status, 0);
        }
 
    }
 
    return 0;    
 
}