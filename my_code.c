#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Setting up the shared memory space for the processes.
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0)
    {
        printf("Fatal Error: Setting up shared memory space!\n");
        exit(1);
    }

    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1)
    {
        printf("Fatal Error: Setting up shared memory space!\n");
        exit(1);
    }

    ShmPTR[0] = 0; // Let 0 be the Bank Account
    ShmPTR[1] = 0; // Let 1 be the Turn

    // Successful setting up of shared memory space
    printf("Orig Bank Account = %d\n", ShmPTR[0]);

    // Preparing processes
    pid = fork();

    if (pid < 0)
    {
        printf("Error forking process!\n");
        exit(1);
    }
    // This is the child process (CHILD withdraws money)
    else if (pid == 0)
    {
        int bankBalance = 0;
        int moneyToWithdraw = 0;
        for (int i = 0; i < 25; i++)
        {
            sleep((rand() % 5) + 1);
            while (ShmPTR[1] != 1)
            {
                // do nothing
            }
            bankBalance = ShmPTR[0];
            moneyToWithdraw = rand() % 51;
            printf("Poor Student needs $%d\n", moneyToWithdraw);
            if (moneyToWithdraw <= bankBalance)
            {
                bankBalance = bankBalance - moneyToWithdraw;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", moneyToWithdraw, bankBalance);
                ShmPTR[0] = bankBalance;
            }
            else
            {
                printf("Poor Student: Not Enough Cash ($%d)\n", bankBalance);
            }
            ShmPTR[1] = 0;
        }
        exit(0); // Hand over access to parent
    }

    // This is the parent process (DAD to deposit money)
    else
    {
        int bankBalance = 0;
        for (int i = 0; i < 25; i++)
        {
            sleep((rand() % 5) + 1);
            bankBalance = ShmPTR[0];
            while (ShmPTR[1] != 0)
            {
                // do nothing
            }
            int moneyToDeposit = 0;
            if (bankBalance <= 100)
            {
                moneyToDeposit = rand() % 101;

                if (moneyToDeposit % 2 == 1)
                {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
                else
                {
                    bankBalance = bankBalance + moneyToDeposit;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", moneyToDeposit, bankBalance);
                    ShmPTR[0] = bankBalance;
                }
            }
            else
            {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", bankBalance);
            }
            // Hand over control to child.
            ShmPTR[1] = 1;
        }
    }

    wait(&status);
    printf("Server has detected the completion of its child...\n");
    shmdt((void *)ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0); // kill program after done
}