DROP DATABASE IF EXISTS capsAuthentification;
CREATE DATABASE capsAuthentification;
USE capsAuthentification;

CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    pw VARCHAR(255) NOT NULL,
    customer VARCHAR(50) NOT NULL,
    endUser VARCHAR(50),
    project VARCHAR(50),
    region VARCHAR(50),
    dateOfPurchase DATE NOT NULL,
    dateEndSupport DATE NOT NULL,
    invoiceNumber VARCHAR(10)
);

CREATE TABLE usergroups (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    groupname VARCHAR(50) NOT NULL,
    FOREIGN KEY (username) REFERENCES users(username)
);