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

- Python3 installed on the server :

    **Install Python 3**
    ```sh
    sudo dnf install python3
    ```

### First use

- Go to the `capsos_lic_mgt` folder

- Launch the `firstRun.sh` script
    ```sh
    ./firstRun.sh
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

### Project structure

    **Description of the project structure**

    - `bin/` : contains the executable files
    - `doc/` : contains the doxygen documentation after using make doc
    - `script/` : contains the script files
    - `sql/` : contains the sql files
    - `src/` : contains the source code
    - `save/` : contains the c files, py files, sh files, sql files after using make save