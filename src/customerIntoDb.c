#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int parseDate(const char* dateString, char* outputDate) {
    struct tm timeStruct = {0};

    // List of possible date formats
    const char* formats[] = {
        "%m/%d/%Y",   // MM/DD/YYYY
        "%m/%d/%y",   // MM/DD/YY
        "%m-%d-%Y",   // MM-DD-YYYY
        "%m-%d-%y"    // MM-DD-YY
    };

    // Check if dateString is empty
    if (strlen(dateString) == 0 || strcmp(dateString, "None") == 0) {
        strcpy(outputDate, "0000-01-01");
        return 0;
    }

    for (int i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
        if (strptime(dateString, formats[i], &timeStruct) != NULL) {
            // Convert to YYYY-MM-DD format
            strftime(outputDate, 11, "%Y-%m-%d", &timeStruct);
            return 0;
        }
    }

    // If all parsing attempts fail, return an error
    return -1;
}

// Remove quotes from a string
void trimQuotes(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[0] == '\"' && str[len - 1] == '\"') {
        str[len - 1] = '\0';
        memmove(str, str + 1, len - 2);
    }
}

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

        // Initialize all fields to a default value
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

        // Parse each field separated by commas
        while (*currentChar != '\0' && fieldIndex < 9) {
            char *start = currentChar;
            while (*currentChar != ',' && *currentChar != '\n' && *currentChar != '\0') {
                currentChar++;
            }

            if (currentChar - start > 0) {
                strncpy(fields[fieldIndex], start, currentChar - start);
                fields[fieldIndex][currentChar - start] = '\0';
                trimQuotes(fields[fieldIndex]);
            }

            if (*currentChar == ',') {
                currentChar++;
            }
            fieldIndex++;
        }

        // Check if username and password are empty
        if (strcmp(username, "NA") == 0 || strcmp(password, "NA") == 0 || strlen(username) == 0 || strlen(password) == 0) {
            printf("Le nom d'utilisateur et le mot de passe ne peuvent pas être vides.\n");
            continue;
        }

        // Parse dates using parseDate function
        if (parseDate(dateOfPurchase, dateOfPurchase) != 0) {
            printf("Échec de la conversion de la date de purchase pour \"%s\". Valeur de date: \"%s\"\n", username, dateOfPurchase);
        }

        if (parseDate(dateEndSupport, dateEndSupport) != 0) {
            printf("Échec de la conversion de la date de support pour \"%s\". Valeur de date: \"%s\"\n", username, dateEndSupport);
        }

        // Construct the command
        char command[1000];
        snprintf(command, sizeof(command), "../bin/addUser.exe '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'",
                 username, password, customer, endUser, project, region, dateOfPurchase, dateEndSupport, invoiceNumber);
        system(command);
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
