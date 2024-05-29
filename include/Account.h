#pragma once

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

// Class representing financial account
class Account
{
private:
    double balance_;
    double interest_;

    static const std::set<std::string> validTypes_;

public:
    std::string name_;
    std::string bank_;
    std::string type_;

    // Constructor to initialize an account
    Account(const std::string &n, const std::string &b, double bal, double i, const std::string &t);

    // Getters and setters for balance and interest
    double balance() const;
    void setBalance(double bal);

    double interest() const;
    void setInterest(double i);

    // Append single account to accounts CSV file
    void AddAccountToCSV();
};

class FinanceSummary
{
public:
    double totalBalance_;
    double currentBalance_;
    double savingsBalance_;
    double creditBalance_;
    double isaBalance_;
    double giaBalance_;
    double cryptoBalance_;
    double totalInterest_;


    // Constructor to calculate financial summary
    FinanceSummary(std::vector<Account> accountList);

    // Append a snapshot of financial summary to history CSV file
    void SaveFinanceSummary();
};


// Load list of accounts from accounts CSV file
std::vector<Account> LoadAccountsFromCSV();

// Implementation

const std::set<std::string> Account::validTypes_ = {"Savings", "Current", "Credit", "ISA", "GIA", "Crypto"};

Account::Account(const std::string &n, const std::string &b, double bal, double i, const std::string &t)
    : name_(n), bank_(b), balance_(bal), interest_(i), type_(t)
{
    if (validTypes_.find(t) == validTypes_.end())
    {
        throw std::invalid_argument("Invalid account type");
    }
}

double Account::balance() const { return balance_; }

void Account::setBalance(double bal) { balance_ = bal; }

double Account::interest() const { return interest_; }

void Account::setInterest(double i) { interest_ = i; }

void Account::AddAccountToCSV()
{
    std::ofstream file("accounts.csv", std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("File is not open");
    }

    file << name_ << "," << bank_ << "," << balance() << "," << interest() << "," << type_ << std::endl;
    file.close();
}

FinanceSummary::FinanceSummary(std::vector<Account> accountList)
{
    totalBalance_ = 0;
    currentBalance_ = 0;
    savingsBalance_ = 0;
    creditBalance_ = 0;
    isaBalance_ = 0;
    giaBalance_ = 0;
    cryptoBalance_ = 0;
    totalInterest_ = 0;

    for (const Account &account : accountList)
    {
        totalBalance_ += account.balance();
        totalInterest_ += account.interest() * account.balance() * 0.01;

        if (account.type_ == "Current")
        {
            currentBalance_ += account.balance();
        }
        else if (account.type_ == "Savings")
        {
            savingsBalance_ += account.balance();
        }
        else if (account.type_ == "Credit")
        {
            creditBalance_ += account.balance();
        }
        else if (account.type_ == "ISA")
        {
            isaBalance_ += account.balance();
        }
        else if (account.type_ == "GIA")
        {
            giaBalance_ += account.balance();
        }
        else if (account.type_ == "Crypto")
        {
            cryptoBalance_ += account.balance();
        }
    }
}

void FinanceSummary::SaveFinanceSummary()
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

    file << dt << "," << totalBalance_ << "," << currentBalance_ << "," << savingsBalance_ << "," << creditBalance_ << "," << isaBalance_ << "," << giaBalance_ << "," << cryptoBalance_ << "," << totalInterest_ << "," << std::endl;
    file.close();
}

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
