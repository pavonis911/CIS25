#include <iostream>
#include <string>
using namespace std;

// ---------------------------------------------
// Base Class: User
// ---------------------------------------------
class User {
protected:
    string username; // protected access control

public:
    User(string name) : username(name) {}

    virtual void accessLevel() {
        cout << username << " - General Access\n";
    }
};

// ---------------------------------------------
// Derived Class: Employee
// ---------------------------------------------
class Employee : public User {
public:
    Employee(string name) : User(name) {}

    void accessLevel() override {
        cout << username << " (Employee) - View inventory\n";
    }
};

// ---------------------------------------------
// Derived Class: Manager
// ---------------------------------------------
class Manager : public User {
public:
    Manager(string name) : User(name) {}

    void accessLevel() override {
        cout << username << " (Manager) - View and update inventory\n";
    }
};

// ---------------------------------------------
// Derived Class: Admin
// ---------------------------------------------
class Admin : public User {
public:
    Admin(string name) : User(name) {}

    void accessLevel() override {
        cout << username << " (Admin) - Full access: add, update, delete inventory\n";
    }
};

// ---------------------------------------------
// Main Program
// ---------------------------------------------
int main() {
    Employee caleb("Caleb");
    Manager john("John");
    Admin zaki("Zaki");

    caleb.accessLevel();
    john.accessLevel();
    zaki.accessLevel();

    return 0;
}