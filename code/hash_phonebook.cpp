#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int STARTING_TABLE_SIZE = 10;
const double MAX_LOAD_FACTOR = 0.75;

struct Contact {
    string name;
    string phoneNumber;
    string ipAddress;
    Contact* next;
};

class HashPhoneBook {
private:
    Contact** table;
    int tableSize;
    int contactCount;

    //this hash function converts a name into an array index it adds the character values in the name, then uses modulo so the index always stays inside current table size

    int hashFunction(const string& name) const {
        int total = 0;

        for (char ch : name) {
            total += ch;
        }

        return total % tableSize;
    }

    Contact** createEmptyTable(int size) const{
      Contact** newTable = new Contact*[size];
      for (int i = 0; i < size; i++){
        newTable[i] = nullptr;
      }

      return newTable;
    }


    void rehash(){
      int oldTableSize = tableSize;
      Contact** oldTable = table;

      tableSize = tableSize * 2;
      table = createEmptyTable(tableSize);

      for(int i = 0; i < oldTableSize; i++){
        Contact* current = oldTable[i];

        while (current != nullptr){
          Contact* nextContact = current-> next;
          int newIndex = hashFunction(current->name);

          current->next = table[newIndex];
          table[newIndex] = current;

          current = nextContact;
        }
      }

      delete[] oldTable;
    }
public:
    //the constructor starts every bucket as empty by setting each array position to nullptr the contact count begins at 0 because the phonebook has no stored contacts yet

    HashPhoneBook() {
        tableSize = STARTING_TABLE_SIZE;
        table = createEmptyTable(tableSize);

        contactCount = 0;
    }

    //the destructor deletes every linked list node that was created with new this prevents memory leaks when the program ends

    ~HashPhoneBook() {
        for (int i = 0; i < tableSize; i++) {
            Contact* current = table[i];

            while (current != nullptr) {
                Contact* toDelete = current;
                current = current->next;
                delete toDelete;
            }
        }

        delete[] table;
    }

    //insert adds a new contact to the phonebook, or updates the contact if the name already exists if multiple names hash to the same index, they are stored in a linked list at that bucket

    void insertContact(const string& name, const string& phoneNumber, const string& ipAddress, bool showMessage = true) {
        int index = hashFunction(name);
        Contact* current = table[index];

        while (current != nullptr) {
            if (current->name == name) {
                current->phoneNumber = phoneNumber;
                current->ipAddress = ipAddress;

                if (showMessage) {
                    cout << "Updated existing contact.\n";
                }
                return;
            }

            current = current->next;
        }

        Contact* newContact = new Contact;
        newContact->name = name;
        newContact->phoneNumber = phoneNumber;
        newContact->ipAddress = ipAddress;
        newContact->next = table[index];

        table[index] = newContact;
        contactCount++;

        if (getLoadFactor() > MAX_LOAD_FACTOR){
          rehash();

          if (showMessage){
            cout << "Table resized and rehashed.\n";
          }
        }

        if (showMessage) {
            cout << "Contact saved.\n";
        }
    }

    //search uses the hash function to jump directly to the most likely bucket then it walks through that bucket's linked list until it finds the name or reaches the end

    Contact* searchContact(const string& name) const {
        int index = hashFunction(name);
        Contact* current = table[index];

        while (current != nullptr) {
            if (current->name == name) {
                return current;
            }

            current = current->next;
        }

        return nullptr;
    }

    //delete finds the correct bucket, then removes the matching linked-list node if it exists the previous pointer is needed so the list can be reconnected after the contact is removed
     
    bool deleteContact(const string& name) {
        int index = hashFunction(name);
        Contact* current = table[index];
        Contact* previous = nullptr;

        while (current != nullptr) {
            if (current->name == name) {
                if (previous == nullptr) {
                    table[index] = current->next;
                } else {
                    previous->next = current->next;
                }

                delete current;
                contactCount--;
                return true;
            }

            previous = current;
            current = current->next;
        }

        return false;
    }

    //yhr load factor shows how full the hash table is when this num goes above max laod factor the table resizes and rehashes the contacts

    double getLoadFactor() const {
        return static_cast<double>(contactCount) / tableSize;
    }

    //This function saves every contact in the hash table to a text file. The file becomes the permanent phonebook, so changes made in the program are still there the next time the program runs

