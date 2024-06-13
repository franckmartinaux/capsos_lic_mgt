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
 * @return int The return code of the program.
 */
int exit_error(MYSQL *conn) {
    fprintf(stderr, "Error: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
}

/**
 * @brief Checks user groups based on contract end dates and updates their status.
 * @author Tanguy Soutric
 * 
 * This function queries the users table to get usernames and their contract end dates.
 * It updates the user groups to "licenseExpired" or "licenseActive" based on the comparison
 * between the contract end date and the current date.
 * 
 * @param conn The MySQL connection.
 * @return int The return code of the program.
 */
int check_user_groups(MYSQL *conn) {
    char query[1024];
    if (mysql_query(conn, "SELECT username, dateEndSupport FROM users")) {
        return exit_error(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        return exit_error(conn);
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        const char *username = row[0];
        const char *contract_end_date = row[1];
        time_t now;
        time(&now);
        struct tm *local = localtime(&now);
        char current_date[11];
        strftime(current_date, 11, "%Y-%m-%d", local);
        const char *groupname = (strcmp(contract_end_date, current_date) < 0) ? "licenseExpired" : "licenseActive";
        snprintf(query, sizeof(query), "UPDATE usergroups SET groupname = '%s' WHERE username = '%s'", groupname, username);
        if (mysql_query(conn, query)) {
            mysql_free_result(result);
            return exit_error(conn);
        }
    }

    mysql_free_result(result);
    printf("User groups updated successfully.\n");
    return 0;
}

/**
 * @brief The main entry point of the program.
 * @author Tanguy Soutric
 * 
 * @return int The exit status of the program.
 */
int main() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return -1;
    }
    if (mysql_real_connect(conn, "localhost", "capsule", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        return exit_error(conn);
    }
    check_user_groups(conn);
    mysql_close(conn);
    return 0;
}
