# InternGrow_AuthenticationSystem

## 📌 Project Title
Secure User Authentication System

## 📖 Description
A C++ console application that implements a basic but secure login system:
- Validates username and password formats
- Prevents duplicate username registration
- Stores credentials **hashed with a per-user salt** in an external file
  (`users.txt`) — plaintext passwords are never stored

### 🚀 Upgrade Feature — Account Lockout Mechanism
After **3 consecutive failed login attempts**, the account is temporarily
**locked for 60 seconds**. Lockout state is tracked per-user in
`lockouts.txt`, so the freeze persists even if the program is restarted
during the lockout window. A successful login resets the failed-attempt
counter.

> ⚠️ Note: The hashing used here is a lightweight custom hash (djb2-based)
> chosen to keep the project dependency-free for a learning exercise. In a
> production system, use a vetted algorithm such as **bcrypt** or **Argon2**.

## 🛠️ Tech Stack
- Language: C++ (C++17)
- Storage: Plain text files (`users.txt`, `lockouts.txt`) used as a simple
  external database

## 📂 File Structure
```
InternGrow_AuthenticationSystem/
├── Task2_Authentication_System.cpp
├── users.txt        (auto-created on first registration)
├── lockouts.txt      (auto-created on first failed login)
└── README.md
```

## ⚙️ How to Compile & Run
```bash
g++ -std=c++17 -o AuthSystem Task2_Authentication_System.cpp
./AuthSystem
```
On Windows (MinGW):
```bash
g++ -std=c++17 -o AuthSystem.exe Task2_Authentication_System.cpp
AuthSystem.exe
```

## ▶️ How to Use
On launch you'll see a menu:
```
1. Register
2. Login
3. Exit
```

**Register**
- Username: 4–20 characters, letters/digits/underscore only
- Password: minimum 8 characters, must contain at least one uppercase
  letter, one lowercase letter, and one digit

**Login**
- Enter your username and password.
- On 3 consecutive wrong passwords, the account locks for 60 seconds —
  further attempts during this window are rejected with a countdown message.
- A correct login resets the failed-attempt counter to zero.

## 📊 Sample Output
```
===== Secure Authentication System =====
1. Register
2. Login
3. Exit
Choose an option: 2

--- Login ---
Enter username: ali_dev
Enter password: wrongpass1
Incorrect password. Attempts remaining: 2
...
Too many failed attempts! Account locked for 60 seconds.
```

## 🔒 Security Notes
- Passwords are never stored or displayed in plaintext.
- Each password is combined with a unique salt before hashing, protecting
  against simple rainbow-table attacks.
- Lockout state survives program restarts since it's persisted to disk.

## 👤 Author
InternGrow C++ Programming Track — Module 1, Task 2

## 📄 License
This project was developed as part of the InternGrow internship program for
educational purposes.
