#include <iostream>
using namespace std;

struct Patient
{
    int id;
    string name;
    int age;
};

int main()
{
    // Regular variable
    Patient patient1;

    // Assign values
    patient1.id = 911;
    patient1.name = "John Snow";
    patient1.age = 21;

    // Pointer storing address
    Patient* p = &patient1;

    // Display using pointer
    cout << "ID: " << p->id << endl;
    cout << "Name: " << p->name << endl;
    cout << "Age: " << p->age << endl;

    return 0;
}