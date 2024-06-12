/**
 * @file modifyUser.c
 * @brief Program to modify the contract end date of a user in a MySQL database.
 * 
 * This program takes a username and a new contract end date as arguments,
 * checks if the user exists in the database, updates the user's contract end date,
 * and adjusts the user's group based on the new contract end date.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>

/**
 * @brief Prints an error message and exits the program.
 * @author Tanguy Soutric
 * 
 * @param conn The MySQL connection.
 */
void exit_error(MYSQL *conn) {
    fprintf(stderr, "Error: %s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

/**
 * @brief Modifies the contract end date of a user in the database.
 * @author Tanguy Soutric
 * 
 * Checks if the user exists in the database. If the user exists, it updates the user's contract end date.
 * Based on the new date, it updates the user's group to "licenseExpired" or "licenseActive".
 * 
 * @param conn The MySQL connection.
 * @param username The username.
 * @param new_date The new contract end date.
 */
void modify_user(MYSQL *conn, const char *username, const char *new_date) {
    char query[1024];

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM users WHERE username = '%s'", username);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int user_exists = atoi(row[0]);
    mysql_free_result(result);

    if (!user_exists) {
        printf("User %s does not exist in the database.\n", username);
        return;
    }

    snprintf(query, sizeof(query), "UPDATE users SET dateFinContrat = '%s' WHERE username = '%s'", new_date, username);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char current_date[11];
    strftime(current_date, 11, "%Y-%m-%d", local);
    const char *groupname = (strcmp(new_date, current_date) < 0) ? "licenseExpired" : "licenseActive";
    
    snprintf(query, sizeof(query), "UPDATE usergroups SET groupname = '%s' WHERE username = '%s'", groupname, username);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    printf("%s updated with new contract end date %s and group %s.\n", username, new_date, groupname);
}

/**
 * @brief The main entry point of the program.
 * @author Tanguy Soutric
 * 
 * @param argc The number of arguments.
 * @param argv The arguments passed to the program.
 * @return int The return code of the program.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <username> <new_date_fin_contrat>\n", argv[0]);
        exit(1);
    }

    const char *username = argv[1];
    const char *new_date = argv[2];

    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(conn, "localhost", "capsule", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        exit_error(conn);
    }

    modify_user(conn, username, new_date);

    mysql_close(conn);

    return 0;
}
