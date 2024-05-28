#include <string>

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
        : name(n), bank(b), balance(bal), interest(i), type(t) {
        }

    // Getters and setters for balance and interest
    double getBalance() const { return balance; }
    void setBalance(double bal) { balance = bal; }

    double getInterest() const { return interest; }
    void setInterest(double i) { interest = i; }
};
