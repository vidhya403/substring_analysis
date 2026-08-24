#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cctype>

using namespace std;

// ============================================================
//                    CONFIGURATION
// ============================================================

const int MAX_LEVEL = 8;
const double PROBABILITY = 0.5;

// ============================================================
//                    SKIP LIST NODE
// ============================================================

class Node {
public:
    string key;
    vector<Node*> forward;

    Node(const string& value, int level)
        : key(value), forward(level + 1, nullptr) {}
};

// ============================================================
//                    SKIP LIST CLASS
// ============================================================

class SkipList {
private:
    Node* header;
    int currentLevel;

    int randomLevel() const {
        int level = 0;

        while ((double)rand() / RAND_MAX < PROBABILITY &&
               level < MAX_LEVEL) {
            level++;
        }

        return level;
    }

public:

    // Constructor
    SkipList() {
        currentLevel = 0;
        header = new Node("", MAX_LEVEL);
    }

    // Destructor
    ~SkipList() {
        clear();
        delete header;
    }

    // --------------------------------------------------------
    // INSERT
    // --------------------------------------------------------

    bool insert(const string& key) {

        vector<Node*> update(MAX_LEVEL + 1, nullptr);

        Node* current = header;

        for (int i = currentLevel; i >= 0; i--) {

            while (current->forward[i] != nullptr &&
                   current->forward[i]->key < key) {

                current = current->forward[i];
            }

            update[i] = current;
        }

        current = current->forward[0];

        // Duplicate check
        if (current != nullptr && current->key == key) {
            return false;
        }

        int newLevel = randomLevel();

        if (newLevel > currentLevel) {

            for (int i = currentLevel + 1; i <= newLevel; i++) {
                update[i] = header;
            }

            currentLevel = newLevel;
        }

        Node* newNode = new Node(key, newLevel);

        for (int i = 0; i <= newLevel; i++) {

            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }

        return true;
    }

    // --------------------------------------------------------
    // SEARCH
    // --------------------------------------------------------

    bool search(const string& key) const {

        Node* current = header;

        for (int i = currentLevel; i >= 0; i--) {

            while (current->forward[i] != nullptr &&
                   current->forward[i]->key < key) {

                current = current->forward[i];
            }
        }

        current = current->forward[0];

        return current != nullptr &&
               current->key == key;
    }

    // --------------------------------------------------------
    // DELETE
    // --------------------------------------------------------

    bool remove(const string& key) {

        vector<Node*> update(MAX_LEVEL + 1, nullptr);

        Node* current = header;

        for (int i = currentLevel; i >= 0; i--) {

            while (current->forward[i] != nullptr &&
                   current->forward[i]->key < key) {

                current = current->forward[i];
            }

            update[i] = current;
        }

        current = current->forward[0];

        if (current == nullptr ||
            current->key != key) {

            return false;
        }

        for (int i = 0; i <= currentLevel; i++) {

            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] =
                current->forward[i];
        }

        delete current;

        while (currentLevel > 0 &&
               header->forward[currentLevel] == nullptr) {

            currentLevel--;
        }

        return true;
    }

    // --------------------------------------------------------
    // DISPLAY
    // --------------------------------------------------------

    void display() const {

        Node* current = header->forward[0];

        int count = 1;

        cout << "\n";
        cout << "============================================\n";
        cout << "             UNIQUE SUBSTRINGS\n";
        cout << "============================================\n";

        while (current != nullptr) {

            cout << setw(4) << count++
                 << ". " << current->key << endl;

            current = current->forward[0];
        }

        cout << "============================================\n";
    }

    // --------------------------------------------------------
    // COUNT NODES
    // --------------------------------------------------------

    int size() const {

        int count = 0;

        Node* current = header->forward[0];

        while (current != nullptr) {

            count++;

            current = current->forward[0];
        }

        return count;
    }

    // --------------------------------------------------------
    // CLEAR
    // --------------------------------------------------------

    void clear() {

        Node* current = header->forward[0];

        while (current != nullptr) {

            Node* next = current->forward[0];

            delete current;

            current = next;
        }

        for (int i = 0; i <= MAX_LEVEL; i++) {
            header->forward[i] = nullptr;
        }

        currentLevel = 0;
    }

    // --------------------------------------------------------
    // GET ALL SUBSTRINGS
    // --------------------------------------------------------

    vector<string> getAll() const {

        vector<string> result;

        Node* current = header->forward[0];

        while (current != nullptr) {

            result.push_back(current->key);

            current = current->forward[0];
        }

        return result;
    }
};

// ============================================================
//                 STRING UTILITY FUNCTIONS
// ============================================================

// Convert string to lowercase
string toLowerCase(const string& text) {

    string result = text;

    for (char& c : result) {
        c = tolower(static_cast<unsigned char>(c));
    }

    return result;
}

// ------------------------------------------------------------
// Generate all substrings
// ------------------------------------------------------------

void generateSubstrings(SkipList& skipList,
                        const string& text) {

    for (size_t i = 0; i < text.length(); i++) {

        string substring = "";

        for (size_t j = i; j < text.length(); j++) {

            substring += text[j];

            skipList.insert(substring);
        }
    }
}

