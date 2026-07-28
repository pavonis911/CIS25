// Our Story Trivia
// A trivia game based on my relationship.
// Questions get saved to a file (questions.txt) so they're still there
// next time you run it, and you can add/edit/delete them from the menu.
// Scores get saved too (history.txt) so we can track them over time.
//
// Author: Zaki El-Mueed

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <limits>

using namespace std;

const string QUESTIONS_FILE = "questions.txt";
const string HISTORY_FILE   = "history.txt";
const char   DELIM          = '|';   // splits each line in the data files

struct Question {
    int    id;
    string category;
    string questionText;
    string answer;   // stored in original case for display if ever needed
    string memory;   // message/memory revealed when answered correctly
};

struct HistoryEntry {
    string date;
    string category;
    int    score;
    int    total;
};

// gets rid of extra spaces before/after what the user typed
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// makes a string all lowercase so I can compare answers without
// worrying about capital letters
string toLower(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return tolower(c); });
    return result;
}

// swaps out the '|' character if someone types it, since that's what
// I use to separate fields in the data files
string sanitize(const string& s) {
    string result = s;
    replace(result.begin(), result.end(), DELIM, '/');
    return result;
}

// cleans up cin after bad input so it doesn't mess up the next read
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// keeps asking until the user types a valid number in range
int getValidatedIntChoice(const string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << prompt;
        if (cin >> choice && choice >= minVal && choice <= maxVal) {
            clearInputBuffer();
            return choice;
        }
        if (cin.eof()) {
            // just in case input ever runs out, exit instead of looping forever
            cout << "\nNo more input available. Exiting.\n";
            exit(0);
        }
        cout << "  Invalid choice. Please enter a number between "
             << minVal << " and " << maxVal << ".\n";
        clearInputBuffer();
    }
}

// reads a full line and asks again if it's left blank
string getValidatedLine(const string& prompt) {
    string line;
    while (true) {
        cout << prompt;
        getline(cin, line);
        if (cin.eof() && line.empty()) {
            cout << "\nNo more input available. Exiting.\n";
            exit(0);
        }
        line = trim(line);
        if (!line.empty()) return sanitize(line);
        cout << "  This can't be empty. Please try again.\n";
    }
}

// grabs today's date so I can stamp it on each score in history.txt
string getTodayDate() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    ostringstream oss;
    oss << (now->tm_year + 1900) << "-"
        << (now->tm_mon + 1 < 10 ? "0" : "") << (now->tm_mon + 1) << "-"
        << (now->tm_mday < 10 ? "0" : "") << now->tm_mday;
    return oss.str();
}

// Handles everything to do with the trivia questions themselves -
// loading them from the file, saving them, and adding/updating/deleting.
class QuestionBank {
private:
    vector<Question> questions;
    int nextId = 1;

    // breaks a line from the file into its separate pieces
    vector<string> splitLine(const string& line) {
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, DELIM)) fields.push_back(field);
        return fields;
    }

