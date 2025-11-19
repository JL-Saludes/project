#include <iostream>
#include <vector>

void encrypted() {
    int key;
    cout << "Enter key: " << endl;
    cin >> key;

    if (key == 12345) {
        cout << "Access granted: text decreypted" << endl;
    } else {
        cout << "Access denied: incorrect key" << endl;
        return;
    }
}


using namespace std;

int main() {

    while (true){
        vector<string> notepad_text = {};

        if (notepad_text.empty()) {
            cout << "The notepad is currently empty." << endl;
        } else {
            cout << "Current notepad content:" << endl;
            for (const auto& line : notepad_text) {
                cout << line << endl;
            }


        }

        cout
            << "Welcome to the Notepad application!" << endl
            << "[1] Add Text" << endl
            << "[2] Display Text" << endl
            << "[3] Clear Text" << endl
            << "[4] Exit" << endl;

        int choice;
        cin >> choice;
        cin.ignore(); // To ignore the newline character after the integer input

        switch (choice)
        {
        case 1: {
            cout << "Enter text to add (type 'END' on a new line to finish):" << endl;
            string line;
            while (true) {
                getline(cin, line);
                if (line == "END") {
                    break;
                }
                notepad_text.push_back(line);
            }
            break;
        }
        case 2: {
            if (notepad_text.empty()) {
                cout << "The notepad is empty." << endl;
            } else {
                cout << "Notepad content:" << endl;
                for (const auto& line : notepad_text) {
                    cout << line << endl;
                }
            }
            break;
        }
        case 3:
            notepad_text.clear();
            cout << "Notepad cleared." << endl;
            break;
        case 4:
            cout << "Exiting the Notepad application. Goodbye!" << endl;
            return 0;

        case 123:
            encrypted();

            break;
        
        default:
            break;
        }


    }
    return 0;
}
