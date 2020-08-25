#include "../include/utils.hpp"
int day_in_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
bool isLeapYear(int year)
{
    if (year % 100 == 0)
    {
        if (year % 400 == 0)
            return true;
        else
            return false;
    }
    else if (year % 4 == 0)
        return true;
    else
        return false;
}

bool isValidDay(int year, int month, int day)
{
    if (isLeapYear(year))
        day_in_month[2] = 29;
    else
        day_in_month[2] = 28;
    if (day > 0 && day < day_in_month[month])
        return true;
    else
        return false;
}