public:
    // loads questions from questions.txt (if it's the first time running
    // the program, that file won't exist yet, so we just start empty)
    void load() {
        questions.clear();
        ifstream inFile(QUESTIONS_FILE);
        if (!inFile.is_open()) return;

        string line;
        int maxId = 0;
        while (getline(inFile, line)) {
            if (trim(line).empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 5) continue; // skip malformed lines
            Question q;
            q.id = stoi(f[0]);
            q.category = f[1];
            q.questionText = f[2];
            q.answer = f[3];
            q.memory = f[4];
            questions.push_back(q);
            maxId = max(maxId, q.id);
        }
        inFile.close();
        nextId = maxId + 1;
    }

    // rewrites questions.txt with whatever is currently in memory
    void save() {
        ofstream outFile(QUESTIONS_FILE, ios::trunc);
        for (const auto& q : questions) {
            outFile << q.id << DELIM << q.category << DELIM << q.questionText
                    << DELIM << q.answer << DELIM << q.memory << "\n";
        }
        outFile.close();
    }

    bool isEmpty() const { return questions.empty(); }
    int  count() const { return static_cast<int>(questions.size()); }

    // loads our real questions the first time the game is ever run
    void seedDefaultsIfEmpty() {
        if (!questions.empty()) return;
        addQuestion("First Date", "Where did we go on our first date?",
                    "Drive in movie", "I remember not looking at you for like the whole date :)");
        addQuestion("First Date", "What movie did we watch on our first date?",
                    "How to Train Your Dragon", "I picked this movie because I really did NOT want to see Lilo & Stitch");
        addQuestion("Inside Jokes", "What was our first inside joke?",
                    "spread em", "I still think it was you that said it first");
        addQuestion("Milestones", "When did we start dating?",
                    "7/21/2025", "I like you... like a lot!!");
        save();
    }

    // adds a new question and immediately saves it to file
    void addQuestion(const string& category, const string& questionText,
                      const string& answer, const string& memory) {
        Question q{ nextId++, category, questionText, answer, memory };
        questions.push_back(q);
        save();
    }

    const vector<Question>& getAll() const { return questions; }

    // grabs just the questions in one category (not case sensitive)
    vector<Question> getByCategory(const string& category) const {
        vector<Question> result;
        for (const auto& q : questions) {
            if (toLower(q.category) == toLower(category)) result.push_back(q);
        }
        return result;
    }

    // returns each category name once (sorted) so the play menu can list them
    vector<string> getCategories() const {
        vector<string> cats;
        for (const auto& q : questions) {
            if (find(cats.begin(), cats.end(), q.category) == cats.end())
                cats.push_back(q.category);
        }
        sort(cats.begin(), cats.end());
        return cats;
    }

    // finds the question with this id and overwrites its fields
    bool updateQuestion(int id, const string& category, const string& questionText,
                         const string& answer, const string& memory) {
        for (auto& q : questions) {
            if (q.id == id) {
                q.category = category;
                q.questionText = questionText;
                q.answer = answer;
                q.memory = memory;
                save();
                return true;
            }
        }
        return false;
    }

    // removes the question with this id, if it exists
    bool deleteQuestion(int id) {
        auto it = remove_if(questions.begin(), questions.end(),
                             [id](const Question& q) { return q.id == id; });
        if (it == questions.end()) return false;
        questions.erase(it, questions.end());
        save();
        return true;
    }

    // looks up a question by its id so I can edit it in place
    Question* findById(int id) {
        for (auto& q : questions) {
            if (q.id == id) return &q;
        }
        return nullptr;
    }
};

// Keeps track of every round played, saved to history.txt
class HistoryLog {
public:
    // adds one more line to history.txt after a round finishes
    void addEntry(const string& category, int score, int total) {
        ofstream outFile(HISTORY_FILE, ios::app);
        outFile << getTodayDate() << DELIM << category << DELIM
                << score << DELIM << total << "\n";
        outFile.close();
    }

    // reads every past round back out of history.txt
    vector<HistoryEntry> loadAll() {
        vector<HistoryEntry> entries;
        ifstream inFile(HISTORY_FILE);
        if (!inFile.is_open()) return entries;

        string line;
        while (getline(inFile, line)) {
            if (trim(line).empty()) continue;
            stringstream ss(line);
            string field;
            vector<string> f;
            while (getline(ss, field, DELIM)) f.push_back(field);
            if (f.size() < 4) continue;
            HistoryEntry h;
            h.date = f[0];
            h.category = f[1];
            h.score = stoi(f[2]);
            h.total = stoi(f[3]);
            entries.push_back(h);
        }
        inFile.close();
        return entries;
    }
};

void printDivider() {
    cout << "----------------------------------------------------\n";
}

void printQuestionRow(const Question& q) {
    cout << "  [" << q.id << "] (" << q.category << ") " << q.questionText
         << "  ->  Answer: " << q.answer << "\n";
}

