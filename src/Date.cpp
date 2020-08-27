#include "Date.hpp"
#include <regex>
#include <string>
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
    if (day > 0 && day <= day_in_month[month])
        return true;
    else
        return false;
}
#ifdef DEBUG
#include <iostream>
int main()
{
    using namespace std;
    Date date1(0, 0, 0, 0, 0);
    cout << Date::dateToString(date1) << endl;
    Date date2(1234, 12, 3, 4, 54); //vaild date
    cout << Date::dateToString(date2) << endl;
    Date date3(1234, 12, 32, 4, 54); //invaild date
    cout << Date::dateToString(date3) << endl;
    Date date4(1234, 12, 3, 4, 54); //small date
    cout << Date::dateToString(date2) << endl;
    Date date5(1235, 12, 3, 4, 54); //big date
    cout << Date::dateToString(date2) << endl;
    cout << "1 small  " << (date4 < date5) << endl;
    cout << "1 small equal  " << (date4 <= date5) << endl;
    cout << "0 big  " << (date4 > date5) << endl;
    cout << "0 big equal  " << (date4 >= date5) << endl;
    Date date6(1235, 12, 3, 4, 54); //equal date5
    cout << "0 small  " << (date6 < date5) << endl;
    cout << "1 small equal  " << (date6 <= date5) << endl;
    cout << "0 big " << (date6 > date5) << endl;
    cout << "1 big equal  " << (date6 >= date5) << endl;
    cout << "1 equal  " << (date6 == date5) << endl;
    string datestr1 = "1234-02-05/10:50"; //vaild
    string datestr2 = "1234-02-5/10:50";  //invaild
    string datestr3 = "1234-02-30/10:50"; //illegal
    Date date7(datestr1);
    cout << Date::dateToString(date7) << endl;
    Date date8(datestr2);
    cout << Date::dateToString(date8) << endl;
    Date date9(datestr3);
    cout << Date::dateToString(date9) << endl;
}
#endif
Date::Date() : m_year(0), m_month(0), m_day(0), m_hour(0), m_minute(0)
{
}
Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute) : m_year(t_year), m_month(t_month), m_day(t_day), m_hour(t_hour), m_minute(t_minute) {}
Date::Date(const std::string &dateString)
{
    std::regex date_pattern("([0-9]{4})-([0-9]{2})-([0-9]{2})/([0-9]{2}):([0-9]{2})");
    std::smatch result_date;
    if (std::regex_match(dateString, result_date, date_pattern))
    {
        m_year = std::stoi(result_date[1]);
        m_month = std::stoi(result_date[2]);
        m_day = std::stoi(result_date[3]);
        m_hour = std::stoi(result_date[4]);
        m_minute = std::stoi(result_date[5]);
    }
    else
        m_year = m_month = m_day = m_hour = m_minute = 0;
}
int Date::getYear(void) const
{
    return m_year;
}
void Date::setYear(const int t_year)
{
    m_year = t_year;
}
int Date::getMonth(void) const
{
    return m_month;
}
void Date::setMonth(const int t_month)
{
    m_month = t_month;
}
int Date::getDay(void) const
{
    return m_day;
}
void Date::setDay(const int t_day)
{
    m_day = t_day;
}
int Date::getHour(void) const
{
    return m_hour;
}
void Date::setHour(const int t_hour)
{
    m_hour = t_hour;
}
int Date::getMinute(void) const
{
    return m_minute;
}
void Date::setMinute(const int t_minute)
{
    m_minute = t_minute;
}
bool Date::isValid(const Date &t_date)
{
    if (t_date.m_year < 1000 || t_date.m_year > 9999)
        return false;
    if (t_date.m_month < 1 || t_date.m_month > 12)
        return false;
    if (!isValidDay(t_date.m_year, t_date.m_month, t_date.m_day))
        return false;
    if (t_date.m_hour < 0 || t_date.m_hour > 23)
        return false;
    if (t_date.m_minute < 0 || t_date.m_month > 59)
        return false;
    return true;
}
Date Date::stringToDate(const std::string &t_dateString)
{
    return Date(t_dateString);
}
std::string Date::dateToString(const Date &t_date)
{
    if (!isValid(t_date))
    {
        return "0000-00-00/00:00";
    }
    else
    {
        std::string ret = "";
        ret += std::to_string(t_date.m_year) + "-";
        if (std::to_string(t_date.m_month).size() == 1)
            ret += "0";
        ret += std::to_string(t_date.m_month) + "-";

        if (std::to_string(t_date.m_day).size() == 1)
            ret += "0";
        ret += std::to_string(t_date.m_day) + "/";

        if (std::to_string(t_date.m_hour).size() == 1)
            ret += "0";
        ret += std::to_string(t_date.m_hour) + ":";

        if (std::to_string(t_date.m_minute).size() == 1)
            ret += "0";
        ret += std::to_string(t_date.m_minute);

        return ret;
    }
}
Date &Date::operator=(const Date &t_date)
{
    m_year = t_date.m_year;
    m_month = t_date.m_month;
    m_day = t_date.m_day;
    m_hour = t_date.m_hour;
    m_minute = t_date.m_minute;
    return *this;
}
bool Date::operator==(const Date &t_date) const
{
    long long this_date_num = m_year * 1e8 + m_month * 1e6 + m_day * 1e4 + m_hour * 1e2 + m_minute;
    long long t_date_num = t_date.m_year * 1e8 + t_date.m_month * 1e6 + t_date.m_day * 1e4 + t_date.m_hour * 1e2 + t_date.m_minute;
    return this_date_num == t_date_num;
}
bool Date::operator>(const Date &t_date) const
{
    long long this_date_num = m_year * 1e8 + m_month * 1e6 + m_day * 1e4 + m_hour * 1e2 + m_minute;
    long long t_date_num = t_date.m_year * 1e8 + t_date.m_month * 1e6 + t_date.m_day * 1e4 + t_date.m_hour * 1e2 + t_date.m_minute;
    return this_date_num > t_date_num;
}
bool Date::operator<(const Date &t_date) const
{
    long long this_date_num = m_year * 1e8 + m_month * 1e6 + m_day * 1e4 + m_hour * 1e2 + m_minute;
    long long t_date_num = t_date.m_year * 1e8 + t_date.m_month * 1e6 + t_date.m_day * 1e4 + t_date.m_hour * 1e2 + t_date.m_minute;
#ifdef DEBUG
    using namespace std;
    std::cout << endl
              << "this_date_num " << this_date_num << endl;
    std::cout << endl
              << "this_date_num " << this_date_num << endl;
#endif

    return this_date_num <
           t_date_num;
}
bool Date::operator>=(const Date &t_date) const
{
    return *this > t_date || *this == t_date;
}
bool Date::operator<=(const Date &t_date) const
{
    return *this < t_date || *this == t_date;
}
