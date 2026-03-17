// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// clientData structure definition
void displayRecords(FILE *fPtr);
void searchAccount(FILE *fPtr);
void transferMoney(FILE *fPtr);
void initializeFile(void);
int login(void);

struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    if(!login())
    {
        fclose(cfPtr);   // good practice
        return 0;
    }
    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        // create text file from record file
        case 1:
            textFile(cfPtr);
            break;
        // update record
        case 2:
            updateRecord(cfPtr);
            break;
        // create record
        case 3:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 4:
            deleteRecord(cfPtr);
            break;
        // display if user does not select valid choice
        case 5:
        displayRecords(cfPtr);
        break;

         case 6:
         searchAccount(cfPtr);
         break;

        case 7:
        transferMoney(cfPtr);
        break;

        case 8:
        initializeFile();
        fclose(cfPtr);
        cfPtr = fopen("credit.dat","rb+");

        if(cfPtr == NULL){
        printf("Error reopening file!\n");
        exit(1);
        }
        break;

        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("%s", "Enter account to update ( 1 - 100 ): ");
    scanf("%d", &account);

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        scanf("%lf", &transaction);
        client.balance += transaction; // update record balance

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("%s", "Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    // display available options
    printf("%s", "\nEnter your choice\n"
             "1 - store formatted text file\n"
             "2 - update an account\n"
             "3 - add a new account\n"
             "4 - delete an account\n"
             "5 - display all accounts\n"
             "6 - search account\n"
             "7 - transfer money\n"
             "8 - initialize file\n"
             "9 - end program\n? ");

    scanf("%u", &menuChoice); // receive choice from user
    return menuChoice;
} // end function enterChoice

void displayRecords(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n","Acct","Last Name","First Name","Balance");

    while(fread(&client,sizeof(struct clientData),1,fPtr)==1)
    {
        if(client.acctNum!=0)
        {
            printf("%-6d%-16s%-11s%10.2f\n",
            client.acctNum,
            client.lastName,
            client.firstName,
            client.balance);
        }
    }
}

void searchAccount(FILE *fPtr)
{
    unsigned int account;
    struct clientData client;

    printf("Enter account number to search: ");
    scanf("%u",&account);

    fseek(fPtr,(account-1)*sizeof(struct clientData),SEEK_SET);
    fread(&client,sizeof(struct clientData),1,fPtr);

    if(client.acctNum==0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found:\n");
        printf("%-6d%-16s%-11s%10.2f\n",
        client.acctNum,
        client.lastName,
        client.firstName,
        client.balance);
    }
}

void transferMoney(FILE *fPtr)
{
    unsigned int fromAcc,toAcc;
    double amount;

    struct clientData fromClient,toClient;

    printf("Enter source account: ");
    scanf("%u",&fromAcc);

    printf("Enter destination account: ");
    scanf("%u",&toAcc);

    printf("Enter amount to transfer: ");
    scanf("%lf",&amount);

    // Read source account
    fseek(fPtr,(fromAcc-1)*sizeof(struct clientData),SEEK_SET);
    fread(&fromClient,sizeof(struct clientData),1,fPtr);

    // Read destination account
    fseek(fPtr,(toAcc-1)*sizeof(struct clientData),SEEK_SET);
    fread(&toClient,sizeof(struct clientData),1,fPtr);

    if(fromClient.acctNum==0 || toClient.acctNum==0)
    {
        printf("One of the accounts does not exist.\n");
        return;
    }

    if(fromClient.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    // Perform transfer
    fromClient.balance -= amount;
    toClient.balance += amount;

    // Write back source
    fseek(fPtr,(fromAcc-1)*sizeof(struct clientData),SEEK_SET);
    fwrite(&fromClient,sizeof(struct clientData),1,fPtr);

    // Write back destination
    fseek(fPtr,(toAcc-1)*sizeof(struct clientData),SEEK_SET);
    fwrite(&toClient,sizeof(struct clientData),1,fPtr);

    printf("Transfer successful!\n");
}

void initializeFile(void)
{
    FILE *fp = fopen("credit.dat","wb");

    struct clientData blank = {0,"","",0.0};

    for(int i=0;i<100;i++)
    {
        fwrite(&blank,sizeof(struct clientData),1,fp);
    }

    fclose(fp);

    printf("File initialized successfully.\n");
}

int login(void)
{
    char password[20];

    printf("Enter system password: ");
    scanf("%s", password);

    if(strcmp(password, "admin123") == 0)
    {
        printf("Access granted!\n");
        return 1;
    }
    else
    {
        printf("Access denied!\n");
        return 0;
    }
}