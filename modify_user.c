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

    if (mysql_real_connect(conn, "localhost", "root", "Capsule2024!", "capsAuthentification", 0, NULL, 0) == NULL) {
        exit_error(conn);
    }

    modify_user(conn, username, new_date);

    mysql_close(conn);

    return 0;
}
