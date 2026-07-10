/*
    InternGrow C++ Programming Track
    TASK 2: Secure User Authentication System
    Feature: Validates inputs, checks duplicate usernames, stores credentials
             securely (hashed) in an external file.
    Upgrade: Lockout mechanism - freezes account for a set duration after
             3 consecutive failed login attempts.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;

const string USER_FILE = "users.txt";        // username,hash,salt
const string LOCK_FILE  = "lockouts.txt";     // username,failedAttempts,lockUntilEpoch
const int MAX_ATTEMPTS = 3;
const int LOCKOUT_SECONDS = 60; // lockout duration (60s for demo; adjust as needed)

// ---------------- Simple hashing (djb2-based) ----------------
// NOTE: For real production systems use bcrypt/Argon2. This is a
// lightweight, dependency-free hash suitable for a learning project.
unsigned long long simpleHash(const string &input) {
    unsigned long long hash = 5381;
    for (char c : input) {
        hash = ((hash << 5) + hash) + (unsigned char)c; // hash*33 + c
    }
    return hash;
}

string generateSalt(const string &username) {
    // deterministic-but-unique salt derived from username + fixed pepper
    return to_string(simpleHash(username + "InternGrow_Pepper_2025"));
}

string hashPassword(const string &password, const string &salt) {
    unsigned long long h = simpleHash(password + salt);
    stringstream ss;
    ss << hex << h;
    return ss.str();
}

// ---------------- Input validation ----------------
bool isValidUsername(const string &u) {
    if (u.length() < 4 || u.length() > 20) return false;
    for (char c : u) {
        if (!isalnum((unsigned char)c) && c != '_') return false;
    }
    return true;
}

bool isValidPassword(const string &p) {
    if (p.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false;
    for (char c : p) {
        if (isupper((unsigned char)c)) hasUpper = true;
        if (islower((unsigned char)c)) hasLower = true;
        if (isdigit((unsigned char)c)) hasDigit = true;
    }
    return hasUpper && hasLower && hasDigit;
}

// ---------------- File helpers ----------------
bool usernameExists(const string &username) {
    ifstream file(USER_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUser;
        getline(ss, storedUser, ',');
        if (storedUser == username) return true;
    }
    return false;
}

void saveUser(const string &username, const string &hash, const string &salt) {
    ofstream file(USER_FILE, ios::app);
    file << username << "," << hash << "," << salt << "\n";
}

bool getStoredCredentials(const string &username, string &hash, string &salt) {
    ifstream file(USER_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUser, storedHash, storedSalt;
        getline(ss, storedUser, ',');
        getline(ss, storedHash, ',');
        getline(ss, storedSalt, ',');
        if (storedUser == username) {
            hash = storedHash;
            salt = storedSalt;
            return true;
        }
    }
    return false;
}

// ---------------- Lockout mechanism (Upgrade Feature) ----------------
struct LockInfo {
    int failedAttempts = 0;
    long long lockUntil = 0; // epoch seconds; 0 = not locked
};

map<string, LockInfo> loadLockData() {
    map<string, LockInfo> data;
    ifstream file(LOCK_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string user, attemptsStr, lockUntilStr;
        getline(ss, user, ',');
        getline(ss, attemptsStr, ',');
        getline(ss, lockUntilStr, ',');
        if (!user.empty()) {
            LockInfo info;
            info.failedAttempts = stoi(attemptsStr);
            info.lockUntil = stoll(lockUntilStr);
            data[user] = info;
        }
    }
    return data;
}

void saveLockData(const map<string, LockInfo> &data) {
    ofstream file(LOCK_FILE, ios::trunc);
    for (auto &pair : data) {
        file << pair.first << "," << pair.second.failedAttempts << ","
             << pair.second.lockUntil << "\n";
    }
}

long long currentEpoch() {
    return (long long)time(nullptr);
}

// Returns true if the account is currently locked, prints remaining time.
bool isLocked(const string &username, map<string, LockInfo> &lockData) {
    auto it = lockData.find(username);
    if (it == lockData.end()) return false;

    long long now = currentEpoch();
    if (it->second.lockUntil > now) {
        long long remaining = it->second.lockUntil - now;
        cout << "Account locked due to too many failed attempts. "
             << "Try again in " << remaining << " seconds.\n";
        return true;
    } else if (it->second.lockUntil != 0 && it->second.lockUntil <= now) {
        // Lock expired -> reset
        it->second.failedAttempts = 0;
        it->second.lockUntil = 0;
        saveLockData(lockData);
    }
    return false;
}

void registerFailedAttempt(const string &username, map<string, LockInfo> &lockData) {
    LockInfo &info = lockData[username];
    info.failedAttempts++;
    if (info.failedAttempts >= MAX_ATTEMPTS) {
        info.lockUntil = currentEpoch() + LOCKOUT_SECONDS;
        cout << "Too many failed attempts! Account locked for "
             << LOCKOUT_SECONDS << " seconds.\n";
    } else {
        cout << "Incorrect password. Attempts remaining: "
             << (MAX_ATTEMPTS - info.failedAttempts) << "\n";
    }
    saveLockData(lockData);
}

void resetAttempts(const string &username, map<string, LockInfo> &lockData) {
    lockData[username] = LockInfo(); // reset to 0 attempts, no lock
    saveLockData(lockData);
}

// ---------------- Core actions ----------------
void registerUser() {
    string username, password;

    cout << "\n--- Register New User ---\n";
    cout << "Enter username (4-20 chars, letters/digits/underscore): ";
    cin >> username;

    if (!isValidUsername(username)) {
        cout << "Invalid username format.\n";
        return;
    }
    if (usernameExists(username)) {
        cout << "Username already exists. Choose another.\n";
        return;
    }

    cout << "Enter password (min 8 chars, must include upper, lower, digit): ";
    cin >> password;

    if (!isValidPassword(password)) {
        cout << "Password does not meet security requirements.\n";
        return;
    }

    string salt = generateSalt(username);
    string hash = hashPassword(password, salt);
    saveUser(username, hash, salt);

    cout << "Registration successful! You can now log in.\n";
}

void loginUser() {
    string username, password;
    auto lockData = loadLockData();

    cout << "\n--- Login ---\n";
    cout << "Enter username: ";
    cin >> username;

    if (isLocked(username, lockData)) {
        return;
    }

    if (!usernameExists(username)) {
        cout << "No such user found.\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    string storedHash, salt;
    getStoredCredentials(username, storedHash, salt);
    string attemptHash = hashPassword(password, salt);

    if (attemptHash == storedHash) {
        cout << "Login successful! Welcome, " << username << ".\n";
        resetAttempts(username, lockData);
    } else {
        registerFailedAttempt(username, lockData);
    }
}

int main() {
    int choice;
    do {
        cout << "\n===== Secure Authentication System =====\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
            case 1: registerUser(); break;
            case 2: loginUser(); break;
            case 3: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}
