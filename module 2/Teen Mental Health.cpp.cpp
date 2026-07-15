----- CODE -----

// Module 7 - Working with External Data in C++
// Dataset: Teen Mental Health Dataset (Kaggle)
// Reads Teen_Mental_Health_Dataset.csv and displays a few fields for each entry.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// One row of the dataset (one survey respondent)
class TeenSurveyEntry {
public:
    int age;
    string gender;
    double dailySocialMediaHours;
    double sleepHours;
    int stressLevel;
    int anxietyLevel;

    void display() const {
        cout << "----------------------------------------\n";
        cout << "Age:                     " << age << "\n";
        cout << "Gender:                  " << gender << "\n";
        cout << "Daily Social Media (hrs): " << dailySocialMediaHours << "\n";
        cout << "Sleep (hrs):             " << sleepHours << "\n";
        cout << "Stress Level (1-10):     " << stressLevel << "\n";
        cout << "Anxiety Level (1-10):    " << anxietyLevel << "\n";
    }
};

// Splits one CSV line into fields by comma
vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    stringstream ss(line);
    string field;
    while (getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

// Reads the CSV file and returns a vector of entries.
// A vector is used here instead of a fixed-size array because we don't know
// how many rows the file has ahead of time (this one happens to have 1200,
// but the code shouldn't care) - vectors grow automatically as we push_back.
vector<TeenSurveyEntry> loadEntries(const string& filename) {
    vector<TeenSurveyEntry> entries;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: could not open file " << filename << endl;
        return entries;
    }

    string line;
    bool isHeader = true;

    // Column positions based on the CSV header row
    const int COL_AGE = 0;
    const int COL_GENDER = 1;
    const int COL_SOCIAL_MEDIA_HOURS = 2;
    const int COL_SLEEP_HOURS = 4;
    const int COL_STRESS_LEVEL = 9;
    const int COL_ANXIETY_LEVEL = 10;

    while (getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue; // the first line is just column names (age, gender, etc.),
                       // not actual data, so we skip it and move to the real rows
        }
        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);
        if (fields.size() <= (size_t)COL_ANXIETY_LEVEL) continue; // skip bad rows

        TeenSurveyEntry entry;

        // stoi/stod convert text to numbers, but they throw an exception if
        // a field is empty or not actually a number (bad data happens in
        // real datasets). Wrapping each conversion in try/catch means one
        // messy row just falls back to 0 instead of crashing the whole program.
        try { entry.age = stoi(fields[COL_AGE]); } catch (...) { entry.age = 0; }
        entry.gender = fields[COL_GENDER];
        try { entry.dailySocialMediaHours = stod(fields[COL_SOCIAL_MEDIA_HOURS]); } catch (...) { entry.dailySocialMediaHours = 0.0; }
        try { entry.sleepHours = stod(fields[COL_SLEEP_HOURS]); } catch (...) { entry.sleepHours = 0.0; }
        try { entry.stressLevel = stoi(fields[COL_STRESS_LEVEL]); } catch (...) { entry.stressLevel = 0; }
        try { entry.anxietyLevel = stoi(fields[COL_ANXIETY_LEVEL]); } catch (...) { entry.anxietyLevel = 0; }

        entries.push_back(entry);
    }

    file.close();
    return entries;
}

int main() {
    string filename = "Teen_Mental_Health_Dataset.csv";
    vector<TeenSurveyEntry> entries = loadEntries(filename);

    cout << "Loaded " << entries.size() << " entries from " << filename << "\n";
    cout << "========================================\n";

    // Show the first 15 entries so the output stays readable
    int rowsToShow = 15;
    for (int i = 0; i < rowsToShow && i < (int)entries.size(); i++) {
        entries[i].display();
    }

    cout << "----------------------------------------\n";
    return 0;
}
