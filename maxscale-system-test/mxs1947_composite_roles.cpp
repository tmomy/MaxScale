/**
 * MXS-1947: Composite roles are not supported
 *
 * https://jira.mariadb.org/browse/MXS-1947
 */

#include "testconnections.h"

int main(int argc, char** argv)
{
    TestConnections test(argc, argv);

    test.repl->connect();
    test.try_query(test.repl->nodes[0], "%s",
                   "CREATE USER test@'%' IDENTIFIED BY 'test';"
                   "CREATE ROLE a;"
                   "CREATE ROLE b;"
                   "CREATE DATABASE db;"
                   "GRANT ALL ON db.* TO a;"
                   "GRANT a TO b;"
                   "GRANT b TO test@'%';"
                   "SET DEFAULT ROLE b FOR test@'%';");

    // Wait for the users to replicate
    test.repl->sync_slaves();

    test.tprintf("Connect with a user that has a composite role as the default role");
    MYSQL* conn = open_conn_db(test.maxscales->rwsplit_port[0], test.maxscales->IP[0], "db", "test", "test");
    test.assert(mysql_errno(conn) == 0, "Connection failed: %s", mysql_error(conn));
    mysql_close(conn);

    test.try_query(test.repl->nodes[0], "%s",
                   "DROP DATABASE IF EXISTS db;"
                   "DROP ROLE IF EXISTS a;"
                   "DROP ROLE IF EXISTS b;"
                   "DROP USER 'test'@'%';");

    return test.global_result;
}
