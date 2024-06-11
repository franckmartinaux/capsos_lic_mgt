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

INSERT INTO users (username, pw, dateFinContrat) VALUES
('user1', '$2b$12$Im4Or2f9uktNdcnrJI4WYOLoYaXOeEinFGRiIebMLncNURCsVTq3S', '2024-12-31'),
('user2', '$2b$12$6/8GgxL/d29YdbWNP8T8IeBmAu5E4LVd4uXTbzUUUtp9aDBZYPLWe', '2025-01-01'),
('user3', '$2b$12$Z1P2S4x7sRsy7o7NsZkRmet2zH8Fgr0uQpYjcTEoZQYoZ.id8BbFK', '2025-01-01');

INSERT INTO usergroups (username, groupname) VALUES
('user1', 'licenseActive'),
('user2', 'licenseExpired'),
('user3', 'licenseActive');