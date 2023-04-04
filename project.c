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

void processFileOptions(struct stat status, char *filePath) {

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
            printf("Access rights:");
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

void processLinkOptions(struct stat status, char *filePath) {

    char options[20];

    do{
        printf("The options should be preceded by a '-'\n");
        scanf("%20s", options);
    }while(options[0] != '-');

    int nrOfOptions = strlen(options);

    int deleted = 0;

    //we will start iterating through the options starting from 1, since the first character inputed must be '-'
    for(int i = 1; i < nrOfOptions && !deleted; i++) {
        switch (options[i])
        {
        case 'n':
            printf("Name of the link: %s\n", filePath);
            break;


        case 'd':
            printf("Link size: %ld bytes\n", status.st_size);
            break;


        case 't': ;
        
            struct stat targetSt;
            //we use stat instead of lstat to get the target file's size
            if(stat(filePath, &targetSt) == -1) {
                perror("Stat failed.\n");
                break;
            }

            printf("Target file size: %ld bytes\n", targetSt.st_size);

            break;

        case 'a':
            printf("Access rights:");
            printAccessRights(status, filePath);
            break;


        case 'l':
            if(unlink(filePath) == -1) {
                perror("Failed to remove the link.\n");
                break;
            }

            printf("Link removed successfully.\n");

            deleted = 1;
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
        if(lstat(argv[i], &status) == -1) {
            perror("lstat failed.\n");
        }
        
        if (S_ISREG(status.st_mode)) {
            printf("\n%s - REGULAR FILE\n", argv[i]);

            printFileMenu();
            processFileOptions(status, argv[i]);

        } else if (S_ISDIR(status.st_mode)) {
            printf("\n%s - DIRECTORY\n", argv[i]);

            printDirectoryMenu();
            //processMenuOptions();

        } else if (S_ISLNK(status.st_mode)) {
            printf("\n%s - SYMBOLIC LINK.\n", argv[i]);

            printLinkMenu();
            processLinkOptions(status, argv[i]);

        } else {
            printf("\n%s - UNKNOWN.\n", argv[i]);
        }
    }

    printf("\n");

    return 0;
}