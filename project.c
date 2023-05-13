#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <fcntl.h>

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


void handleTerminatedChildren()
{
    int status;
    pid_t terminatedChild;

    terminatedChild = wait(&status); 

    while (terminatedChild > 0)
    {
        // loop until all child processes have terminated

        if (WIFEXITED(status))
        {
            // check if the child process terminated normally
            printf("\nThe child process with PID %d has finished successfully with exit code %d.\n", terminatedChild, WEXITSTATUS(status));
        }
        else
        {
            // if the child process terminated abnormally
            printf("\nThe child process with PID %d has finished erroneously with exit code %d.\n", terminatedChild, WEXITSTATUS(status));
        }

        terminatedChild = wait(&status); // wait for the next child process to terminate
    }
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
    if (errors == 0 && warnings == 0)
    {
        return 10;
    }
    else if (errors >= 1)
    {
        return 1;
    }
    else if (errors == 0 && warnings > 10)
    {
        return 2;
    }

    return 2 + 8 * (10 - warnings) / 10;
}

void printLineCount(char *filepath)
{
    execlp("wc", "wc", "-l", filepath, NULL); // execute wc command
    perror("execlp failed.\n");
    exit(EXIT_FAILURE); // if this part is executed at all it means exec failed
}

void handleRegFile(struct stat st, char *filepath)
{

    //sleep(1);
    pid_t pidRegFile;
    int pfd[2];

    if (pipe(pfd) < 0)
    {
        perror("\nPipe creation failed.\n");
        exit(EXIT_FAILURE);
    }

    if ((pidRegFile = fork()) < 0)
    {
        perror("\nChild process creation failed.\n");
        exit(EXIT_FAILURE);
    }

    else if (pidRegFile == 0)
    {
        // child process => write to pipe
        close(pfd[0]); //close read end of pipe

        if (strstr(filepath, ".c"))
        {
            //c file => execute a script which compiles the C file and prints to stdout the nr of errors and the nr of warnings
            dup2(pfd[1], STDOUT_FILENO); //redirect stdout to write end of pipe

            execlp("bash", "bash", "compileC.sh", filepath, NULL);
            perror("execlp failed.\n"); //if this line is reached exec has failed to execute properly
            exit(EXIT_FAILURE);

        }

        else
        {
            printf("\nThe number of lines: ");
            printLineCount(filepath);

        }
        exit(EXIT_SUCCESS);
    }

    else if (pidRegFile > 0)
    {

        if (strstr(filepath, ".c"))
        {

            int errors = 0, warnings = 0, score = 0;

            close(pfd[1]);

            FILE *stream = fdopen(pfd[0], "r");
            ;

            fscanf(stream, "%d %d", &errors, &warnings);
            printf("Errors: %d\nWarnings: %d\n", errors, warnings);

            score = computeScore(errors, warnings);

            int fd = open("grades.txt", O_RDWR | O_APPEND | O_CREAT, S_IRWXU);

            if (fd == -1)
            {
                perror("Error opening file.\n");
                exit(EXIT_FAILURE);
            }

            char filename[1000], string[1024];

            snprintf(filename, sizeof(filename), "%s", filepath);
            sprintf(string, "%s : %d\n", filename, score);
            write(fd, string, strlen(string));
            close(fd);
            close(pfd[0]);
        }
        handleTerminatedChildren();
    }
}

void createTxtFile(struct stat st, char *filepath)
{

    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("\nfork failed.\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        char filename[1024];
        snprintf(filename, sizeof(filename), "%s/%s_file.txt", filepath, filepath); //format file name to dirname_file.txt
        execlp("touch", "touch", filename, NULL);
        perror("\nexeclp failed.\n");
        exit(EXIT_FAILURE);

    }

    else if (pid > 0)
    {
        handleTerminatedChildren();
    }
}

void changeLinkRights(struct stat status, char *link)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("\nfork failed.\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        execlp("chmod", "chmod", "u+rwx,g+rw-x,o-rwx", link, NULL);
        perror("\nexec failed.\n");
        exit(EXIT_FAILURE);

    }

    else if (pid > 0)
    {
        handleTerminatedChildren();
    }
}



void processArguments(struct stat status, char *filepath)
{

    if (S_ISREG(status.st_mode))
    {

        printf("\n%s - REGULAR FILE\n", filepath);
        printFileMenu();
        processFileOptions(status, filepath);
    }
    else if (S_ISDIR(status.st_mode))
    {

        printf("\n%s - DIRECTORY\n", filepath);
        printDirectoryMenu();
        processDirectoryOptions(status, filepath);
    }
    else if (S_ISLNK(status.st_mode))
    {

        printf("\n%s - SYMBOLIC LINK.\n", filepath);
        printLinkMenu();
        processLinkOptions(status, filepath);
    }
    else
    {
        printf("\n%s - UNKNOWN.\n", filepath);
    }
}

void handleProcesses(struct stat status, char *filepath)
{
    if (S_ISREG(status.st_mode))
    {

        handleRegFile(status, filepath);

    }
    else if (S_ISDIR(status.st_mode))
    {

        createTxtFile(status, filepath);

    }
    else if (S_ISLNK(status.st_mode))
    {

        changeLinkRights(status, filepath);

    }

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

        pid_t pid;

        if ((pid = fork()) < 0)
        {
            perror("\nFork failed.\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            processArguments(status, argv[i]);
            exit(0);
        }
        else if(pid > 0) 
        {
            //wait(NULL);
            handleProcesses(status, argv[i]);
            handleTerminatedChildren();
        }
    }
    /*

    */

    printf("\n");

    return 0;
}