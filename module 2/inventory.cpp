#include <iostream>
using namespace std;

int main() {
    // Print greeting message
    cout << "Hello Inventory System!" << endl;

    // Demonstrate escape characters
    cout << "\nDemonstrating escape characters:\n";
    cout << "\tUsing tab and newline\n";

    // Output sizes of int, float, and char
    cout << "\nData Type Sizes:\n";
    cout << "\tSize of int:   " << sizeof(int)   << " bytes\n";
    cout << "\tSize of float: " << sizeof(float) << " bytes\n";
    cout << "\tSize of char:  " << sizeof(char)  << " byte\n";

    return 0;
}
