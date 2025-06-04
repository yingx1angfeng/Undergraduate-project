#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__
#include "date.h"
#include "acculumator.h"
#include <string>
#include <map>
using namespace std;

class Account;

class AccountRecord {
private:
    Date date;
    const Account* account;
    double amount;
    double balance;
    string desc;

public:
    AccountRecord(const Date& date, const Account* account, double amount, double balance, const string& desc);
    void show() const;
};

typedef multimap<Date, AccountRecord> RecordMap;

class Account {

private:
    string id;
    double balance;	//余额
    static double total;
    static RecordMap recordMap;

protected:
    Account(const Date& date, const string& id);

    void record(const Date& date, double amount, const string& a);
public:
 
    const string& getId() const { return id; }
    double getBalance() const { return balance; }
    static double getTotal() { return total; }

    virtual void show() const = 0;

    virtual void deposit(const Date& date, double amount, const string& id) = 0;
    virtual void withdraw(const Date& date, double amount, const string& id) = 0;
    virtual void settle(const Date& date) = 0;

    static void query(const Date& begin, const Date& end);
    virtual ~Account() = default;
};

class SavingsAccount : public Account {
private :
    Accumulator acc;
    double rate;
public:
    SavingsAccount(const Date& date, const string& id, double rating);
    double getRate() const { return rate; }

    void deposit(const Date& date, double amount, const std::string& desc) override;
    void withdraw(const Date& date, double amount, const std::string& desc) override;
    void settle(const Date& date) override;

    virtual void show() const;

};

class CreditAccount:public Account {
private:
    Accumulator acc;
    double credit;
    double rate;//欠款利率
    double fee;//信用卡年费

    double getDebt() const {
        double balance = getBalance();
        return (balance < 0 ? balance : 0);
    }

public:
    CreditAccount(const Date& date, const std::string& id, double credit, double rate, double fee);
    double getCredit() const { return credit; }
    double getRate() const { return rate; }
    double getFee() const { return fee; }
    double getAvailableCredit() const {
        if (getBalance() < 0)
            return credit + getBalance();
        else
            return credit;
    }

    void deposit(const Date& date, double amount, const std::string& desc) override;
    void withdraw(const Date& date, double amount, const std::string& desc) override;
    void settle(const Date& date) override;
    void show() const override;
};

#endif