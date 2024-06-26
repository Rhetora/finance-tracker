#pragma once

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <memory>
#include <cstring>
#include <tuple>

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

// Class representing financial summary
class FinanceSummary
{
public:
    std::string date_;
    double totalBalance_;
    double currentBalance_;
    double savingsBalance_;
    double creditBalance_;
    double isaBalance_;
    double giaBalance_;
    double cryptoBalance_;
    double totalInterest_;

    /**
     * Constructor to calculate financial summary.
     *
     * @param accountList A vector of Account objects to calculate the summary from.
     */
    FinanceSummary(const std::vector<Account> &accountList);

    FinanceSummary(std::string date, double totalBalance, double currentBalance, double savingsBalance, double creditBalance, double isaBalance, double giaBalance, double cryptoBalance, double totalInterest);
    /**
     * Append a snapshot of financial summary to history CSV file.
     */
    void SaveFinanceSummary() const;
};

// Class holding application saved data
class SavedData
{
public:
    std::vector<Account> accountList_;
    std::vector<FinanceSummary> savedSummaryList_;
    FinanceSummary currentSummary_;

    SavedData();

    // Load list of accounts from accounts CSV file
    std::vector<Account> LoadAccountsFromCSV();
    std::vector<FinanceSummary> loadFinanceSummaryFromCSV();
    std::vector<std::tuple<std::vector<double>, std::vector<double>>> getFinanceSummaryPoints();
    void UpdateAccountsInCSV(std::vector<Account> accountList);
};

// Implementation

// Account
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

// Finance Summary
FinanceSummary::FinanceSummary(const std::vector<Account> &accountList)
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
    time_t now = time(0);
    char *dt = ctime(&now);
    tm *gmtm = gmtime(&now);
    date_ = asctime(gmtm);
}

FinanceSummary::FinanceSummary(std::string date, double totalBalance, double currentBalance, double savingsBalance, double creditBalance, double isaBalance, double giaBalance, double cryptoBalance, double totalInterest)
    : date_(date), totalBalance_(totalBalance), currentBalance_(currentBalance), savingsBalance_(savingsBalance),
      creditBalance_(creditBalance), isaBalance_(isaBalance), giaBalance_(giaBalance),
      cryptoBalance_(cryptoBalance), totalInterest_(totalInterest) {}

void FinanceSummary::SaveFinanceSummary() const
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

// Saved Data
SavedData::SavedData()
    : accountList_(LoadAccountsFromCSV()),
      savedSummaryList_(loadFinanceSummaryFromCSV()),
      currentSummary_(accountList_)
{
}

std::vector<Account> SavedData::LoadAccountsFromCSV()
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

std::vector<FinanceSummary> SavedData::loadFinanceSummaryFromCSV()
{
    std::vector<FinanceSummary> financeSummaryList;
    std::ifstream file("history.csv");
    if (!file.good())
    {
        std::ofstream file("history.csv");
        file.close();
        return financeSummaryList;
    }
    if (!file.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return financeSummaryList;
    }
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string date;
        double totalBalance, currentBalance, savingsBalance, creditBalance, isaBalance, giaBalance, cryptoBalance, totalInterest;
        if (std::getline(ss, date, ',') &&
            ss >> totalBalance &&
            ss.ignore() &&
            ss >> currentBalance &&
            ss.ignore() &&
            ss >> savingsBalance &&
            ss.ignore() &&
            ss >> creditBalance &&
            ss.ignore() &&
            ss >> isaBalance &&
            ss.ignore() &&
            ss >> giaBalance &&
            ss.ignore() &&
            ss >> cryptoBalance &&
            ss.ignore() &&
            ss >> totalInterest)
        {
            financeSummaryList.push_back(FinanceSummary(date, totalBalance, currentBalance, savingsBalance, creditBalance, isaBalance, giaBalance, cryptoBalance, totalInterest));
        }
    }
    return financeSummaryList;
}


std::vector<std::tuple<std::vector<double>, std::vector<double>>> SavedData::getFinanceSummaryPoints()
{
    std::vector<std::tuple<std::vector<double>, std::vector<double>>> summaryPoints;
    std::vector<double> dates;
    std::vector<double> totalBalances;
    std::vector<double> currentBalances;
    std::vector<double> savingsBalances;
    std::vector<double> creditBalances;
    std::vector<double> isaBalances;
    std::vector<double> giaBalances;
    std::vector<double> cryptoBalances;
    std::vector<double> totalInterests;
    double i = 0;
    for (const auto &summary : savedSummaryList_)
    {
        // Convert the date string to char* and store it in the dates vector
        std::string dateStr = summary.date_;
        char *date = new char[dateStr.length() + 1];
        strcpy(date, dateStr.c_str());
        dates.push_back(i);
        i++;

        totalBalances.push_back(summary.totalBalance_);
        currentBalances.push_back(summary.currentBalance_);
        savingsBalances.push_back(summary.savingsBalance_);
        creditBalances.push_back(summary.creditBalance_);
        isaBalances.push_back(summary.isaBalance_);
        giaBalances.push_back(summary.giaBalance_);
        cryptoBalances.push_back(summary.cryptoBalance_);
        totalInterests.push_back(summary.totalInterest_);
    }

    summaryPoints.emplace_back(dates, totalBalances);
    summaryPoints.emplace_back(dates, currentBalances);
    summaryPoints.emplace_back(dates, savingsBalances);
    summaryPoints.emplace_back(dates, creditBalances);
    summaryPoints.emplace_back(dates, isaBalances);
    summaryPoints.emplace_back(dates, giaBalances);
    summaryPoints.emplace_back(dates, cryptoBalances);
    summaryPoints.emplace_back(dates, totalInterests);

    return summaryPoints;
}

void SavedData::UpdateAccountsInCSV(std::vector<Account> accountList)
{
    std::ofstream file("accounts.csv");
    for (const Account &account : accountList)
    {
        file << account.name_ << "," << account.bank_ << "," << account.balance() << "," << account.interest() << "," << account.type_ << std::endl;
    }
    file.close();
}