// ------------------------------------------------------------
// Count occurrences
// ------------------------------------------------------------

int countOccurrences(const string& text,
                     const string& pattern) {

    if (pattern.empty())
        return 0;

    int count = 0;

    size_t position = text.find(pattern);

    while (position != string::npos) {

        count++;

        position = text.find(pattern,
                              position + 1);
    }

    return count;
}

// ------------------------------------------------------------
// Find all positions
// ------------------------------------------------------------

vector<int> findPositions(const string& text,
                          const string& pattern) {

    vector<int> positions;

    if (pattern.empty())
        return positions;

    size_t position = text.find(pattern);

    while (position != string::npos) {

        // Position starts from 1
        positions.push_back(
            static_cast<int>(position) + 1
        );

        position = text.find(
            pattern,
            position + 1
        );
    }

    return positions;
}

// ------------------------------------------------------------
// Longest repeated substring
// ------------------------------------------------------------

string longestRepeatedSubstring(const string& text) {

    string longest = "";

    for (size_t i = 0; i < text.length(); i++) {

        string substring = "";

        for (size_t j = i; j < text.length(); j++) {

            substring += text[j];

            if (substring.length() <= longest.length())
                continue;

            int occurrences =
                countOccurrences(text, substring);

            if (occurrences >= 2) {

                longest = substring;
            }
        }
    }

    return longest;
}

// ============================================================
//                  DISPLAY STATISTICS
// ============================================================

void displayStatistics(const string& text,
                       const SkipList& skipList) {

    int n = static_cast<int>(text.length());

    int totalPossible =
        n * (n + 1) / 2;

    int uniqueSubstrings =
        skipList.size();

    string longestRepeated =
        longestRepeatedSubstring(text);

    cout << "\n";
    cout << "============================================\n";
    cout << "             STRING STATISTICS\n";
    cout << "============================================\n";

    cout << "Main String             : "
         << text << endl;

    cout << "String Length           : "
         << n << endl;

    cout << "Total Possible Substrings: "
         << totalPossible << endl;

    cout << "Unique Substrings       : "
         << uniqueSubstrings << endl;

    cout << "Duplicate Substrings    : "
         << totalPossible - uniqueSubstrings << endl;

    cout << "Skip List Levels        : "
         << MAX_LEVEL + 1 << endl;

    if (!longestRepeated.empty()) {

        cout << "Longest Repeated String : "
             << longestRepeated << endl;

        cout << "Occurrences             : "
             << countOccurrences(
                    text,
                    longestRepeated)
             << endl;
    }
    else {

        cout << "Longest Repeated String : None\n";
    }

    cout << "============================================\n";
}

// ============================================================
//                  CHARACTER FREQUENCY
// ============================================================

void characterFrequency(const string& text) {

    int frequency[256] = {0};

    for (char c : text) {

        frequency[
            static_cast<unsigned char>(c)
        ]++;
    }

    cout << "\n";
    cout << "============================================\n";
    cout << "           CHARACTER FREQUENCY\n";
    cout << "============================================\n";

    for (int i = 0; i < 256; i++) {

        if (frequency[i] > 0) {

            cout << "'" << static_cast<char>(i)
                 << "'  ->  "
                 << frequency[i]
                 << endl;
        }
    }

    cout << "============================================\n";
}

// ============================================================
//                  SAVE RESULTS
// ============================================================

void saveResults(
    const string& mainString,
    const SkipList& skipList,
    const string& lastSearch,
    const string& lastDelete) {

    ofstream file("substring_analysis.txt");

    if (!file) {

        cout << "Error: Unable to create output file.\n";
        return;
    }

    file << "============================================\n";
    file << "       SUBSTRING ANALYSIS REPORT\n";
    file << "============================================\n\n";

    file << "Main String       : "
         << mainString << "\n";

    file << "String Length     : "
         << mainString.length() << "\n";

    file << "Unique Substrings : "
         << skipList.size() << "\n";

    file << "Last Search       : "
         << (lastSearch.empty()
             ? "None"
             : lastSearch)
         << "\n";

    file << "Last Deleted      : "
         << (lastDelete.empty()
             ? "None"
             : lastDelete)
         << "\n\n";

    file << "--------------------------------------------\n";
    file << "ALL UNIQUE SUBSTRINGS\n";
    file << "--------------------------------------------\n";

    vector<string> substrings =
        skipList.getAll();

    int index = 1;

    for (const string& s : substrings) {

        file << index++
             << ". "
             << s
             << "\n";
    }

    file << "\n--------------------------------------------\n";
    file << "LONGEST REPEATED SUBSTRING\n";
    file << "--------------------------------------------\n";

    string longest =
        longestRepeatedSubstring(mainString);

    if (longest.empty()) {

        file << "None\n";

    } else {

        file << "Substring  : "
             << longest << "\n";

        file << "Count      : "
             << countOccurrences(
                    mainString,
                    longest)
             << "\n";
    }

    file << "\n============================================\n";

    file.close();

    cout << "\nResults saved successfully to "
         << "substring_analysis.txt\n";
}