void viewAllQuestions(QuestionBank& bank) {
    printDivider();
    cout << "All Saved Questions:\n";
    if (bank.isEmpty()) {
        cout << "  (No questions saved yet.)\n";
    } else {
        for (const auto& q : bank.getAll()) printQuestionRow(q);
    }
    printDivider();
}

void addQuestionFlow(QuestionBank& bank) {
    cout << "\n-- Add a New Question --\n";
    string category = getValidatedLine("Category (e.g. First Date, Milestones): ");
    string questionText = getValidatedLine("Question: ");
    string answer = getValidatedLine("Correct answer: ");
    string memory = getValidatedLine("Memory/message to unlock when correct: ");
    bank.addQuestion(category, questionText, answer, memory);
    cout << "Question added and saved!\n";
}

void updateQuestionFlow(QuestionBank& bank) {
    if (bank.isEmpty()) {
        cout << "There are no questions to update yet.\n";
        return;
    }
    viewAllQuestions(bank);
    int id = getValidatedIntChoice("Enter the ID of the question to update: ", 1, 1000000);
    Question* q = bank.findById(id);
    if (!q) {
        cout << "No question found with that ID.\n";
        return;
    }
    cout << "Leave a field blank (press Enter) to keep its current value.\n";

    cout << "Category [" << q->category << "]: ";
    string category; getline(cin, category); category = trim(category);
    if (category.empty()) category = q->category; else category = sanitize(category);

    cout << "Question [" << q->questionText << "]: ";
    string questionText; getline(cin, questionText); questionText = trim(questionText);
    if (questionText.empty()) questionText = q->questionText; else questionText = sanitize(questionText);

    cout << "Answer [" << q->answer << "]: ";
    string answer; getline(cin, answer); answer = trim(answer);
    if (answer.empty()) answer = q->answer; else answer = sanitize(answer);

    cout << "Memory/message [" << q->memory << "]: ";
    string memory; getline(cin, memory); memory = trim(memory);
    if (memory.empty()) memory = q->memory; else memory = sanitize(memory);

    bank.updateQuestion(id, category, questionText, answer, memory);
    cout << "Question updated and saved!\n";
}

void deleteQuestionFlow(QuestionBank& bank) {
    if (bank.isEmpty()) {
        cout << "There are no questions to delete yet.\n";
        return;
    }
    viewAllQuestions(bank);
    int id = getValidatedIntChoice("Enter the ID of the question to delete: ", 1, 1000000);
    cout << "Are you sure? (1 = Yes, 2 = Cancel): ";
    int confirm = getValidatedIntChoice("", 1, 2);
    if (confirm == 1) {
        if (bank.deleteQuestion(id)) cout << "Question deleted.\n";
        else cout << "No question found with that ID.\n";
    } else {
        cout << "Cancelled.\n";
    }
}

void manageQuestionsMenu(QuestionBank& bank) {
    while (true) {
        printDivider();
        cout << "Manage Questions\n";
        cout << "1. View all questions\n";
        cout << "2. Add a question\n";
        cout << "3. Update a question\n";
        cout << "4. Delete a question\n";
        cout << "5. Back to main menu\n";
        int choice = getValidatedIntChoice("Choose an option: ", 1, 5);

        if (choice == 1) viewAllQuestions(bank);
        else if (choice == 2) addQuestionFlow(bank);
        else if (choice == 3) updateQuestionFlow(bank);
        else if (choice == 4) deleteQuestionFlow(bank);
        else if (choice == 5) return;
    }
}

