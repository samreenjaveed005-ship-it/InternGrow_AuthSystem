/*
    InternGrow C++ Programming Track - Task 2
    Secure User Authentication System
    ---------------------------------------------------------------
    Features:
    - Validate text string inputs (username/password rules)
    - Check for duplicate usernames
    - Store credentials securely in external file (hashed passwords)
    - Upgrade Feature: Lockout mechanism after 3 consecutive
      failed login attempts, freezes account for a set duration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <functional>

using namespace std;

const string USER_FILE = "users.txt";
const int MAX_FAILED_ATTEMPTS = 3;
const int LOCKOUT_DURATION_SECONDS = 60; // 1 minute lockout for demo purposes

// -----------------------------------------------------------------
// Simple hash function (for demonstration - NOT for production use)
// In real-world systems, use bcrypt/argon2. Here we use std::hash
// to avoid storing plain-text passwords in the file.
// -----------------------------------------------------------------
string hashPassword(const string& password) {
    hash<string> hasher;
    size_t hashed = hasher(password);
    stringstream ss;
    ss << hex << hashed;
    return ss.str();
}

// -----------------------------------------------------------------
// User record structure
// -----------------------------------------------------------------
struct UserRecord {
    string username;
    string hashedPassword;
    int failedAttempts = 0;
    time_t lockoutUntil = 0; // 0 means not locked
};

// -----------------------------------------------------------------
// Input Validation Helpers
// -----------------------------------------------------------------
bool isValidUsername(const string& username) {
    if (username.length() < 4 || username.length() > 20) return false;
    for (char c : username) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool isValidPassword(const string& password) {
    if (password.length() < 6) return false;
    bool hasDigit = false, hasAlpha = false;
    for (char c : password) {
        if (isdigit(c)) hasDigit = true;
        if (isalpha(c)) hasAlpha = true;
    }
    return hasDigit && hasAlpha;
}

// -----------------------------------------------------------------
// AuthSystem class - handles registration, login, file storage
// -----------------------------------------------------------------
class AuthSystem {
private:
    map<string, UserRecord> users; // in-memory cache, synced with file

    // Load all users from file into memory
    void loadUsers() {
        users.clear();
        ifstream file(USER_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            UserRecord u;
            string failedStr, lockoutStr;

            getline(ss, u.username, '|');
            getline(ss, u.hashedPassword, '|');
            getline(ss, failedStr, '|');
            getline(ss, lockoutStr, '|');

            if (!failedStr.empty()) u.failedAttempts = stoi(failedStr);
            if (!lockoutStr.empty()) u.lockoutUntil = (time_t)stoll(lockoutStr);

            users[u.username] = u;
        }
        file.close();
    }

    // Save all users from memory back to file
    void saveUsers() {
        ofstream file(USER_FILE, ios::trunc);
        for (const auto& pair : users) {
            const UserRecord& u = pair.second;
            file << u.username << "|" << u.hashedPassword << "|"
                 << u.failedAttempts << "|" << u.lockoutUntil << "\n";
        }
        file.close();
    }

public:
    AuthSystem() {
        loadUsers();
    }

    bool usernameExists(const string& username) {
        return users.find(username) != users.end();
    }

    // -------------------------------------------------------------
    // REGISTRATION
    // -------------------------------------------------------------
    void registerUser() {
        string username, password, confirmPassword;

        cout << "\n=== New User Registration ===\n";
        cout << "Username (4-20 chars, letters/digits/underscore only): ";
        cin >> username;

        if (!isValidUsername(username)) {
            cout << "Invalid username format. Registration failed.\n";
            return;
        }

        if (usernameExists(username)) {
            cout << "Username already exists. Please choose another.\n";
            return;
        }

        cout << "Password (min 6 chars, must include letters and digits): ";
        cin >> password;

        if (!isValidPassword(password)) {
            cout << "Password too weak. Must be 6+ chars with letters & digits.\n";
            return;
        }

        cout << "Confirm Password: ";
        cin >> confirmPassword;

        if (password != confirmPassword) {
            cout << "Passwords do not match. Registration failed.\n";
            return;
        }

        UserRecord newUser;
        newUser.username = username;
        newUser.hashedPassword = hashPassword(password);
        newUser.failedAttempts = 0;
        newUser.lockoutUntil = 0;

        users[username] = newUser;
        saveUsers();

        cout << "Registration successful! You can now log in.\n";
    }

    // -------------------------------------------------------------
    // LOGIN with Lockout Mechanism (Upgrade Feature)
    // -------------------------------------------------------------
    void loginUser() {
        string username, password;

        cout << "\n=== User Login ===\n";
        cout << "Username: ";
        cin >> username;

        loadUsers(); // refresh from file in case of updates

        if (!usernameExists(username)) {
            cout << "Username not found. Please register first.\n";
            return;
        }

        UserRecord& user = users[username];
        time_t now = time(0);

        // Check if account is currently locked
        if (user.lockoutUntil > now) {
            int remainingSeconds = (int)(user.lockoutUntil - now);
            cout << "Account LOCKED due to multiple failed attempts.\n";
            cout << "Please try again in " << remainingSeconds << " seconds.\n";
            return;
        }

        // Lockout period expired - reset failed attempts
        if (user.lockoutUntil != 0 && user.lockoutUntil <= now) {
            user.failedAttempts = 0;
            user.lockoutUntil = 0;
        }

        cout << "Password: ";
        cin >> password;

        string hashedInput = hashPassword(password);

        if (hashedInput == user.hashedPassword) {
            // Successful login
            user.failedAttempts = 0;
            user.lockoutUntil = 0;
            saveUsers();
            cout << "\nLogin successful! Welcome, " << username << ".\n";
        } else {
            // Failed login attempt
            user.failedAttempts++;
            cout << "Incorrect password. Attempt " << user.failedAttempts
                 << " of " << MAX_FAILED_ATTEMPTS << ".\n";

            if (user.failedAttempts >= MAX_FAILED_ATTEMPTS) {
                user.lockoutUntil = now + LOCKOUT_DURATION_SECONDS;
                cout << "Account LOCKED for " << LOCKOUT_DURATION_SECONDS
                     << " seconds due to 3 consecutive failed attempts.\n";
            } else {
                int remaining = MAX_FAILED_ATTEMPTS - user.failedAttempts;
                cout << remaining << " attempt(s) remaining before lockout.\n";
            }

            saveUsers();
        }
    }

    // -------------------------------------------------------------
    // View all registered users (admin/demo utility)
    // -------------------------------------------------------------
    void listUsers() {
        loadUsers();
        cout << "\n=== Registered Users ===\n";
        if (users.empty()) {
            cout << "No users registered yet.\n";
            return;
        }

        cout << left << setw(15) << "Username" << setw(12) << "Failed"
             << "Status\n";
        cout << string(45, '-') << "\n";

        time_t now = time(0);
        for (const auto& pair : users) {
            const UserRecord& u = pair.second;
            string status = "Active";
            if (u.lockoutUntil > now) {
                status = "LOCKED (" + to_string((int)(u.lockoutUntil - now)) + "s left)";
            }
            cout << left << setw(15) << u.username << setw(12) << u.failedAttempts
                 << status << "\n";
        }
    }
};

// -----------------------------------------------------------------
// Menu-driven interface
// -----------------------------------------------------------------
void printMenu() {
    cout << "\n===================================\n";
    cout << "   InternGrow Secure Auth System\n";
    cout << "===================================\n";
    cout << "1. Register New User\n";
    cout << "2. Login\n";
    cout << "3. View All Registered Users\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    AuthSystem auth;
    int choice;

    cout << "Welcome to InternGrow Secure User Authentication System\n";

    do {
        printMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                auth.registerUser();
                break;
            case 2:
                auth.loginUser();
                break;
            case 3:
                auth.listUsers();
                break;
            case 0:
                cout << "Thank you for using InternGrow Auth System!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
