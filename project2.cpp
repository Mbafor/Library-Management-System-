/*
 * Library Management System in C++
 *
 * This program implements a simple library management system with three main classes:
 * - Book: Represents books in the library
 * - User: Represents library patrons
 * - Librarian: Handles administrative tasks
 *
 * The system allows for:
 * - Adding/removing books
 * - Registering users
 * - Borrowing/returning books
 * - Calculating fines
 * - Displaying inventory and user information
 */

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

// Forward declarations for class dependencies
class Book;
class User;
class Librarian;

/*
 * Book Class
 * Represents a book in the library system
 */
class Book {
private:
    std::string title;
    std::string author;
    std::string ISBN;
    bool isAvailable;  // Tracks if book is available for checkout
    time_t dueDate;    // Stores due date if checked out

public:
    // Constructor initializes book details and sets default availability
    Book(std::string title, std::string author, std::string ISBN)
        : title(title), author(author), ISBN(ISBN), isAvailable(true), dueDate(0) {}

    // Getter methods
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getISBN() const { return ISBN; }
    bool getAvailability() const { return isAvailable; }
    time_t getDueDate() const { return dueDate; }

    // Setter methods
    void setAvailability(bool available) { isAvailable = available; }
    void setDueDate(time_t date) { dueDate = date; }

    // Displays book information
    void displayInfo() const {
        std::cout << "Title: " << title << "\nAuthor: " << author
                  << "\nISBN: " << ISBN << "\nStatus: "
                  << (isAvailable ? "Available" : "Checked Out") << "\n";
    }
};

/*
 * User Class
 * Represents a library patron who can borrow books
 */
class User {
private:
    std::string name;
    std::string userID;
    std::vector<Book*> borrowedBooks;  // Tracks books currently borrowed
    double fines;                      // Accumulated fines

public:
    // Constructor initializes user details with no fines
    User(std::string name, std::string userID)
        : name(name), userID(userID), fines(0.0) {}

    // Getter methods
    std::string getName() const { return name; }
    std::string getUserID() const { return userID; }
    double getFines() const { return fines; }
    const std::vector<Book*>& getBorrowedBooks() const { return borrowedBooks; }

    // Allows user to borrow a book
    void borrowBook(Book* book) {
        if (book->getAvailability()) {  // it checks if the book is available
            borrowedBooks.push_back(book);
            book->setAvailability(false);

            // Set due date to 5 seconds from now
            time_t now = time(0);
            book->setDueDate(now + 5); // 5  seconds
            std::cout << "Book borrowed successfully.\n";
        } else {
            std::cout << "Book is not available.\n";
        }
    }

    // Allows user to return a book
    void returnBook(Book* book) {
        auto it = std::find(borrowedBooks.begin(), borrowedBooks.end(), book);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            book->setAvailability(true);

            // Calculate fine if overdue
            time_t now = time(0);
            if (now > book->getDueDate()) {
                double daysOverdue = difftime(now, book->getDueDate()) / (1);
                fines += daysOverdue * 2; // $2 dollars per second
                std::cout << "Book returned late. Fine added: $" << daysOverdue * 2 << "\n";
            }

            book->setDueDate(0);
            std::cout << "Book returned successfully.\n";
        } else {
            std::cout << "You didn't borrow this book.\n";
        }
    }

    // Allows user to pay fines
    void payFines(double amount) {
        if (amount <= fines) {
            fines -= amount;
            std::cout << "Paid $" << amount << " towards fines. Remaining: $" << fines << "\n";
        } else {
            std::cout << "Payment exceeds owed fines.\n";
        }
    }

    // Displays user information
    void displayInfo() const {
        std::cout << "User: " << name << "\nID: " << userID
                  << "\nFines: $" << fines << "\nBorrowed books: "
                  << borrowedBooks.size() << "\n";
    }
};

/*
 * Librarian Class
 * Handles administrative tasks for the library
 */
class Librarian {
private:
    std::string name;
    std::string employeeID;

public:
    // Constructor
    Librarian(std::string name, std::string employeeID)
        : name(name), employeeID(employeeID) {}

    // Adds a new book to the inventory
    void addBook(std::vector<Book>& inventory, const std::string& title,
                 const std::string& author, const std::string& ISBN) {
        inventory.emplace_back(title, author, ISBN);
        std::cout << "Book added to inventory.\n";
    }

    // Removes a book from inventory by ISBN
    void removeBook(std::vector<Book>& inventory, const std::string& ISBN) {
        auto it = std::find_if(inventory.begin(), inventory.end(),
            [&ISBN](const Book& b) { return b.getISBN() == ISBN; });

        if (it != inventory.end()) {
            inventory.erase(it);
            std::cout << "Book removed from inventory.\n";
        } else {
            std::cout << "Book not found in inventory.\n";
        }
    }

    // Displays all books in inventory
    void displayInventory(const std::vector<Book>& inventory) const {
        std::cout << "\nLibrary Inventory:\n";
        for (const auto& book : inventory) {
            book.displayInfo();
            std::cout << "-----------------\n";
        }
    }

