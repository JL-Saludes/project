#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ===== Base class (Abstraction + Polymorphism) =====
class Note {
protected:
    string title;
    string text;

public:
    Note(string title, string text) : title(title), text(text) {}

    string getTitle() { return title; }

    virtual void display() = 0;  // abstract (pure virtual)
    virtual bool isEncrypted() = 0;  // Return true if encrypted
};

// ===== Normal Note =====
class NormalNote : public Note {
public:
    NormalNote(string title, string text) : Note(title, text) {}

    void display() override {   // Polymorphism
        cout << "Title: " << title << " | " << text << endl;
    }

    bool isEncrypted() override {
        return false;  // Normal notes are not encrypted
    }
};

// ===== Encrypted Note =====
class EncryptedNote : public Note {
private:
    int key;

    string decrypt(string s, int k) {
        string result = "";
        for (char c : s) result += char(c - k);
        return result;
    }

public:
    EncryptedNote(string title, string text, int key) : Note(title, text), key(key) {}

    void display() override {   // Polymorphism
        cout << "Title: " << title << " | [Encrypted]" << endl;
    }

    bool isEncrypted() override {
        return true;  // Encrypted notes are flagged as encrypted
    }

    string getEncryptedText() {
        return text;  // Return encrypted text for decryption
    }

    int getKey() { return key; }
};

// ===== Note Manager =====
class NoteManager {
private:
    vector<Note*> notes;

    string encrypt(string s, int k) {
        string result = "";
        for (char c : s) result += char(c + k);
        return result;
    }

public:
    // Create normal note
    void createNote() {
        string title, text;
        cout << "Enter note title: ";
        cin.ignore();  // Clear input buffer
        getline(cin, title);

        cout << "Enter note text: ";
        getline(cin, text);

        notes.push_back(new NormalNote(title, text));
        cout << "Normal note \"" << title << "\" added!\n";
    }

    // Create encrypted note
    void createEncryptedNote() {
        string title, text;
        int key;

        cout << "Enter note title: ";
        cin.ignore();  // Clear input buffer
        getline(cin, title);

        cout << "Enter text to encrypt: ";
        getline(cin, text);

        cout << "Enter encryption key: ";
        cin >> key;

        string encrypted = encrypt(text, key);

        notes.push_back(new EncryptedNote(title, encrypted, key));
        cout << "Encrypted note \"" << title << "\" added!\n";
    }

    // Display all notes
    void viewNotes() {
        if (notes.empty()) {
            cout << "No notes available.\n";
            return;
        }

        cout << "\n===== All Notes =====\n";
        for (int i = 0; i < notes.size(); i++) {
            cout << "[" << i + 1 << "] ";
            notes[i]->display();
            cout << "-------------------\n";
        }
    }

    // Decrypt a selected note
    void decryptNote() {
        int index;
        cout << "Enter the index of the note to decrypt (1 to " << notes.size() << "): ";
        cin >> index;

        if (index < 1 || index > notes.size()) {
            cout << "Invalid index.\n";
            return;
        }

        Note* selectedNote = notes[index - 1];

        if (selectedNote->isEncrypted()) {
            int key;
            cout << "Note is encrypted. Enter decryption key: ";
            cin >> key;

            EncryptedNote* encNote = dynamic_cast<EncryptedNote*>(selectedNote); // Downcasting
            if (encNote && encNote->getKey() == key) {
                cout << "Decrypted Note: " << encNote->getEncryptedText() << endl;
            } else {
                cout << "Incorrect key! Cannot decrypt the note.\n";
            }
        } else {
            cout << "This note is not encrypted, no need for decryption.\n";
        }
    }

    // Delete a note by title
    void deleteNote() {
        string title;
        cout << "Enter note title to delete: ";
        cin.ignore();  // Clear input buffer
        getline(cin, title);

        for (int i = 0; i < notes.size(); i++) {
            if (notes[i]->getTitle() == title) {
                delete notes[i];
                notes.erase(notes.begin() + i);
                cout << "Note with title \"" << title << "\" deleted.\n";
                return;
            }
        }
        cout << "Note with title \"" << title << "\" not found.\n";
    }
};

// ===== Main Program =====
int main() {
    NoteManager manager;
    int choice;

    while (true) {
        // Display all notes before the main menu
        manager.viewNotes();

        cout << "\n===== SIMPLE NOTEPAD =====\n";
        cout << "1. Create Note\n";
        cout << "2. Create Encrypted Note\n";
        cout << "3. View Notes\n";
        cout << "4. Delete Note\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: manager.createNote(); break;
            case 2: manager.createEncryptedNote(); break;
            case 3: manager.decryptNote(); break;
            case 4: manager.deleteNote(); break;
            case 5: 
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice.\n";
        }
    }
}
