/*
    Hospital Patient Management System
    Module 8 - Structs, Pointers, Dynamic Arrays, Binary Search, File I/O

    Features:
      1. Add a patient
      2. Display all patients
      3. Search for a patient by ID (Binary Search)
      4. Update patient information
      5. Save patients to file
      6. Exit program

    Notes on design choices:
      - Patients are stored in a DYNAMIC ARRAY accessed through a pointer
        (Patient* patients = new Patient[capacity];), as required.
      - A separate "count" variable tracks how many patients are actually
        stored in the array (the array itself may have unused capacity).
      - Binary search requires a SORTED array. To keep binary search valid
        at all times, new patients are inserted directly into their sorted
        position by ID (an "insertion" style add) rather than just appended
        and sorted later. This satisfies the "organizing data" learning
        objective as well.
      - On startup, the program automatically loads any existing data from
        patients.txt so staff don't lose previously saved records.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
using namespace std;

// ---------------------------------------------------------
// Step 1: Struct to store patient data
// ---------------------------------------------------------
struct Patient
{
    int id;
    string name;
    int age;
    string gender;
    string diagnosis;
};

const string FILENAME = "patients.txt";

// ---------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------
int loadPatientsFromFile(Patient*& patients, int capacity, const string& filename);
void savePatientsToFile(Patient* patients, int count, const string& filename);
void addPatient(Patient*& patients, int& count, int& capacity);
void displayPatients(Patient* patients, int count);
int binarySearch(Patient* patients, int count, int searchID);
void searchPatient(Patient* patients, int count);
void updatePatient(Patient* patients, int count);
void resizeArray(Patient*& patients, int& capacity);
void printMenu();
int getIntInput(const string& prompt);
string getLineInput(const string& prompt);

// ---------------------------------------------------------
// main()
// ---------------------------------------------------------
int main()
{
    // Step 2: Dynamic array requirement - patients stored via pointer
    int capacity = 100;                       // starting capacity
    Patient* patients = new Patient[capacity]; // dynamic array of Patient
    int count = 0;                             // tracks number of patients in use

    // Load existing patients from file (if any) when the program starts
    count = loadPatientsFromFile(patients, capacity, FILENAME);
    if (count > 0)
        cout << "Loaded " << count << " patient record(s) from " << FILENAME << ".\n";
    else
        cout << "No existing patient file found. Starting with an empty system.\n";

    int choice = 0;

    do
    {
        printMenu();
        choice = getIntInput("Enter your choice (1-6): ");

        switch (choice)
        {
        case 1:
            addPatient(patients, count, capacity);
            break;
        case 2:
            displayPatients(patients, count);
            break;
        case 3:
            searchPatient(patients, count);
            break;
        case 4:
            updatePatient(patients, count);
            break;
        case 5:
            savePatientsToFile(patients, count, FILENAME);
            break;
        case 6:
            cout << "\nGood Bye!\n";
            break;
        default:
            cout << "\nInvalid choice. Please enter a number between 1 and 6.\n";
            break;
        }

    } while (choice != 6);

    // Clean up dynamic memory before exiting
    delete[] patients;
    patients = nullptr;

    return 0;
}

// ---------------------------------------------------------
// Displays the main menu
// ---------------------------------------------------------
void printMenu()
{
    cout << "\n===================================\n";
    cout << "  HOSPITAL PATIENT MANAGEMENT SYSTEM\n";
    cout << "===================================\n";
    cout << "1. Add a patient\n";
    cout << "2. Display all patients\n";
    cout << "3. Search for a patient by ID (Binary Search)\n";
    cout << "4. Update patient information\n";
    cout << "5. Save patients to file\n";
    cout << "6. Exit program\n";
    cout << "===================================\n";
}

// ---------------------------------------------------------
// Safely reads an integer from the user, re-prompting on bad input
// ---------------------------------------------------------
int getIntInput(const string& prompt)
{
    int value;
    cout << prompt;
    while (!(cin >> value))
    {
        cout << "Invalid input. Please enter a whole number: ";
        cin.clear(); // clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard bad input
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard trailing newline
    return value;
}

// ---------------------------------------------------------
// Safely reads a full line of text from the user
// ---------------------------------------------------------
string getLineInput(const string& prompt)
{
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// ---------------------------------------------------------
// Doubles the capacity of the dynamic array when it becomes full
// ---------------------------------------------------------
void resizeArray(Patient*& patients, int& capacity)
{
    int newCapacity = capacity * 2;
    Patient* newArray = new Patient[newCapacity];

    for (int i = 0; i < capacity; i++)
        newArray[i] = patients[i];

    delete[] patients;
    patients = newArray;
    capacity = newCapacity;

    cout << "(System note: patient array capacity expanded to " << capacity << ")\n";
}

// ---------------------------------------------------------
// Step 4: Add patient
// Inserts the new patient directly into its correct sorted
// position (by ID) so the array stays sorted for binary search.
// ---------------------------------------------------------
void addPatient(Patient*& patients, int& count, int& capacity)
{
    if (count >= capacity)
        resizeArray(patients, capacity);

    Patient newPatient;

    newPatient.id = getIntInput("Enter Patient ID: ");

    // Prevent duplicate IDs, since binary search depends on unique, sorted IDs
    if (binarySearch(patients, count, newPatient.id) != -1)
    {
        cout << "A patient with ID " << newPatient.id << " already exists. Add cancelled.\n";
        return;
    }

    newPatient.name = getLineInput("Enter Patient Name: ");
    newPatient.age = getIntInput("Enter Patient Age: ");
    newPatient.gender = getLineInput("Enter Patient Gender: ");
    newPatient.diagnosis = getLineInput("Enter Patient Diagnosis: ");

    // Find correct sorted insertion index (ascending by ID)
    int insertIndex = 0;
    while (insertIndex < count && patients[insertIndex].id < newPatient.id)
        insertIndex++;

    // Shift everything after insertIndex one spot to the right
    for (int i = count; i > insertIndex; i--)
        patients[i] = patients[i - 1];

    patients[insertIndex] = newPatient;
    count++;

    cout << "\nPatient added successfully!\n";
}

// ---------------------------------------------------------
// Step 5: Display all patients
// ---------------------------------------------------------
void displayPatients(Patient* patients, int count)
{
    if (count == 0)
    {
        cout << "\nNo patient records to display.\n";
        return;
    }

    cout << "\n--------------------------------------------------------------------\n";
    cout << left;
    cout.width(8);  cout << "ID";
    cout.width(20); cout << "Name";
    cout.width(6);  cout << "Age";
    cout.width(10); cout << "Gender";
    cout.width(20); cout << "Diagnosis";
    cout << "\n--------------------------------------------------------------------\n";

    for (int i = 0; i < count; i++)
    {
        cout.width(8);  cout << patients[i].id;
        cout.width(20); cout << patients[i].name;
        cout.width(6);  cout << patients[i].age;
        cout.width(10); cout << patients[i].gender;
        cout.width(20); cout << patients[i].diagnosis;
        cout << "\n";
    }
    cout << "--------------------------------------------------------------------\n";
    cout << "Total patients: " << count << "\n";
}

// ---------------------------------------------------------
// Step 6: Binary search on a sorted-by-ID array
// Returns the index of the patient if found, or -1 if not found
// ---------------------------------------------------------
int binarySearch(Patient* patients, int count, int searchID)
{
    int left = 0;
    int right = count - 1;

    while (left <= right)
    {
        int mid = (left + right) / 2;

        if (patients[mid].id == searchID)
            return mid;
        else if (patients[mid].id < searchID)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1; // not found
}

// ---------------------------------------------------------
// Uses binarySearch() to find and display one patient by ID
// ---------------------------------------------------------
void searchPatient(Patient* patients, int count)
{
    if (count == 0)
    {
        cout << "\nNo patient records to search.\n";
        return;
    }

    int searchID = getIntInput("Enter Patient ID to search: ");
    int result = binarySearch(patients, count, searchID);

    if (result != -1)
    {
        cout << "\nPatient Found!\n";
        cout << "ID: "        << patients[result].id        << "\n";
        cout << "Name: "      << patients[result].name      << "\n";
        cout << "Age: "       << patients[result].age       << "\n";
        cout << "Gender: "    << patients[result].gender    << "\n";
        cout << "Diagnosis: " << patients[result].diagnosis << "\n";
    }
    else
    {
        cout << "\nPatient with ID " << searchID << " not found.\n";
    }
}

// ---------------------------------------------------------
// Step 7: Update an existing patient's information
// ---------------------------------------------------------
void updatePatient(Patient* patients, int count)
{
    if (count == 0)
    {
        cout << "\nNo patient records to update.\n";
        return;
    }

    int searchID = getIntInput("Enter Patient ID to update: ");
    int result = binarySearch(patients, count, searchID);

    if (result == -1)
    {
        cout << "\nPatient with ID " << searchID << " not found.\n";
        return;
    }

    cout << "\nPatient found. Current info:\n";
    cout << "Name: "      << patients[result].name      << "\n";
    cout << "Age: "       << patients[result].age       << "\n";
    cout << "Gender: "    << patients[result].gender    << "\n";
    cout << "Diagnosis: " << patients[result].diagnosis << "\n";

    // Note: ID is intentionally NOT editable here, because changing it
    // would break the sorted order the binary search depends on.
    string newName = getLineInput("\nEnter new Name (leave blank to keep current): ");
    if (!newName.empty()) patients[result].name = newName;

    string ageStr = getLineInput("Enter new Age (leave blank to keep current): ");
    if (!ageStr.empty())
    {
        try { patients[result].age = stoi(ageStr); }
        catch (...) { cout << "Invalid age entered. Age not updated.\n"; }
    }

    string newGender = getLineInput("Enter new Gender (leave blank to keep current): ");
    if (!newGender.empty()) patients[result].gender = newGender;

    string newDiagnosis = getLineInput("Enter new Diagnosis (leave blank to keep current): ");
    if (!newDiagnosis.empty()) patients[result].diagnosis = newDiagnosis;

    cout << "\nPatient record updated successfully!\n";
}

// ---------------------------------------------------------
// Step 8: Save all patients to a text file
// Each patient is written as one comma-separated line:
// id,name,age,gender,diagnosis
// ---------------------------------------------------------
void savePatientsToFile(Patient* patients, int count, const string& filename)
{
    ofstream outFile(filename);

    if (!outFile)
    {
        cout << "\nError: Could not open file for writing.\n";
        return;
    }

    for (int i = 0; i < count; i++)
    {
        outFile << patients[i].id << ","
                << patients[i].name << ","
                << patients[i].age << ","
                << patients[i].gender << ","
                << patients[i].diagnosis << "\n";
    }

    outFile.close(); // close the file object
    cout << "\n" << count << " patient record(s) saved to " << filename << ".\n";
}

// ---------------------------------------------------------
// Loads patients from file into the dynamic array at startup.
// Returns the number of patients loaded.
// Assumes the file (if present) is already sorted by ID, since
// it was produced by this same program's sorted-insert add logic.
// ---------------------------------------------------------
int loadPatientsFromFile(Patient*& patients, int capacity, const string& filename)
{
    ifstream inFile(filename);
    int count = 0;

    if (!inFile)
        return 0; // file doesn't exist yet - that's fine on first run

    string line;
    while (getline(inFile, line) && count < capacity)
    {
        if (line.empty()) continue;

        stringstream ss(line);
        string field;
        Patient p;

        getline(ss, field, ','); p.id = stoi(field);
        getline(ss, field, ','); p.name = field;
        getline(ss, field, ','); p.age = stoi(field);
        getline(ss, field, ','); p.gender = field;
        getline(ss, field, ','); p.diagnosis = field;

        patients[count] = p;
        count++;
    }

    inFile.close(); // close the file object
    return count;
}