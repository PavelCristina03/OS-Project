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

void printLineCount(char* filepath)
{
    printf("The number of lines: ");
    execlp("wc", "wc", "-l", filepath, NULL); // execute wc command
    perror("execlp fauiled.\n"); 
    exit(EXIT_FAILURE); // if this part is executed at all it means exec failed
}

void compileCFile(char* filepath)
{
    execlp("bash", "bash", "compileC.sh", filepath, NULL);
    perror("execlp failed.\n");
    exit(EXIT_FAILURE); //if this line is reached execlp failed
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

void handleProcesses(struct stat status, char *filepath, int pidProcesses)
{

    int pfd[2];

    if(pipe(pfd) < 0) {
        perror("Pipe creation failed.\n");
        exit(1);
    }

    if (S_ISREG(status.st_mode))
    {

        if(pidProcesses == 0) 
        {
            //child process for regular files will write to pipe
            if(strstr(filepath, ".c")) 
            {
                //c file => run a script which counts the number of errors and the number of warnings
                close(pfd[0]); //close read end of pipe
                dup2(pfd[1], STDOUT_FILENO); //redirect stdout to wrtie end of pipe
                compileCFile(filepath);
                close(pfd[1]); //close write end when done
            }
            else
            {
                //non c file => count the number of lines
                printLineCount(filepath);
            }

        }
        else if(pidProcesses > 0 && strstr(filepath, ".c")) 
        {
            //parent process will read from pipe 
            //c file => compute a score based on the number of errors and the number of warnings and print it to grades.txt
            close(pfd[1]); //close write end of pipe
            FILE* pipeStream = fdopen(pfd[0],"r"); //read from pipe using fdopen

            int errors = 0, warnings = 0;
            
            fscanf(pipeStream, "%d %d", &errors, &warnings);
            printf("Errors: %d\nWarnings: %d\n", errors, warnings);

            int score = computeScore(errors, warnings);
            char scoreBuffer[20] = "";
            int result = snprintf(scoreBuffer, sizeof(scoreBuffer), "%s - %d\n", filepath, score);

            if (result < 0) {
                perror("snprintf failed.\n");
                return 1;
            }

            int fdScore = open("grades.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                    if (fdScore == -1) {
                        perror("open failed.\n");
                        exit(EXIT_FAILURE);
                    }
                    if (write(fdScore, scoreBuffer, strlen(scoreBuffer)) == -1) {
                        perror("write failed.\n");
                        exit(EXIT_FAILURE);
                    }

            close(fdScore);

        }
        handleTerminatedChildren();

    }    
    else if (S_ISDIR(status.st_mode))
    {

        if(pidProcesses == 0) 
        {

        }
        else if(pidProcesses > 0) 
        {

        }
        
    }
    else if (S_ISLNK(status.st_mode))
    {
        if(pidProcesses == 0) 
        {

        }
        else if(pidProcesses > 0) 
        {

        }

    }

}

void handleTerminatedChildren()
{
    int status;
    pid_t terminatedChild;

    terminatedChild = wait(&status); //wait for child process to terminate and get its exit status

    while(terminatedChild > 0)
    {   
        // loop until all child processes have terminated

        if(WIFEXITED(status))
        {      
            // check if the child process terminated normally
            printf("\nThe child process with PID %d has finished successfully with exit code %d.\n\n", terminatedChild, WEXITSTATUS(status));
        }
        else
        {                       
            // if the child process terminated abnormally
            printf("\nThe child process with PID %d has finished abnormally with exit code %d.\n\n", terminatedChild, WEXITSTATUS(status));
        }

        terminatedChild = wait(&status);  // wait for the next child process to terminate and get its PID and exit status
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

        int pidOptions = fork();
        if(pidOptions < 0) 
        {
            perror("\nOption fork failed.\n");
            exit(EXIT_FAILURE);
        }

        else if(pidOptions == 0) 
        {
            processArguments(status, argv[i]);
            exit(EXIT_SUCCESS);
        }

        // this check might be unnecessary as the 1st child process has terminated by the time this code is reached
        else if (pidOptions > 0) 
        {
            int pidProcesses = fork();
            if(pidProcesses < 0) 
            {
                perror("Process fork failed.\n");
                exit(EXIT_FAILURE);
            }
            handleProcesses(status, argv[i], pidProcesses);
        }

    }
    /*

    */

    printf("\n");

    return 0;
}