#include <iostream>
using namespace std;

int main() {
    int choice;

    cout << "MENU" << endl;
    cout << "1. Say Hello" << endl;
    cout << "2. Say Bye" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Hello!" << endl;
    }
    else if (choice == 2) {
        cout << "Bye!" << endl;
    }
    else {
        cout << "Invalid choice." << endl;
    }

    return 0;
}