    // Displays user information including borrowed books
    void displayUserInfo(const User& user) const {
        user.displayInfo();
        std::cout << "Borrowed Books:\n";
        for (const auto book : user.getBorrowedBooks()) {
            book->displayInfo();
            std::cout << "-----------------\n";
        }
    }
};

/*
 * Main Function
 * Provides a menu-driven interface for the library system
 */
int main() {
    std::vector<Book> inventory;  // Stores all books
    std::vector<User> users;      // Stores all registered users
    Librarian librarian("Admin", "L001");  // Default librarian

    int choice;
    do {
        // Display menu options
        std::cout << "\nLibrary Management System\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. Remove Book\n";
        std::cout << "3. Display Inventory\n";
        std::cout << "4. Register User\n";
        std::cout << "5. Borrow Book\n";
        std::cout << "6. Return Book\n";
        std::cout << "7. Pay Fines\n";
        std::cout << "8. Display User Info\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;

        switch(choice) {
            case 1: {  // Add Book
                std::string title, author, ISBN;
                std::cout << "Enter title: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                std::cout << "Enter author: ";
                std::getline(std::cin, author);
                std::cout << "Enter ISBN: ";
                std::getline(std::cin, ISBN);
                librarian.addBook(inventory, title, author, ISBN);
                break;
            }
            case 2: {  // Remove Book
                std::string ISBN;
                std::cout << "Enter ISBN of book to remove: ";
                std::cin.ignore();
                std::getline(std::cin, ISBN);
                librarian.removeBook(inventory, ISBN);
                break;
            }
            case 3:  // Display Inventory
                librarian.displayInventory(inventory);
                break;
            case 4: {  // Register User
                std::string name, userID;
                std::cout << "Enter user name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userID);
                users.emplace_back(name, userID);
                std::cout << "User registered successfully.\n";
                break;
            }
            case 5: {  // Borrow Book
                if (users.empty() || inventory.empty()) {
                    std::cout << "No users or books available.\n";
                    break;
                }

                // Select user
                std::cout << "Select user (0-" << users.size()-1 << "):\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    std::cout << i << ". " << users[i].getName() << "\n";
                }
                size_t userIndex;
                std::cin >> userIndex;

                // Select book
                std::cout << "Select book (0-" << inventory.size()-1 << "):\n";
                for (size_t i = 0; i < inventory.size(); ++i) {
                    std::cout << i << ". " << inventory[i].getTitle() << "\n";
                }
                size_t bookIndex;
                std::cin >> bookIndex;

                // Process borrowing
                if (userIndex < users.size() && bookIndex < inventory.size()) {
                    users[userIndex].borrowBook(&inventory[bookIndex]);
                } else {
                    std::cout << "Invalid selection.\n";
                }
                break;
            }
            case 6: {  // Return Book
                if (users.empty()) {
                    std::cout << "No users available.\n";
                    break;
                }

                // Select user
                std::cout << "Select user (0-" << users.size()-1 << "):\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    std::cout << i << ". " << users[i].getName() << "\n";
                }
                size_t userIndex;
                std::cin >> userIndex;

                if (userIndex >= users.size()) {
                    std::cout << "Invalid user selection.\n";
                    break;
                }

                // Select book to return
                auto& borrowed = users[userIndex].getBorrowedBooks();
                if (borrowed.empty()) {
                    std::cout << "No books borrowed.\n";
                    break;
                }

                std::cout << "Select book to return (0-" << borrowed.size()-1 << "):\n";
                for (size_t i = 0; i < borrowed.size(); ++i) {
                    std::cout << i << ". " << borrowed[i]->getTitle() << "\n";
                }
                size_t bookIndex;
                std::cin >> bookIndex;

                // Process return
                if (bookIndex < borrowed.size()) {
                    users[userIndex].returnBook(borrowed[bookIndex]);
                } else {
                    std::cout << "Invalid book selection.\n";
                }
                break;
            }
            case 7: {  // Pay Fines
                if (users.empty()) {
                    std::cout << "No users available.\n";
                    break;
                }

                // Select user
                std::cout << "Select user (0-" << users.size()-1 << "):\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    std::cout << i << ". " << users[i].getName() << " (Fines: $"
                              << users[i].getFines() << ")\n";
                }
                size_t userIndex;
                std::cin >> userIndex;

                // Process payment
                if (userIndex < users.size()) {
                    double amount;
                    std::cout << "Enter amount to pay: $";
                    std::cin >> amount;
                    users[userIndex].payFines(amount);
                } else {
                    std::cout << "Invalid selection.\n";
                }
                break;
            }
            case 8: {  // Display User Info
                if (users.empty()) {
                    std::cout << "No users available.\n";
                    break;
                }

                // Select user
                std::cout << "Select user (0-" << users.size()-1 << "):\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    std::cout << i << ". " << users[i].getName() << "\n";
                }
                size_t userIndex;
                std::cin >> userIndex;

                // Display info
                if (userIndex < users.size()) {
                    librarian.displayUserInfo(users[userIndex]);
                } else {
                    std::cout << "Invalid selection.\n";
                }
                break;
            }
            case 0:  // Exit
                std::cout << "Exiting...\n";
                break;
            default:  // Invalid choice
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}