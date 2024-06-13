# License Management CapsOs

## Authors

    - Gitton Julien
    - Soutric Tanguy

## Project description

This project goal is to simplify the management of CapsOs user licenses on Alma Linux

## Running instructions

### Required libraries

#### Prerequisites

- MySQL installed on the server :

    **Add the official MySQL Repository**
    ```sh
    sudo dnf install https://dev.mysql.com/get/mysql80-community-release-el8-3.noarch.rpm
    ```

    **Install MySQL server**
    ```sh
    sudo dnf install mysql-server
    ```

### First use

- You need to be logged with a sudoer user

- Go to the `capsos_lic_mgt` folder

- Use firstRun target of the Makefile
    ```sh
    make firstRun
    ```

    - It will start the MySQL service
    - Add a granted user to access the database
    - Create the database
    - Make the executables in the `bin` folder
    - Set up the licence expiry cron job

### How to use it

- Go to the `bin` folder
    ```sh
    cd ./bin
    ```

- To add a user
    ```sh
    ./addUser.exe <user> <password> <licenseExpiracyDate>
    ```
    The password will be using the bcrypt hashing method

- To modify the expiry date of a user license
    ```sh
    ./modifyUser.exe <user> <licenseExpiracyDate>
    ```

- To update license's status (it will be done automatically every day at midnight)
    ```sh 
    ./updateDatabase.exe
    ```

- To add user into the database from a csv file :
    - Csv seperator need to be a ',' and meet the following column : "LicenseId", "Password", "Customer", "End User", "Project", "Region", "Date of Purchase", "End of Support", "Invoice Number". Please use date like MM/DD/YYYY.
    ```sh
    ./customerIntoDb.exe <csv_file_path>
    ```

### Project structure

#### Description of the project structure

    - `bin/` : contains the executable files
    - `doc/` : contains the doxygen documentation after using make doc
    - `script/` : contains the script files
    - `sql/` : contains the sql files
    - `src/` : contains the source code
    - `save/` : contains the c files, sh files, sql files after using make save

### To protect a folder of Apache server based on the database information

#### To protect a folder of your Apache server you need to

    - Create a folder secure in /var/www/html/secure

    - Copy the content of /capsos_lic/mgt/httpd.conf in /etc/httpd/conf/httpd.conf

        - the secure access will be only accessible by a valid user with an active license
    
    - You will need to create an SSL certificate and replace with the right path those lines :

        - SSLCertificateFile /var/www/html/certificate/cert.pem
        
        - SSLCertificateKeyFile /var/www/html/certificate/key.pem