// ============================================================
//                       MAIN FUNCTION
// ============================================================

int main() {

    srand(static_cast<unsigned int>(time(nullptr)));

    SkipList skipList;

    string mainString;

    cout << "\n";
    cout << "============================================\n";
    cout << "     SUBSTRING SEARCH & ANALYSIS ENGINE\n";
    cout << "             USING SKIP LIST\n";
    cout << "============================================\n";

    cout << "\nEnter main string: ";
    cin >> mainString;

    // Generate substrings
    generateSubstrings(skipList, mainString);

    cout << "\n";
    cout << "Substrings generated successfully!\n";

    cout << "Unique substrings stored: "
         << skipList.size()
         << endl;

    string lastSearch = "";
    string lastDelete = "";

    int choice;

    do {

        cout << "\n";
        cout << "================ MENU ================\n";
        cout << "1. Display all unique substrings\n";
        cout << "2. Search substring\n";
        cout << "3. Count substring occurrences\n";
        cout << "4. Find substring positions\n";
        cout << "5. Delete substring\n";
        cout << "6. Character frequency analysis\n";
        cout << "7. Display string statistics\n";
        cout << "8. Find longest repeated substring\n";
        cout << "9. Save analysis report\n";
        cout << "10. Clear Skip List\n";
        cout << "0. Exit\n";
        cout << "======================================\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        // ----------------------------------------------------
        // DISPLAY
        // ----------------------------------------------------

        case 1:

            skipList.display();

            break;

        // ----------------------------------------------------
        // SEARCH
        // ----------------------------------------------------

        case 2: {

            string pattern;

            cout << "Enter substring to search: ";
            cin >> pattern;

            if (skipList.search(pattern)) {

                cout << "\n[FOUND]\n";
                cout << "Substring \"" << pattern
                     << "\" exists in Skip List.\n";

            } else {

                cout << "\n[NOT FOUND]\n";
                cout << "Substring \"" << pattern
                     << "\" does not exist.\n";
            }

            lastSearch = pattern;

            break;
        }

        // ----------------------------------------------------
        // COUNT
        // ----------------------------------------------------

        case 3: {

            string pattern;

            cout << "Enter substring: ";
            cin >> pattern;

            int count =
                countOccurrences(
                    mainString,
                    pattern);

            cout << "\nSubstring: "
                 << pattern << endl;

            cout << "Occurrences: "
                 << count << endl;

            break;
        }

        // ----------------------------------------------------
        // POSITIONS
        // ----------------------------------------------------

        case 4: {

            string pattern;

            cout << "Enter substring: ";
            cin >> pattern;

            vector<int> positions =
                findPositions(
                    mainString,
                    pattern);

            cout << "\nPositions of \""
                 << pattern
                 << "\": ";

            if (positions.empty()) {

                cout << "Not found";

            } else {

                for (int position : positions) {

                    cout << position << " ";
                }
            }

            cout << endl;

            break;
        }

        // ----------------------------------------------------
        // DELETE
        // ----------------------------------------------------

        case 5: {

            string pattern;

            cout << "Enter substring to delete: ";
            cin >> pattern;

            if (skipList.remove(pattern)) {

                cout << "\nSubstring deleted successfully.\n";

                lastDelete = pattern;

            } else {

                cout << "\nSubstring not found in Skip List.\n";
            }

            break;
        }

        // ----------------------------------------------------
        // CHARACTER FREQUENCY
        // ----------------------------------------------------

        case 6:

            characterFrequency(mainString);

            break;

        // ----------------------------------------------------
        // STATISTICS
        // ----------------------------------------------------

        case 7:

            displayStatistics(
                mainString,
                skipList);

            break;

        // ----------------------------------------------------
        // LONGEST REPEATED SUBSTRING
        // ----------------------------------------------------

        case 8: {

            string longest =
                longestRepeatedSubstring(
                    mainString);

            cout << "\n";
            cout << "============================================\n";

            if (longest.empty()) {

                cout << "No repeated substring found.\n";

            } else {

                cout << "Longest Repeated Substring : "
                     << longest << endl;

                cout << "Length                    : "
                     << longest.length() << endl;

                cout << "Occurrences               : "
                     << countOccurrences(
                            mainString,
                            longest)
                     << endl;
            }

            cout << "============================================\n";

            break;
        }

        // ----------------------------------------------------
        // SAVE
        // ----------------------------------------------------

        case 9:

            saveResults(
                mainString,
                skipList,
                lastSearch,
                lastDelete);

            break;

        // ----------------------------------------------------
        // CLEAR
        // ----------------------------------------------------

        case 10:

            skipList.clear();

            cout << "\nSkip List cleared successfully.\n";

            break;

        // ----------------------------------------------------
        // EXIT
        // ----------------------------------------------------

        case 0:

            cout << "\nThank you for using the system!\n";
            cout << "Exiting...\n";

            break;

        default:

            cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}