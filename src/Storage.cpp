#include "Storage.hpp"
#include "Path.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <algorithm>
#include <string>
#include <cstdio>

std::shared_ptr<Storage> Storage::m_instance = nullptr;
Storage::Storage()
{
    try
    {
        readFromFile();
    }
    catch (std::string error_str)
    {
        std::cout << error_str << std::endl;
    }
    m_dirty = false;
}
bool Storage::readFromFile(void)
{
    std::fstream file_meeting, file_user;
    file_meeting.open(Path::meetingPath, std::ios::in);
    file_user.open(Path::userPath, std::ios::in);
    if (!file_meeting.is_open() || !file_user.is_open())
    {
        throw "Read from file fail.";
    }
    std::string user_buf;
    //read user
    std::regex user_pattern("\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\"");
    std::smatch result_user;
    while (file_user.peek() != EOF)
    {
        std::getline(file_user, user_buf);
        std::regex_match(user_buf, result_user, user_pattern);
        if (result_user.size() != 5)
        {
            throw "file format illegal";
        }
        createUser(User(result_user[1], result_user[2], result_user[3], result_user[4]));
    }
    file_user.close();
    //read meeting
    std::string meeting_buf;
    std::string meeting_participator_buf;
    std::regex meeting_pattern("\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\"");
    std::smatch result_meeting;
    while (file_meeting.peek() != EOF)
    {
        std::getline(file_meeting, meeting_buf);
        std::regex_match(meeting_buf, result_meeting, meeting_pattern);
        if (result_meeting.size() != 6)
        {
            throw "file format illegal";
        }
        //split meeting parcitopator
        std::vector<std::string> meeting_parcitopator;
        std::regex meeting_participator_pattern("(.*?)&");
        std::smatch result_parcitopators;
        std::string meeting_parcitopator_str = result_meeting[2];
        meeting_parcitopator_str += "&";
        std::string::const_iterator iterStart = meeting_parcitopator_str.begin();
        std::string::const_iterator iterEnd = meeting_parcitopator_str.end();
        while (regex_search(iterStart, iterEnd, result_parcitopators, meeting_participator_pattern))
        {
            meeting_parcitopator.push_back(result_parcitopators[1]);
            iterStart = result_parcitopators[0].second; //更新搜索起始位置,搜索剩下的字符串
        }
        createMeeting(Meeting(result_meeting[1], meeting_parcitopator, Date::stringToDate(result_meeting[3]), Date::stringToDate(result_meeting[4]), result_meeting[5]));
    }
    file_meeting.close();
    return true;
}
bool Storage::writeToFile(void)
{
    std::fstream file_meeting, file_user;
    file_meeting.open(Path::meetingPath, std::ios::out);
    file_user.open(Path::userPath, std::ios::out);
    if (!file_user.is_open() || !file_meeting.is_open())
        throw "write file fail";
    //write user file
    for (auto user : m_userList)
    {
        char user_str[200];
        file_user << [&user_str](User t_user) -> std::string {
            sprintf(user_str, "\"%s\",\"%s\",\"%s\",\"%s\"", t_user.getName().c_str(), t_user.getPassword().c_str(), t_user.getEmail().c_str(), t_user.getPhone().c_str()); //test pass @8/25 2:50AM
            return std::string(user_str);
        }(user) << std::endl;
    }
    file_user.close();
    //write meeting file
    for (auto meeting : m_meetingList)
    {
        char meeting_str[200];

        file_user << [&meeting_str](Meeting t_meeting) -> std::string {
            std::string participator_str;
            for (auto participator : t_meeting.getParticipator())
            {
                participator_str += (participator + '&');
            }
            participator_str.erase(participator_str.end() - 1);

            sprintf(meeting_str, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"", t_meeting.getSponsor().c_str(), participator_str.c_str(), Date::dateToString(t_meeting.getStartDate()).c_str(), Date::dateToString(t_meeting.getEndDate()).c_str(), t_meeting.getTitle().c_str());
            return std::string(meeting_str);
        }(meeting) << std::endl;
    }
    file_meeting.close();
    return true;
}
std::shared_ptr<Storage> Storage::getInstance(void)
{
    if (m_instance == nullptr)
        return m_instance = std::make_shared<Storage>();
}
Storage::~Storage()
{
    sync();
    m_instance = nullptr;
}
void Storage::createUser(const User &t_user)
{
    if (std::find(m_userList.begin(), m_userList.end(), t_user) == m_userList.end())
    {
        m_userList.push_back(t_user);
        m_dirty = true;
    }
}
std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const
{
    std::list<User> ret_list;
    for (auto &user : m_userList)
    {
        if (filter(user))
            ret_list.push_back(user);
    }
    return ret_list;
}
int Storage::updateUser(std::function<bool(const User &)> filter,
                        std::function<void(User &)> switcher)
{
    int ret_count = 0;
    for (auto &user : m_userList)
    {
        if (filter(user))
        {
            ret_count++;
            m_dirty = true;
            switcher(user);
        }
    }
    return ret_count;
}
int Storage::deleteUser(std::function<bool(const User &)> filter)
{
    int ret_count = 0;
    for (auto iter = m_userList.begin(); iter != m_userList.end(); iter++)
    {
        if (filter(*iter))
        {
            m_userList.erase(iter);
            ret_count++;
            m_dirty = true;
        }
        else
        {
            iter++;
        }
    }
    return ret_count;
}
void Storage::createMeeting(const Meeting &t_meeting)
{
    if (std::find(m_meetingList.begin(), m_meetingList.end(), t_meeting) == m_meetingList.end())
    {
        m_meetingList.push_back(t_meeting);
        m_dirty = true;
    }
}
std::list<Meeting> Storage::queryMeeting(
    std::function<bool(const Meeting &)> filter) const
{
    std::list<Meeting> ret_list;
    for (auto &meeting : m_meetingList)
    {
        if (filter(meeting))
            ret_list.push_back(meeting);
    }
    return ret_list;
}
int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                           std::function<void(Meeting &)> switcher)
{
    int ret_count = 0;
    for (auto &meeting : m_meetingList)
    {
        if (filter(meeting))
        {
            ret_count++;
            m_dirty = true;
            switcher(meeting);
        }
    }
    return ret_count;
}
int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter)
{
    int ret_count = 0;
    for (auto iter = m_meetingList.begin(); iter != m_meetingList.end(); iter++)
    {
        if (filter(*iter))
        {
            m_meetingList.erase(iter);
            ret_count++;
            m_dirty = true;
        }
        else
        {
            iter++;
        }
    }
    return ret_count;
}
bool Storage::sync(void)
{
    if (m_dirty)
    {
        try
        {
            writeToFile();
        }
        catch (std::string error)
        {
            std::cout << error << std::endl;
        }
        m_dirty = false;
        return true;
    }
    return false;
}
