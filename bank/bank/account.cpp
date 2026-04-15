#include "account.h"
#include <cmath>
#include <iostream>
#include <utility>
using namespace std;
using namespace std::rel_ops;

double Account::total = 0;

RecordMap Account::recordMap;

Account::Account(const Date& date, const string& id): id(id), balance(0){
    date.show();
    cout << "\t#" << id << " created" << endl;
}

void Account::record(const Date& date, double amount, const string& desc) {
    amount = floor(amount * 100 + 0.5) / 100;
    balance += amount;
    total += amount;
    date.show();
    cout << "\t#" << id << "\t" << amount << "\t" << balance << "\t" << desc << endl;
}

void Account::show() const {
    cout << id << "\tBalance: " << balance;
}

void Account::query(const Date& begin, const Date& end) {
    RecordMap::iterator iter1 = recordMap.lower_bound(begin);
    RecordMap::iterator iter2 = recordMap.upper_bound(end);
    for (RecordMap::iterator iter = iter1; iter != iter2; ++iter)
        iter->second.show();
}

//AccountRecord
AccountRecord::AccountRecord(const Date& date, const Account* account, double amount, double balance, const std::string& desc)
    :date(date), account(account), amount(amount), balance(balance), desc(desc) {}

void AccountRecord::show()const {
    date.show();
    cout << "\t#" << account->getId() << "\t" << amount << "\t" << balance << "\t" << desc << endl;
}

//SavingsAccount
SavingsAccount::SavingsAccount(const Date& date, const string& id, double rate):Account(date,id),acc(date,0), rate(rate){}

void SavingsAccount::deposit(const Date& date, double amount, const string& a) {
    record(date, amount, a);
    acc.change(date, getBalance());

}

void SavingsAccount::withdraw(const Date& date, double amount, const string& a) {
    record(date, -amount, a);
    acc.change(date, getBalance());
}

void SavingsAccount::settle(const Date& date) {
    if (date.getMonth() == 1) {
        double interest = acc.getSum(date) * rate / date.distance(Date(date.getYear() - 1, 1, 1));
        if (interest != 0) {
            record(date, interest, "interest");
        }
    }
    acc.reset(date, getBalance());
}

void SavingsAccount::show() const  {
    Account::show();
}

//CreditAccount
CreditAccount::CreditAccount(const Date& date, const string& id, double credit, double rate, double fee): Account(date, id),acc(date,0), credit(credit), rate(rate), fee(fee){ }

void CreditAccount::deposit(const Date& date, double amount, const string& a) {
    record(date, amount, a);
    acc.change(date, getDebt());
}

void CreditAccount::withdraw(const Date& date, double amount, const string& a) {
    record(date, -amount, a);
    acc.change(date, getDebt());
}

void CreditAccount::settle(const Date& date) {
    if (date.getDay() != 1) return;
    double interest = acc.getSum(date) * rate;
    if (interest != 0)
        record(date, interest, "interest");
    if (date.getMonth() == 1)
        record(date, -fee, "annual fee");
    acc.reset(date, getDebt());
}

void CreditAccount::show() const {
    Account::show();
    cout << "\tAvailable credit:" << getAvailableCredit();
}
