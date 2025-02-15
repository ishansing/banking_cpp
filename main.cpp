#include <fstream>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <vector>

struct Account {
  int accNo;
  std::string name;
  std::string password;
  int balance;
};

Account getAccount(int accNo, const std::string &password = "") {
  std::ifstream db("database.csv");
  Account account = {-1, "", "", -1};

  if (!db) {
    std::cerr << "Error opening file!" << std::endl;
    return account;
  }

  std::string line;
  while (std::getline(db, line)) {
    std::stringstream ss(line);
    std::string accNoStr, name, storedPass, balanceStr;

    std::getline(ss, accNoStr, ',');
    std::getline(ss, name, ',');
    std::getline(ss, storedPass, ',');
    std::getline(ss, balanceStr, ',');

    int storedAccNo = std::stoi(accNoStr);

    if (storedAccNo == accNo) {

      if (password.empty() || storedPass == password)
        account = {storedAccNo, name, storedPass, std::stoi(balanceStr)};
      else
        std::cerr << "Incorrect password!\n";
      break;
    }
  }
  db.close();
  return account;
}

bool updateBalance(int accNo, int newBalance) {
  std::ifstream db("database.csv");
  if (!db) {
    std::cerr << "Error opening file!" << std::endl;
    return false;
  }

  std::vector<std::string> lines;
  std::string line;
  bool updated = false;

  while (std::getline(db, line)) {
    std::stringstream ss(line);
    std::string accNoStr, name, password, balanceStr;
    std::getline(ss, accNoStr, ',');
    std::getline(ss, name, ',');
    std::getline(ss, password, ',');
    std::getline(ss, balanceStr, ',');

    if (std::stoi(accNoStr) == accNo) {
      line = accNoStr + "," + name + "," + password + "," +
             std::to_string(newBalance);
      updated = true;
    }
    lines.push_back(line);
  }
  db.close();

  if (!updated) {
    std::cerr << "Account not found!\n";
    return false;
  }

  std::ofstream outFile("database.csv");
  if (!outFile) {
    std::cerr << "Error writing file!" << std::endl;
    return false;
  }

  for (const auto &l : lines)
    outFile << l << "\n";
  outFile.close();

  return true;
}

void signUp() {
  std::fstream db("database.csv", std::ios::in); // Open the file in read mode
  std::string line, lastLine;
  int accNo = 100'025'000; // Default starting account number

  // Find the last line in the file to get the last account number
  while (std::getline(db, line))
    if (!line.empty())
      lastLine = line;
  db.close();

  // Extract the last account number from the last line
  if (!lastLine.empty()) {
    std::stringstream ss(lastLine);
    std::string accNoStr;
    std::getline(ss, accNoStr, ','); // Get first column (account number)
    accNo = std::stoi(accNoStr) + 1; // Increment account number
  }

  // Take user input for name and password
  std::string name, passwd;
  std::cout << "Enter username: ";
  std::getline(std::cin >> std::ws, name);
  while (true) {
    std::cout << "Enter a secure password : ";
    std::cin >> passwd;
    std::string conpass = "";
    std::cout << "Confirm password : ";
    std::cin >> conpass;
    if (conpass == passwd) {
      break;
    } else {
      std::cout << "Password doesn't match! Try Again!!\n";
      continue;
    }
  }

  int balance = 0; // Default balance

  // Open file in append mode and write new user data
  std::ofstream outFile("database.csv", std::ios::app);
  if (!outFile) {
    std::cerr << "Error opening file!" << std::endl;
    return;
  }

  outFile << accNo << "," << name << "," << passwd << "," << balance << "\n";
  outFile.close();

  std::cout << "Account created successfully! Your account number is " << accNo
            << std::endl;
}

void deposit(int accNo, int amount) {
  Account account = getAccount(accNo);
  if (account.accNo == -1)
    return;

  account.balance += amount;
  if (updateBalance(accNo, account.balance)) {
    std::cout << "Deposit successful! New balance: $" << account.balance
              << std::endl;
  }
}

void withdraw(int accNo, int amount) {
  Account account = getAccount(accNo);
  if (account.accNo == -1)
    return;

  if (amount > account.balance) {
    std::cerr << "Insufficient balance!\n";
    return;
  }

  account.balance -= amount;
  if (updateBalance(accNo, account.balance)) {
    std::cout << "Withdrawal successful! New balance: $" << account.balance
              << std::endl;
  }
}

void printBalance(int accNo) {
  Account account = getAccount(accNo, "");
  if (account.accNo != -1)
    std::cout << "Current Balance: $" << account.balance << std::endl;
}

int main() {
  int accNo;
  std::string password;

  int choice = 0;
  std::cout << "***********************************************\n";
  std::cout << "           WELCOME TO VITB BANK\n";
  std::cout << "***********************************************\n";
  std::cout << "1. Sign Up\n";
  std::cout << "2. Login\n";
  std::cout << "3. Exit\n";
  std::cin >> choice;

  if (choice == 1) {
    signUp();

  } else if (choice == 2) {

    std::cout << "Enter Account Number: ";
    std::cin >> accNo;
    std::cin.ignore();
    std::cout << "Enter Password: ";
    std::getline(std::cin, password);

    Account user = getAccount(accNo, password);
    if (user.accNo == -1) {
      std::cerr << "Login failed!\n";
      return 1;
    }
    std::cout << "***********************************************\n";
    std::cout << "Login successful! Welcome, " << user.name << "!\n";
    std::cout << "***********************************************\n";
    int n, amount;
    while (true) {
      std::cout << "*********************MENU*********************";
      std::cout
          << "\n1. Deposit\n2. Withdraw\n3. Show Balance\n4. Exit\nChoose "
             "an option: ";
      std::cin >> n;

      switch (n) {
      case 1:
        std::cout << "Enter deposit amount: ";
        std::cin >> amount;
        deposit(user.accNo, amount);
        break;
      case 2:
        std::cout << "Enter withdrawal amount: ";
        std::cin >> amount;
        withdraw(user.accNo, amount);
        break;
      case 3:
        printBalance(user.accNo);
        break;
      case 4:
        std::cout << "Exiting...\n";
        return 0;
      default:
        std::cerr << "Invalid option!\n";
      }
    }
  } else if (choice == 3) {
    std::cout << "Exiting...\n";
  }
  return 0;
}
