/**
 * @file addUser.c
 * @brief Program to insert users into a MySQL database.
 * 
 * This program takes a username, a password, and a contract end date as arguments,
 * hashes the password using a Python script, and inserts the user into the MySQL database.
 * If the user already exists, a notification is displayed. 
 * Based on the contract end date, the user is placed in either an active or expired license group.
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
 * @brief Inserts a user into the database.
 * @author Tanguy Soutric
 * Checks if the user already exists. If not, it inserts the user
 * with the hashed password and contract end date. 
 * Depending on the contract end date, the user is added to the "licenseExpired" or "licenseActive" group.
 * 
 * @param conn The MySQL connection.
 * @param username The username.
 * @param hashed_password The hashed password.
 * @param date_fin_contrat The contract end date.
 */
void insert_user(MYSQL *conn, const char *username, const char *hashed_password, const char *date_fin_contrat) {
    char query[1024];
    MYSQL_RES *res;
    MYSQL_ROW row;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM users WHERE username = '%s'", username);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    int user_exists = atoi(row[0]);
    mysql_free_result(res);

    if (user_exists) {
        printf("User %s already exists in the database.\n", username);
        return;
    }

    snprintf(query, sizeof(query), "INSERT INTO users (username, pw, dateFinContrat) VALUES ('%s', '%s', '%s')", username, hashed_password, date_fin_contrat);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char current_date[11];
    strftime(current_date, 11, "%Y-%m-%d", local);
    const char *groupname = (strcmp(date_fin_contrat, current_date) < 0) ? "licenseExpired" : "licenseActive";

    snprintf(query, sizeof(query), "INSERT INTO usergroups (username, groupname) VALUES ('%s', '%s')", username, groupname);
    if (mysql_query(conn, query)) {
        exit_error(conn);
    }

    printf("%s inserted with hashed password and contract end date %s.\n", username, date_fin_contrat);
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
    if (argc != 4) {
        printf("Usage: %s <username> <password> <date_fin_contrat>\n", argv[0]);
        exit(1);
    }

    const char *username = argv[1];
    const char *password = argv[2];
    const char *date_fin_contrat = argv[3];

    char hashed_password[256];
    char command[1024];
    FILE *fp;

    snprintf(command, sizeof(command), "python3 ../src/python/hashPassword.py \"%s\"", password);
    fp = popen(command, "r");
    if (fp == NULL || fgets(hashed_password, sizeof(hashed_password), fp) == NULL) {
        fprintf(stderr, "Error calling Python script\n");
        exit(1);
    }
    pclose(fp);
    hashed_password[strcspn(hashed_password, "\n")] = '\0';

    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(conn, "localhost", "capsule", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        exit_error(conn);
    }

    insert_user(conn, username, hashed_password, date_fin_contrat);
    mysql_close(conn);

    return 0;
}