void playRound(QuestionBank& bank, HistoryLog& history) {
    if (bank.isEmpty()) {
        cout << "There are no questions yet! Add some in 'Manage Questions' first.\n";
        return;
    }

    vector<string> categories = bank.getCategories();
    printDivider();
    cout << "Choose a category to play:\n";
    for (size_t i = 0; i < categories.size(); i++) {
        cout << "  " << (i + 1) << ". " << categories[i] << "\n";
    }
    int allOption = static_cast<int>(categories.size()) + 1;
    cout << "  " << allOption << ". All categories\n";

    int choice = getValidatedIntChoice("Your choice: ", 1, allOption);

    vector<Question> roundQuestions;
    string categoryLabel;
    if (choice == allOption) {
        roundQuestions = bank.getAll();
        categoryLabel = "All";
    } else {
        categoryLabel = categories[choice - 1];
        roundQuestions = bank.getByCategory(categoryLabel);
    }

    // shuffle so the questions aren't in the same order every time
    unsigned seed = static_cast<unsigned>(time(nullptr));
    shuffle(roundQuestions.begin(), roundQuestions.end(), default_random_engine(seed));

    int score = 0;
    int total = static_cast<int>(roundQuestions.size());

    printDivider();
    cout << "Starting round: " << categoryLabel << " (" << total << " question"
         << (total == 1 ? "" : "s") << ")\n";
    printDivider();

    for (size_t i = 0; i < roundQuestions.size(); i++) {
        const Question& q = roundQuestions[i];
        cout << "\nQ" << (i + 1) << " [" << q.category << "]: " << q.questionText << "\n";
        cout << "Your answer: ";
        string userAnswer;
        getline(cin, userAnswer);
        userAnswer = trim(userAnswer);

        if (toLower(userAnswer) == toLower(trim(q.answer))) {
            score++;
            cout << "Correct! 🎉\n";
            cout << "  Memory unlocked: \"" << q.memory << "\"\n";
        } else {
            cout << "Not quite. The answer was: " << q.answer << "\n";
        }
    }

    printDivider();
    cout << "Round complete! You got " << score << "/" << total;
    if (total > 0 && score == total) cout << " -- Here's to another year together! ❤️";
    else if (total > 0 && score >= (total + 1) / 2) cout << " -- Not bad, love! Let's brush up on our story. ❤️";
    else cout << " -- Looks like it's time for a re-date to refresh those memories! ❤️";
    cout << "\n";
    printDivider();

    history.addEntry(categoryLabel, score, total);
    cout << "Score saved to history!\n";
}

void viewHistory(HistoryLog& history) {
    vector<HistoryEntry> entries = history.loadAll();
    printDivider();
    cout << "Score History:\n";
    if (entries.empty()) {
        cout << "  No rounds played yet.\n";
    } else {
        for (const auto& h : entries) {
            cout << "  " << h.date << " | " << h.category
                 << " | Score: " << h.score << "/" << h.total << "\n";
        }
        // add everything up for a lifetime total
        int totalCorrect = 0, totalPossible = 0;
        for (const auto& h : entries) { totalCorrect += h.score; totalPossible += h.total; }
        printDivider();
        cout << "  Rounds played: " << entries.size()
             << " | Lifetime score: " << totalCorrect << "/" << totalPossible << "\n";
    }
    printDivider();
}

int main() {
    QuestionBank bank;
    HistoryLog history;

    bank.load();
    bank.seedDefaultsIfEmpty();

    cout << "======================================\n";
    cout << "         OUR STORY TRIVIA \n";
    cout << "======================================\n";
    cout << "Welcome! Test your memory of our story.\n";

    while (true) {
        printDivider();
        cout << "Main Menu\n";
        cout << "1. Play Trivia\n";
        cout << "2. Manage Questions (Add/View/Update/Delete)\n";
        cout << "3. View Score History\n";
        cout << "4. Exit\n";
        int choice = getValidatedIntChoice("Choose an option: ", 1, 4);

        if (choice == 1) playRound(bank, history);
        else if (choice == 2) manageQuestionsMenu(bank);
        else if (choice == 3) viewHistory(history);
        else if (choice == 4) {
            cout << "Thanks for playing!! I love you Bebe! ❤️\n";
            break;
        }
    }

    return 0;
}