    bool saveToFile(const string& filename) const {
        ofstream fout(filename);

        if (!fout.is_open()) {
            return false;
        }

        for (int i = 0; i < tableSize; i++) {
            Contact* current = table[i];

            while (current != nullptr) {
                fout << current->name << '|'
                     << current->phoneNumber << '|'
                     << current->ipAddress << endl;

                current = current->next;
            }
        }

        fout.close();
        return true;
    }

    //this function reads contacts from a text file when the program starts rach line must be written as name|phone number|IP address so the program can split the line into the correct contact fields

    bool loadFromFile(const string& filename) {
        ifstream fin(filename);

        if (!fin.is_open()) {
            return false;
        }

        string line;

        while (getline(fin, line)) {
            size_t firstDivider = line.find('|');

            if (firstDivider != string::npos) {
                size_t secondDivider = line.find('|', firstDivider + 1);

                if (secondDivider == string::npos) {
                    continue;
                }

                string name = line.substr(0, firstDivider);
                string phoneNumber = line.substr(firstDivider + 1, secondDivider - firstDivider - 1);
                string ipAddress = line.substr(secondDivider + 1);

                insertContact(name, phoneNumber, ipAddress, false);
            }
        }

        fin.close();
        return true;
    }

    //display prints each bucket in the table if several contacts appear in one bucket, that shows a collision handled by separate chaining

    void displayTable() const {
        cout << "\nHash Table:\n";

        for (int i = 0; i < tableSize; i++) {
            cout << "Bucket " << i << ": ";
            Contact* current = table[i];

            if (current == nullptr) {
                cout << "empty";
            }

            while (current != nullptr) {
                cout << current->name;

                if (current->next != nullptr) {
                    cout << " -> ";
                }

                current = current->next;
            }

            cout << endl;
        }
    }

    void displayStats() const {
        cout << "\nTotal contacts: " << contactCount << endl;
        cout << "Table size: " << tableSize << endl;
        cout << "Load factor: " << getLoadFactor() << endl;
        cout << "Max Load factor before rehashing: " << MAX_LOAD_FACTOR << endl;
    }
};

void printMenu() {
    cout << "\nHash Table Phonebook\n";
    cout << "1. Add or update contact\n";
    cout << "2. Search for contact\n";
    cout << "3. Delete contact\n";
    cout << "4. Display hash table\n";
    cout << "5. Show load factor\n";
    cout << "6. Exit\n";
    cout << "Choose an option: ";
}

string readLine(const string& prompt) {
    string text;
    cout << prompt;
    getline(cin, text);
    return text;
}

int main() {
    const string filename = "phonebook.txt";
    HashPhoneBook phoneBook;
    int choice = 0;

    cout << "This program uses a hash table to store phonebook contacts.\n";

    if (phoneBook.loadFromFile(filename)) {
        cout << "Loaded contacts from " << filename << ".\n";
        phoneBook.displayStats();
    } else {
        cout << "No phonebook file found yet. A new one will be created when you save contacts.\n";
    }

    while (choice != 6) {
        printMenu();
        cin >> choice;
        cin.ignore(1000, '\n');

        if (choice == 1) {
            string name = readLine("Name: ");
            string phoneNumber = readLine("Phone number: ");
            string ipAddress = readLine("IP address: ");

            phoneBook.insertContact(name, phoneNumber, ipAddress);

            if (!phoneBook.saveToFile(filename)) {
                cout << "Warning: could not save to " << filename << ".\n";
            }
        } else if (choice == 2) {
            string name = readLine("Name to search for: ");
            Contact* contact = phoneBook.searchContact(name);

            if (contact != nullptr) {
                cout << "\nContact found:\n";
                cout << "Name: " << contact->name << endl;
                cout << "Phone number: " << contact->phoneNumber << endl;
                cout << "IP address: " << contact->ipAddress << endl;
            } else {
                cout << "Contact not found.\n";
            }
        } else if (choice == 3) {
            string name = readLine("Name to delete: ");

            if (phoneBook.deleteContact(name)) {
                cout << "Contact deleted.\n";

                if (!phoneBook.saveToFile(filename)) {
                    cout << "Warning: could not save to " << filename << ".\n";
                }
            } else {
                cout << "Contact not found.\n";
            }
        } else if (choice == 4) {
            phoneBook.displayTable();
        } else if (choice == 5) {
            phoneBook.displayStats();
        } else if (choice == 6) {
            cout << "Goodbye.\n";
        } else {
            cout << "Invalid option. Please choose 1-6.\n";
        }
    }

    return 0;
}

