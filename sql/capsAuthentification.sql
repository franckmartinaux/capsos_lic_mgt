DROP DATABASE IF EXISTS capsAuthentification;
CREATE DATABASE capsAuthentification;
USE capsAuthentification;

CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    pw VARCHAR(255) NOT NULL,
    dateFinContrat DATE NOT NULL
);

CREATE TABLE usergroups (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    groupname VARCHAR(50) NOT NULL,
    FOREIGN KEY (username) REFERENCES users(username)
);