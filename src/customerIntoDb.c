#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int userIntoDb(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Échec de l'ouverture du fichier.\n");
        return -1;
    }

    char buffer[1000];  // Buffer to read each line
    fgets(buffer, sizeof(buffer), file);  // Read and discard the first line

    char username[100];
    char password[100];
    char customer[100];
    char endUser[100];
    char project[100];
    char region[100];
    char dateOfPurchase[100];
    char dateEndSupport[100];
    char invoiceNumber[100];

    while (fgets(buffer, sizeof(buffer), file)) {

        strcpy(username, "NA");
        strcpy(password, "NA");
        strcpy(customer, "NA");
        strcpy(endUser, "NA");
        strcpy(project, "NA");
        strcpy(region, "NA");
        strcpy(dateOfPurchase, "0000-01-01");
        strcpy(dateEndSupport, "0000-01-01");
        strcpy(invoiceNumber, "NA");

        char *currentChar = buffer;
        char *fields[] = {username, password, customer, endUser, project, region, dateOfPurchase, dateEndSupport, invoiceNumber};
        int fieldIndex = 0;

        while (*currentChar != '\0' && fieldIndex < 9) {
            char *start = currentChar;
            while (*currentChar != ',' && *currentChar != '\n' && *currentChar != '\0') {
                currentChar++;
            }

            if (currentChar - start > 0) {
                strncpy(fields[fieldIndex], start, currentChar - start);
                fields[fieldIndex][currentChar - start] = '\0';
            }

            if (*currentChar == ',') {
                currentChar++;
            }
            fieldIndex++;
        }

        if (strcmp(username, "NA") == 0 || strcmp(password, "NA") == 0) {
            printf("Le nom d'utilisateur et le mot de passe ne peuvent pas être vides.\n");
            continue;
        } else {
            if (strcmp(customer, "None") == 0) {
                strcpy(dateOfPurchase, "0000-01-01");
            } else if (strcmp(dateOfPurchase, "0000-01-01") != 0) {
                struct tm purchaseTime = {0};
                strptime(dateOfPurchase, "%m/%d/%Y", &purchaseTime);
                strftime(dateOfPurchase, sizeof(dateOfPurchase), "%Y-%m-%d", &purchaseTime);
            }
            if (strcmp(dateEndSupport, "None") == 0) {
                strcpy(dateEndSupport, "0000-01-01");
            } else if (strcmp(dateEndSupport, "0000-01-01") != 0) {
                struct tm endSupportTime = {0};
                strptime(dateEndSupport, "%m/%d/%Y", &endSupportTime);
                strftime(dateEndSupport, sizeof(dateEndSupport), "%Y-%m-%d", &endSupportTime);
            }

            char command[1000];
            snprintf(command, sizeof(command), "../bin/addUser.exe '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'",
                     username, password, customer, endUser, project, region, dateOfPurchase, dateEndSupport, invoiceNumber);
            system(command);
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <csv_file_path>\n", argv[0]);
        return -1;
    }

    return userIntoDb(argv[1]);
}
