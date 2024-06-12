SET GLOBAL validate_password.policy=MEDIUM;
CREATE USER IF NOT EXISTS 'capsule'@'localhost' IDENTIFIED BY 'Capsule2024!';

-- Giving all privileges to new created user 'capsule' on the database 'capsAuthentification'
GRANT ALL PRIVILEGES ON capsAuthentification.* TO 'capsule'@'localhost';

GRANT SUPER ON *.* TO 'capsule'@'localhost';

-- Update privileges
FLUSH PRIVILEGES;