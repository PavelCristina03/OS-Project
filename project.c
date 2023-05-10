#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

void printFileMenu()
{
    printf("----FILE-MENU----\n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("\th - hard link count\n");
    printf("\tm - time of last modification\n");
    printf("\ta - access rights\n");
    printf("\tl - create symbolic link\n");
    printf("Please enter your options.\n");
    printf("The options should be preceded by a '-'\n");
}

void printDirectoryMenu()
{
    printf("----DIRECTORY-MENU----\n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("\ta - access rights\n");
    printf("\tc - number of C file entries\n");
    printf("Please enter your options.\n");
    printf("The options should be preceded by a '-'\n");
}

void printLinkMenu()
{
    printf("----LINK-MENU----\n");
    printf("\tn - name\n");
    printf("\tl - delete\n");
    printf("\td - size\n");
    printf("\tt - size of target file\n");
    printf("\ta - access rights\n");
    printf("Please enter your options.\n");
    printf("The options should be preceded by a '-'\n");
}

void printAccessRights(struct stat status, char *filePath)
{
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

void processFileOptions(struct stat status, char *filePath)
{

    char options[20];
    int nrOfOptions;
    int isValid = 1;

    do
    {
        if (!isValid)
        {
            printFileMenu();
        }

        if (scanf("%20s", options) != 1)
        {
            perror("Scanf failed.\n");
        }
        isValid = 1;
        nrOfOptions = strlen(options);

        if (options[0] != '-' || nrOfOptions < 2)
        {
            printf("Please enter the correct options.\n");
            isValid = 0;
        }
        else
        {
            for (int i = 1; i < nrOfOptions; i++)
            {
                if (!strchr("ndhmal", options[i]))
                {
                    printf("Please enter the correct options.\n");
                    isValid = 0;
                    break;
                }
            }
        }

    } while (!isValid);

    // we will start iterating through the options starting from 1, since the first character inputed must be '-'
    for (int i = 1; i < nrOfOptions; i++)
    {
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
            if (scanf("%s", linkname) != 1)
            {
                perror("Scanf failed.\n");
            }

            if (symlink(filePath, linkname) == -1)
            {
                perror("Failed to create the symbolic link.\n");
            }
            else
            {
                printf("\nSymbolic link was created: from %s to %s\n", linkname, filePath);
            }

            break;

        default:
            break;
        }
    }
}

void processLinkOptions(struct stat status, char *filePath)
{

    char options[20];
    int nrOfOptions;
    int isValid = 1;
    do
    {
        if (!isValid)
        {
            printLinkMenu();
        }

        if (scanf("%20s", options) != 1)
        {
            perror("Scanf failed.\n");
        }
        isValid = 1;
        nrOfOptions = strlen(options);

        if (options[0] != '-' || nrOfOptions < 2)
        {
            printf("Please enter the correct options.\n");
            isValid = 0;
        }
        else
        {
            for (int i = 1; i < nrOfOptions; i++)
            {
                if (!strchr("ndtal", options[i]))
                {
                    printf("Please enter the correct options.\n");
                    isValid = 0;
                    break;
                }
            }
        }

    } while (!isValid);

    int deleted = 0;

    // we will start iterating through the options starting from 1, since the first character inputed must be '-'
    for (int i = 1; i < nrOfOptions && !deleted; i++)
    {
        switch (options[i])
        {
        case 'n':
            printf("Name of the link: %s\n", filePath);
            break;

        case 'd':
            printf("Link size: %ld bytes\n", status.st_size);
            break;

        case 't':;

            struct stat targetSt;
            // we use stat instead of lstat to get the target file's size
            if (stat(filePath, &targetSt) == -1)
            {
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
            if (unlink(filePath) == -1)
            {
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

void processDirectoryOptions(struct stat status, char *filePath)
{

    char options[20];

    int nrOfOptions;
    int isValid = 1;

    do
    {
        if (!isValid)
        {
            printDirectoryMenu();
        }

        if (scanf("%20s", options) != 1)
        {
            perror("Scanf failed.\n");
        }
        isValid = 1;
        nrOfOptions = strlen(options);

        if (options[0] != '-' || nrOfOptions < 2)
        {
            printf("Please enter the correct options.\n");
            isValid = 0;
        }
        else
        {
            for (int i = 1; i < nrOfOptions; i++)
            {
                if (!strchr("ndac", options[i]))
                {
                    printf("Please enter the correct options.\n");
                    isValid = 0;
                    break;
                }
            }
        }

    } while (!isValid);

    // we will start iterating through the options starting from 1, since the first character inputed must be '-'
    for (int i = 1; i < nrOfOptions; i++)
    {
        switch (options[i])
        {
        case 'n':
            printf("Name of file: %s\n", filePath);
            break;

        case 'd':
            printf("File size: %ld bytes\n", status.st_size);
            break;

        case 'a':
            printf("Access rights:");
            printAccessRights(status, filePath);
            break;

        case 'c':;

            DIR *directory = opendir(filePath);
            if (directory == NULL)
            {
                perror("Could not open the directory.\n");
                break;
            }

            int countC = 0;
            struct dirent *entry;

            while ((entry = readdir(directory)) != NULL)
            {

                struct stat entryStat;
                if (lstat(filePath, &entryStat) == -1)
                {
                    perror("lstat failed.\n");
                    break;
                }

                int entryNameLength = strlen(entry->d_name);

                if (entry->d_name[entryNameLength - 2] == '.' && entry->d_name[entryNameLength - 1] == 'c')
                {
                    countC++;
                }
            }
            printf("The number of C files in the directory is %d.\n", countC);

            break;

        default:
            break;
        }
    }
}

int computeScore(int errors, int warnings) 
{
    if(errors == 0 && warnings == 0) 
    {
        return 10;
    }
    else if(errors >= 1) 
    {
        return 1;
    }
    else if (errors == 0 && warnings > 10) 
    {
        return 2;
    }

    return 2 + 8 * (10 - warnings)/10;
    
}

void processArguments(struct stat status, char *filepath)
{

    int pid = fork();

    if (pid < 0)
    {
        perror("fork\n");
        exit(1);
    }

    if (S_ISREG(status.st_mode))
    {
        if (pid == 0)
        {
            // child process handles input
            printf("\n%s - REGULAR FILE\n", filepath);

            printFileMenu();
            processFileOptions(status, filepath);
            exit(0);
        }
        else
        {
            // parent process creates a 2nd child process

            int pfd[2];

            if(pipe(pfd) < 0) {
                perror("pipe\n");
                exit(1);
            }

            int pidc = fork();

            if(pidc < 0) {
                perror("fork\n");
                exit(1);
            }

            if (pidc == 0)
            {
                close(pfd[0]); //close read end of pipe, child will write
                dup2(pfd[1], STDOUT_FILENO); //duplicate write end of pipe to stdout
                close(pfd[1]); //close original write end of pipe

                if (strstr(filepath, ".c") != NULL)
                {
                    // if we have a .c regular file we execute a script
                    // which prints the number of errors and warnings
                    //printf("c ");
                    execl("/bin/bash", "bash", "filescript.sh", filepath, NULL);
                    perror("execl failed\n");
                    exit(1);
                }
                else
                {
                    // otherwise, count the number of lines

                    execlp("wc", "wc", "-l", NULL);
                    perror("execl failed\n");
                    exit(1);
                }

                exit(0);
            }
            else
            {
                // Parent process
                // Read output from pipe
                close(pfd[1]); // Close write end of pipe, parent will read

                int num1, num2, isCFile = 0;
                char buffer[20] = "";

                read(pfd[0], buffer, 20);
                //printf("buffer: %s\n", buffer);
                //sscanf(buffer, "%d", &isCFile);


                if (isCFile) 
                {
                    sscanf(buffer, "%d %d", &num1, &num2);
                    printf("Errors: %d\nWarnings: %d\n\n", num1, num2);
                    printf("The score is: %d", computeScore(num1, num2));
                }
                else
                {
                    sscanf(buffer, "%d", &num1);
                    printf("lines: %d\n\n", num1);
                }



                // Wait for child process to finish
            }
            wait(NULL);
        }
    }
    else if (S_ISDIR(status.st_mode))
    {
        if (pid == 0)
        {
            // child process handles input
            printf("\n%s - DIRECTORY\n", filepath);

            printDirectoryMenu();
            processDirectoryOptions(status, filepath);
            exit(0);
        }
        else
        {
            // parent process creates a second child process
            // which creates a new file with a formatted name
            int pidc = fork();
            if (pidc == 0)
            {
                char filename[1024];
                snprintf(filename, sizeof(filename), "%s_file.txt", filepath);
                execl("/bin/touch", "touch", filename, NULL);
                exit(0);
            }

            
        }
        
       wait(NULL);
    }
    else if (S_ISLNK(status.st_mode))
    {
        if (pid == 0)
        {
            // child process handles input
            printf("\n%s - SYMBOLIC LINK.\n", filepath);

            printLinkMenu();
            processLinkOptions(status, filepath);
            exit(0);
        }
        else
        {
        }
    }
    else
    {
        printf("\n%s - UNKNOWN.\n", filepath);
    }
    wait(NULL);
}

int main(int argc, char **argv)
{

    if (argc == 1)
    {
        printf("Not enough command line arguments.\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {

        struct stat status;
        if (lstat(argv[i], &status) == -1)
        {
            perror("lstat failed.\n");
        }

        processArguments(status, argv[i]);
    }
    /*

    */

    printf("\n");

    return 0;
}