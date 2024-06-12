#!/bin/bash
sudo systemctl start mysqld

echo "MySQL will ask for your root password"
# Starting mysql as root
mysql -u root -p -e "source sql/addUser.sql" > /dev/null 2>&1
echo "Add user capsule to the database with the granted privileges needed"

mysql -u capsule -p'Capsule2024!' -e "source sql/capsAuthentification.sql" > /dev/null 2>&1
echo "Set up the database tables for the capsule user"

# Makefile execution
make -C "$(dirname "./Makefile")" | sed '1d;$d'

./script/updateLicenceExpiracy.sh
echo "Set up the licence expiracy cron job"

exit 0