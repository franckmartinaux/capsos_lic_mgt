import sys
import bcrypt

def hash_password(password):
    return bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt()).decode('utf-8')

if len(sys.argv) != 2:
    print("Usage: hash_password.py <password>")
    sys.exit(1)

password = sys.argv[1]
hashed_password = hash_password(password)
print(hashed_password)
