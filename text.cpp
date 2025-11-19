#include <iostream>
#include <vector>
#include <string>

using namespace std;

// ------------------- Base Class -------------------
class Note {
public:
    virtual ~Note() {}
    virtual void display() const = 0;
    virtual string decryptWithKey() const = 0; 
};

// ------------------- Text Note -------------------
class TextNote : public Note {
private:
    string content;

public:
    TextNote(const string& text) : content(text) {}

    void display() const override {
        cout << "[Text Note]\n" << content << "\n";
    }

    string decryptWithKey() const override {
        return content;
    }
};

// ------------------- Caesar Cipher Helpers -------------------
string caesarEncrypt(const string& text, int shift) {
    string result;
    shift %= 26;

    for (char c : text) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            result += char((c - base + shift + 26) % 26 + base);
        } else {
            result += c;
        }
    }
    return result;
}

string caesarDecrypt(const string& text, int shift) {
    return caesarEncrypt(text, -shift);
}

// ------------------- Caesar Cipher Note -------------------
class CaesarCipherNote : public Note {
private:
    string encrypted;
    int shift;

public:
    CaesarCipherNote(const string& text, int shift)
        : shift(shift)
    {
        encrypted = caesarEncrypt(text, shift);
    }

    void display() const override {
        cout << "[Caesar Cipher Note] (Encrypted)\n";
        cout << encrypted << "\n";
    }

    string decryptWithKey() const override {
        int userShift;

        cout << "Enter decryption key (shift): ";
        cin >> userShift;
        cin.ignore();

        if (userShift != shift) {
            return "❌ ERROR: Incorrect key! Cannot decrypt this note.";
        }

        return caesarDecrypt(encrypted, shift);
    }
};

// ------------------- Notepad Manager -------------------
class Notepad {
private:
    vector<Note*> notes;

public:
    ~Notepad() {
        for (Note* n : notes) delete n;
    }

    void addNote(Note* note) {
        notes.push_back(note);
    }

    void displayAll() const {
        if (notes.empty()) {
            cout << "No notes available.\n";
            return;
        }

        cout << "\n--- ALL NOTES ---\n";
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << "Note #" << (i + 1) << ":\n";
            notes[i]->display();
            cout << "------------------\n";
        }
    }

    void viewNoteContent(int index) const {
        if (index < 0 || index >= notes.size()) {
            cout << "Invalid index.\n";
            return;
        }

        cout << "\nDecrypted Content:\n";
        cout << notes[index]->decryptWithKey() << "\n";
    }

    void removeNote(int index) {
        if (index < 0 || index >= notes.size()) {
            cout << "Invalid index.\n";
            return;
        }
        delete notes[index];
        notes.erase(notes.begin() + index);
        cout << "Note removed.\n";
    }

    void clearAllNotes() {
        for (Note* n : notes) delete n;
        notes.clear();
        cout << "All notes cleared.\n";
    }
};

// =============================================================
//                         MAIN PROGRAM
// =============================================================
int main() {
    Notepad pad;
    int choice;

    while (true) {
        cout << "\n--- Notepad Menu ---\n"
             << "1. Add Text Note\n"
             << "2. Add Caesar Cipher Note\n"
             << "3. Display All Notes\n"
             << "4. View Decrypted Note (requires key)\n"
             << "5. Remove Specific Note\n"
             << "6. Clear All Notes\n"
             << "7. Exit\n"
             << "Choose: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            string text;
            cout << "Enter note text: ";
            getline(cin, text);

            pad.addNote(new TextNote(text));
        }

        else if (choice == 2) {
            string text;
            int shift;

            cout << "Enter note text: ";
            getline(cin, text);

            cout << "Enter Caesar cipher key (shift number): ";
            cin >> shift;
            cin.ignore();

            pad.addNote(new CaesarCipherNote(text, shift));
        }

        else if (choice == 3) {
            pad.displayAll();
        }

        else if (choice == 4) {
            int index;
            cout << "Enter note number: ";
            cin >> index;
            pad.viewNoteContent(index - 1);
        }

        else if (choice == 5) {
            int index;
            cout << "Enter note number to remove: ";
            cin >> index;
            pad.removeNote(index - 1);
        }

        else if (choice == 6) {
            pad.clearAllNotes();
        }

        else if (choice == 7) {
            cout << "Goodbye!\n";
            break;
        }

        else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}
