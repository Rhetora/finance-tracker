#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
// #include <ctime>

// Class representing a financial account
class Account
{
private:
    double balance;
    double interest;

public:
    std::string name;
    std::string bank;
    std::string type;

    // Constructor to initialize an account
    Account(const std::string &n, const std::string &b, double bal, double i, const std::string &t)
        : name(n), bank(b), balance(bal), interest(i), type(t)
    {
    }

    // Getters and setters for balance and interest
    double getBalance() const { return balance; }
    void setBalance(double bal) { balance = bal; }

    double getInterest() const { return interest; }
    void setInterest(double i) { interest = i; }

    void AddAccountToCSV()
    {
        std::ofstream file("accounts.csv", std::ios::app);
        if (!file.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        file << name << "," << bank << "," << getBalance() << "," << getInterest() << "," << type << std::endl;
        file.close();
    }
};

class FinanceSummary
{
public:
    double totalBalance;
    double currentBalance;
    double savingsBalance;
    double creditBalance;
    double ISABalance;
    double GIABalance;
    double CryptoBalance;
    double totalInterest;

    FinanceSummary(std::vector<Account> accountList)
    {
        totalBalance = 0;
        currentBalance = 0;
        savingsBalance = 0;
        creditBalance = 0;
        ISABalance = 0;
        GIABalance = 0;
        CryptoBalance = 0;
        totalInterest = 0;

        for (const Account &account : accountList)
        {
            totalBalance += account.getBalance();
            totalInterest += account.getInterest() * account.getBalance() * 0.01;

            if (account.type == "Current")
            {
                currentBalance += account.getBalance();
            }
            else if (account.type == "Savings")
            {
                savingsBalance += account.getBalance();
            }
            else if (account.type == "Credit")
            {
                creditBalance += account.getBalance();
            }
            else if (account.type == "ISA")
            {
                ISABalance += account.getBalance();
            }
            else if (account.type == "GIA")
            {
                GIABalance += account.getBalance();
            }
            else if (account.type == "Crypto")
            {
                CryptoBalance += account.getBalance();
            }
        }
    }

    void SaveFinanceSummary()
    {
        std::ofstream file("history.csv", std::ios::app);
        if (!file.good())
        {
            std::ofstream file("history.csv");
            file.close();
            return;
        }

        if (!file.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        time_t now = time(0);
        char *dt = ctime(&now);
        tm *gmtm = gmtime(&now);
        dt = asctime(gmtm);

        file << dt << "," << totalBalance << "," << currentBalance << "," << savingsBalance << "," << creditBalance << "," << ISABalance << "," << GIABalance << "," << CryptoBalance << "," << totalInterest << "," << std::endl;
        file.close();
    }
};

std::vector<Account> LoadAccountsFromCSV()
{
    std::vector<Account> accountList;
    std::ifstream file("accounts.csv");
    if (!file.good())
    {
        std::ofstream file("accounts.csv");
        file.close();
        return accountList;
    }

    if (!file.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return accountList;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string name, bank, type;
        double balance, interest;
        if (std::getline(ss, name, ',') &&
            std::getline(ss, bank, ',') &&
            ss >> balance &&
            ss.ignore() &&
            ss >> interest &&
            ss.ignore() &&
            std::getline(ss, type))
        {
            accountList.push_back(Account(name, bank, balance, interest, type));
        }
    }

    return accountList;
}
