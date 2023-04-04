#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void printFileMenu() {
    printf("----FILE-MENU----\n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("\th - hard link count\n");
    printf("\tm - time of last modification\n");
    printf("\ta - access rights\n");
    printf("\tl - create symbolic link\n");
    printf("Please enter your options.\n");
}

void printDirectoryMenu() {
    printf("----DIRECTORY-MENU----\n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("Please enter your options.\n");
}

void printLinkMenu() {
    printf("----LINK-MENU----\n");
    printf("\tn - name\n");
    printf("\tl - delete\n");
    printf("\td - size\n");
    printf("\tt - size of target file\n");
    printf("\ta - access rights\n");
    printf("Please enter your options.\n");
}

void printAccessRights(struct stat status, char *filePath) {
    printf("\nUser:\n");
    printf("\tRead - %s\n", (status.st_mode & S_IRUSR) ? "yes" : "no");
    printf("\tWrite - %s\n", (status.st_mode & S_IWUSR) ? "yes" : "no");
    printf("\tExecute - %s\n\n", (status.st_mode & S_IXUSR) ? "yes" : "no");

    printf("Group:\n");
    printf("\tRead - %s\n", (status.st_mode & S_IRGRP) ? "yes" : "no");
    printf("\tWrite - %s\n", (status.st_mode & S_IWGRP) ? "yes" : "no");
    printf("\tExecute - %s\n\n", (status.st_mode & S_IXGRP) ? "yes" : "no");

    printf("Others:\n");
    printf("\tRead - %s\n", (status.st_mode & S_IROTH) ? "yes" : "no");
    printf("\tWrite - %s\n", (status.st_mode & S_IWOTH) ? "yes" : "no");
    printf("\tExecute - %s\n\n", (status.st_mode & S_IXOTH) ? "yes" : "no");
}

void processOptions(struct stat status, char *filePath) {

    char options[20];

    if(scanf("%20s", options) != 1) {
        perror("Scanf failed.\n");
    }

    int nrOfOptions = strlen(options);

    //we will start iterating through the options starting from 1, since the first character inputed must be '-'
    for(int i = 1; i < nrOfOptions; i++) {
        switch (options[i])
        {
        case 'n':
            printf("Name of file: %s\n", filePath);
            break;


        case 'd':
            printf("File size: %ld bytes\n", status.st_size);
            break;


        case 'h':
            printf("Hard link count: %ld\n", status.st_nlink);


            break;
        case 'm':
            printf("Time of last modification: %s\n", ctime(&status.st_mtime));
            break;


        case 'a':
            printf("Access rights:\n");
            printAccessRights(status, filePath);
            break;


        case 'l':
            printf("Please enter the link name: ");

            char linkname[256];
            if(scanf("%s", linkname) != 1) {
                perror("Scanf failed.\n");
            }

            if (symlink(filePath, linkname) == -1) {
                perror("Failed to create the symbolic link.\n");
            } else {
                printf("\nSymbolic link was created: from %s to %s\n", linkname, filePath);
            }

            break;
        default:
            break;
        }
    }
}


int main(int argc, char **argv) {

    if(argc == 1) {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    for(int i = 1; i < argc; i++) {

        struct stat status;
        lstat(argv[i], &status);
        
        if (S_ISREG(status.st_mode)) {
            printf("\n%s - REGULAR FILE\n", argv[i]);

            printFileMenu();
            processOptions(status, argv[i]);

        } else if (S_ISDIR(status.st_mode)) {
            printf("\n%s - DIRECTORY\n", argv[i]);

            printDirectoryMenu();
            //processOptions();

        } else if (S_ISLNK(status.st_mode)) {
            printf("\n%s - SYMBOLIC LINK.\n", argv[i]);

            printLinkMenu();
            //processOptions();

        } else {
            printf("\n%s - UNKNOWN.\n", argv[i]);
        }
    }

    printf("\n");

    return 0;
}