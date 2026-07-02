#include <iostream>
using namespace std;

int main() {

    // three numbers the user will give us, using float in case they enter decimals
    float n1, n2, n3;

    // ask the user for their three numbers
    cout << "Enter three numbers: ";

    // grab all three inputs at once, separated by spaces
    cin >> n1 >> n2 >> n3;

    // first guess: is n1 the biggest? check it against both of the others
    if (n1 >= n2 && n1 >= n3) {
        // yep, n1 wins
        cout << "Largest number: " << n1;
    }
    // if n1 wasn't the biggest, it's got to be n2 or n3
    else {
        // now check if n2 beats both n1 and n3
        if (n2 >= n1 && n2 >= n3) {
            // n2 takes the win
            cout << "Largest number: " << n2;
        }
        // if it's not n1 and it's not n2, there's only one option left
        else {
            // n3 must be the largest by process of elimination
            cout << "Largest number: " << n3;
        }
    }

    // wrap up the program
    return 0;
}