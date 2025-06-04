#ifndef __DATE_H__
#define __DATE_H__
#include <iostream>
using namespace std;

class Date {
private:
    int year;
    int month;
    int day;
    int totalDays;

public:
    Date(int year, int month, int day);
    Date();

    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getMaxDay() const;

    bool isLeapYear() const;

    void show() const;

    int distance(const Date& date) const {
        return totalDays - date.totalDays;
    }

    static Date read(){
        int year, month, day;
        char c1, c2;
        cin >> year >> c1 >> month >> c2 >> day;
        return Date(year, month, day);
    }

    bool operator < (const Date& date) const {
        return totalDays < date.totalDays;
    }
};

#endif