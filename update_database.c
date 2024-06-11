#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>

void exit_error(MYSQL *conn) {
    fprintf(stderr, "Error: %s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

void check_user_groups(MYSQL *conn) {
    char query[1024];
    if (mysql_query(conn, "SELECT username, dateFinContrat FROM users")) {
        exit_error(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        exit_error(conn);
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        const char *username = row[0];
        const char *date_fin_contrat = row[1];
        time_t now;
        time(&now);
        struct tm *local = localtime(&now);
        char current_date[11];
        strftime(current_date, 11, "%Y-%m-%d", local);
        const char *groupname = (strcmp(date_fin_contrat, current_date) < 0) ? "licenseExpired" : "licenseActive";
        snprintf(query, sizeof(query), "UPDATE usergroups SET groupname = '%s' WHERE username = '%s'", groupname, username);
        if (mysql_query(conn, query)) {
            mysql_free_result(result);
            exit_error(conn);
        }
    }

    mysql_free_result(result);
    printf("User groups updated successfully.\n");
}

int main() {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }
    if (mysql_real_connect(conn, "localhost", "root", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        exit_error(conn);
    }
    check_user_groups(conn);
    mysql_close(conn);
    return 0;
}
