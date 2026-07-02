#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// one item in the cart
class Item {
public:
    string name;
    double price;
    int quantity;

    // price times quantity
    double getTotalPrice() {
        return price * quantity;
    }
};

// so main knows these exist
void addItemToCart();
void viewCheckout();

int main() {
    int choice;

    // show menu till they exit
    do {
        cout << "\n===== SHOPPING CART MENU =====" << endl;
        cout << "1. Add item to cart" << endl;
        cout << "2. View checkout" << endl;
        cout << "3. Exit" << endl;
        cout << "Choose an option: ";
        cin >> choice;

        // send them to the right function
        if (choice == 1) {
            addItemToCart();
        }
        else if (choice == 2) {
            viewCheckout();
        }
        else if (choice == 3) {
            cout << "Thank you for shopping!" << endl;
        }
        else {
            cout << "Invalid choice, try again." << endl;
        }

    } while (choice != 3);

    return 0;
}

// get item info and save it
void addItemToCart() {
    Item newItem;

    cout << "Enter item name: ";
    cin >> newItem.name;

    cout << "Enter item price: ";
    cin >> newItem.price;

    cout << "Enter item quantity: ";
    cin >> newItem.quantity;

    // catch bad input (like a name with a space) so it doesn't break the menu
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Item not added." << endl;
        return;
    }

    // app = add to file, don't erase it
    ofstream outFile("cart.txt", ios::app);

    if (outFile.is_open()) {
        outFile << newItem.name << " " << newItem.price << " " << newItem.quantity << endl;
        outFile.close();
        cout << "Item added to cart." << endl;
    }
    else {
        cout << "Unable to open file for writing." << endl;
    }
}

// read items back and show total
void viewCheckout() {
    ifstream inFile("cart.txt");
    double grandTotal = 0;

    if (!inFile.is_open()) {
        cout << "No items found. Cart is empty." << endl;
        return;
    }

    cout << "\n===== CHECKOUT =====" << endl;

    Item currentItem;

    // one item at a time till file runs out
    while (inFile >> currentItem.name >> currentItem.price >> currentItem.quantity) {
        double lineTotal = currentItem.getTotalPrice();
        grandTotal += lineTotal;

        cout << currentItem.name << " - $" << currentItem.price
             << " x " << currentItem.quantity
             << " = $" << lineTotal << endl;
    }

    inFile.close();

    cout << "----------------------" << endl;
    cout << "Total: $" << grandTotal << endl;
}