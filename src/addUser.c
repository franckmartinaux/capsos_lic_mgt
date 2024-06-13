/**
 * @file addUser.c
 * @brief Program to insert users into a MySQL database.
 * 
 * This program takes a username, a password, customer, end user, project, region, date of purchase,
 * contract end date, and invoice number as arguments, hashes the password using bcrypt, and inserts 
 * the user into the MySQL database. If the user already exists, a notification is displayed. 
 * Based on the contract end date, the user is placed in either an active or expired license group.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include <crypt.h>

/**
 * @brief Prints an error message and exits the program.
 * 
 * @param conn The MySQL connection.
 * @return int The return code of the program.
 */
int exit_error(MYSQL *conn) {
    fprintf(stderr, "Error: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
}

/**
 * @brief Inserts a user into the database.
 * Checks if the user already exists. If not, it inserts the user
 * with the hashed password and contract end date. 
 * Depending on the contract end date, the user is added to the "licenseExpired" or "licenseActive" group.
 * 
 * @param conn The MySQL connection.
 * @param username The username.
 * @param hashed_password The hashed password.
 * @param customer The customer.
 * @param endUser The end user.
 * @param project The project.
 * @param region The region.
 * @param dateOfPurchase The date of purchase.
 * @param contract_end_date The contract end date.
 * @param invoiceNumber The invoice number.
 * @return int The return code of the program.
 */
int insert_user(MYSQL *conn, const char *username, const char *hashed_password, const char *customer, const char *endUser, const char *project, const char *region, const char *dateOfPurchase, const char *contract_end_date, const char *invoiceNumber) {
    char query[2048];
    MYSQL_RES *res;
    MYSQL_ROW row;
    srand(time(NULL));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM users WHERE username = '%s'", username);
    if (mysql_query(conn, query)) {
        return exit_error(conn);
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    int user_exists = atoi(row[0]);
    mysql_free_result(res);

    if (user_exists) {
        fprintf(stderr, "User %s already exists in the database.\n", username);
        return 1;
    }

    snprintf(query, sizeof(query), "INSERT INTO users (username, pw, customer, endUser, project, region, dateOfPurchase, dateEndSupport, invoiceNumber) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", username, hashed_password, customer, endUser, project, region, dateOfPurchase, contract_end_date, invoiceNumber);
    if (mysql_query(conn, query)) {
        return exit_error(conn);
    }

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char current_date[11];
    strftime(current_date, 11, "%Y-%m-%d", local);
    const char *groupname = (strcmp(contract_end_date, current_date) < 0) ? "licenseExpired" : "licenseActive";

    snprintf(query, sizeof(query), "INSERT INTO usergroups (username, groupname) VALUES ('%s', '%s')", username, groupname);
    if (mysql_query(conn, query)) {
        return exit_error(conn);
    }

    printf("%s inserted with hashed password and contract end date %s.\n", username, contract_end_date);
    return 0;
}

/**
 * @brief The main entry point of the program.
 * 
 * @param argc The number of arguments.
 * @param argv The arguments passed to the program.
 * @return int The return code of the program.
 */
int main(int argc, char *argv[]) {
    if (argc != 10) {
        printf("Usage: %s <username> <password> <customer> <endUser> <project> <region> <dateOfPurchase> <contract_end_date> <invoiceNumber>\n", argv[0]);
        return 1;
    }

    const char *username = argv[1];
    const char *password = argv[2];
    const char *customer = argv[3];
    const char *endUser = argv[4];
    const char *project = argv[5];
    const char *region = argv[6];
    const char *dateOfPurchase = argv[7];
    const char *contract_end_date = argv[8];
    const char *invoiceNumber = argv[9];

    char salt[30] = "$2b$12$";
    const char *salt_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";
    for (int i = 7; i < 29; i++) {
        salt[i] = salt_chars[rand() % 64];
    }
    salt[29] = '\0';

    char *hashed_password = crypt(password, salt);
    if (hashed_password == NULL) {
        fprintf(stderr, "Error hashing password\n");
        return -1;
    }

    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return -1;
    }

    if (mysql_real_connect(conn, "localhost", "capsule", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        return exit_error(conn);
    }

    insert_user(conn, username, hashed_password, customer, endUser, project, region, dateOfPurchase, contract_end_date, invoiceNumber);
    mysql_close(conn);

    return 0;
}
