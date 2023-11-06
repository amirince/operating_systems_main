// C program to demonstrate use of fork() and pipe()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send concatenated string from child

    int fd1[2]; // Used to store two ends of first pipe  Parent_To_Child
    int fd2[2]; // Used to store two ends of second pipe

    char fixed_str[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100];
    pid_t p;

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("Enter a string to concatenate:");
    scanf("%s", input_str);
    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process
    else if (p > 0)
    {
        // this pipe is for writing to the child process, no need for parent
        // to read from it.
        close(fd1[0]); // Close reading end of fd1
        write(fd1[1], input_str, strlen(input_str) + 1);

        // Wait for child to send a string
        wait(NULL);

        //  After child process and writing new string to fd2
        char concat_str[100];
        read(fd2[0], concat_str, 100);

        int k = strlen(concat_str);
        int i;
        for (i = 0; i < strlen(fixed_str2); i++)
            concat_str[k++] = fixed_str2[i];

        concat_str[k] = '\0'; // string ends with '\0'

        printf("Concatenated string %s\n", concat_str);

        close(fd1[1]); // Close writing end of pipes
        close(fd2[1]); // Close p2
    }

    // child process
    else
    {
        // Child process only reads from fd1, this is how the parent
        // send data to the child.
        close(fd1[1]); // Close writing end of first pipes

        char concat_str[100];
        read(fd1[0], concat_str, 100); // reading data in p1 from parent proc

        // Concatenate a fixed string with it
        int k = strlen(concat_str);
        int i;
        for (i = 0; i < strlen(fixed_str); i++)
            concat_str[k++] = fixed_str[i];

        concat_str[k] = '\0'; // string ends with '\0'

        printf("Concatenated string %s\n", concat_str);
        // Close both reading ends in the child
        close(fd1[0]);
        close(fd2[0]);

        // We can close reading end of fd2 in child since
        // child only write to fd2, this is how the child sends data to the
        // parent

        printf("Enter a string to concatenate:");
        scanf("%s", input_str);
        // Write to fd2, this is the pipe used to send data from child to parent.
        write(fd2[1], input_str, strlen(input_str) + 1); // Write string to fd2
        exit(0);                                         // exit child.
    }
}