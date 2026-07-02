# InternGrow Secure Authentication System

Secure User Authentication System built in C++ as part of the **InternGrow C++ Programming Track (Task 2)**.

## 📋 Overview
This console-based application allows users to register and log in securely. It validates input formats, prevents duplicate usernames, stores hashed credentials in an external file, and includes a **lockout mechanism** that temporarily freezes account access after repeated failed login attempts.

## ✨ Features
- **User Registration**: Create a new account with validated username and password
- **Input Validation**:
  - Username: 4-20 characters, letters/digits/underscore only
  - Password: minimum 6 characters, must contain both letters and digits
- **Duplicate Username Check**: Prevents multiple accounts with the same username
- **Secure Credential Storage**: Passwords are hashed before being saved to `users.txt` (never stored in plain text)
- **Login System**: Verifies hashed password against stored records
- **User Directory View**: Displays all registered users and their current account status

### 🚀 Upgrade Feature: Lockout Mechanism
After **3 consecutive failed login attempts**, the account is automatically locked for **60 seconds**. During the lockout period:
- Login attempts are blocked with a countdown message showing remaining lockout time
- Once the lockout period expires, the failed attempt counter resets automatically
- A successful login at any point resets the failed attempt counter to zero

## 💻 How to Run

### Compile
```bash
g++ -std=c++17 InternGrow_AuthSystem.cpp -o auth_system
```

### Run
```bash
./auth_system
```
(On Windows: `auth_system.exe`)

## 📖 Usage
1. **Register New User** – Create a username and password following the validation rules
2. **Login** – Enter credentials to access the system; 3 failed attempts triggers a temporary lockout
3. **View All Registered Users** – See all accounts and whether they are Active or Locked

## 🧩 Tech Stack
- **Language**: C++ (C++17)
- **Concepts Used**: File Handling (fstream), STL (map), String Validation, Hashing, Time-based Lockout Logic (ctime)

## 🔒 Security Note
This project uses `std::hash` for password hashing purely for **educational/demonstration purposes**. In real-world production systems, cryptographic hashing algorithms such as bcrypt or Argon2 (with salting) should be used instead.

## 📌 Built For
**InternGrow C++ Programming Track** — Module 1, Task 2
Contact: interngrow.official@gmail.com

## 👤 Author
[Samreen Javeed]

## 📄 License
This project was built for educational purposes as part of the InternGrow internship program.
