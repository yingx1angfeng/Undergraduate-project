#pragma once
#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__
#include "date.h"
 

class Accumulator{
private:
	Date lastDate;
	double value;
	double sum;

public:
	Accumulator(const Date& date, double value):lastDate(date),value(value),sum(0){}
	double getSum(const Date& date)const {
		return sum + value * date.distance(lastDate);
	}
	//指定日期变更value
	void change(const Date& date, double value) {
		sum = getSum(date);
		lastDate = date;
		this->value = value;
	}
	//累加器的初始化
	void reset(const Date& date, double value) {
		lastDate = date;
		this->value = value;
		sum = 0;
	}
};
#endif