#include "AgendaService.hpp"
#include <iostream>
AgendaService::AgendaService()
{
    startAgenda();
}
AgendaService::~AgendaService()
{
    quitAgenda();
}
bool AgendaService::userLogIn(const std::string &userName, const std::string &password)
{
    auto user_filter = [&userName, &password](User t_user) {
        return t_user.getName() == userName && t_user.getPassword() == password;
    };
    return !m_storage->queryUser(user_filter).empty();
}
bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                                 const std::string &email, const std::string &phone)
{
    //check unique
    auto user_list = m_storage->queryUser([&userName](User t_user) { return t_user.getName() == userName; });
    if (user_list.empty())
    {
        m_storage->createUser(User(userName, password, email, phone));
        return true;
    }
    else
        return false;
}
bool AgendaService::deleteUser(const std::string &userName, const std::string &password)
{
    if (userLogIn(userName, password))
    {
        deleteAllMeetings(userName);
        m_storage->deleteUser([&userName](User t_user) { return t_user.getName() == userName; });
        return true;
    }
    return false;
}
std::list<User> AgendaService::listAllUsers(void) const
{
    return m_storage->queryUser([](const User &i) { return true; });
}
bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                                  const std::string &startDate, const std::string &endDate,
                                  const std::vector<std::string> &participator)
{
    Date t_startDate(startDate);
    Date t_endDate(endDate);
    if (!Date::isValid(t_startDate) || !Date::isValid(t_endDate))
    {
        std::cout << "check time fail" << std::endl;
    }
    auto title_fliter = [&title](Meeting t_meeting) { return t_meeting.getTitle() == title; };
    //check all register
    auto username_fliter = [&userName, &participator](User t_user) { 
        if(t_user.getName()==userName)
            return true; 
        else{for (auto &part:participator){
            if(t_user.getName()==part){
                return true;
            }
        }
        return false;
    } };
    auto time_fliter = [&t_startDate, &t_endDate](Meeting t_meeting) {
        return !(t_startDate >= t_meeting.getEndDate() || t_endDate <= t_meeting.getStartDate());
    };
    if (m_storage->queryMeeting(title_fliter).empty() &&
        m_storage->queryUser(username_fliter).size() == (participator.size() + 1) &&
        m_storage->queryMeeting(time_fliter).empty())
    {
        m_storage->createMeeting(Meeting(userName, participator, t_startDate, t_endDate, title));
        return true;
    }
    return false;
}
bool AgendaService::addMeetingParticipator(const std::string &userName,
                                           const std::string &title,
                                           const std::string &participator)
{
    auto meeting_fliter = [&title, &userName](const Meeting &t_meeting) { return t_meeting.getTitle() == title && t_meeting.getSponsor() == userName; };
    auto meeting_switch = [&participator](Meeting &t_meeting) {
        t_meeting.addParticipator(participator);
    };
    m_storage->updateMeeting(meeting_fliter, meeting_switch);
    return true;
}
bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                              const std::string &title,
                                              const std::string &participator)
{
    auto meeting_fliter = [&title, &userName](const Meeting &t_meeting) { return t_meeting.getTitle() == title && t_meeting.getSponsor() == userName; };
    auto meeting_switch = [&participator](Meeting &t_meeting) {
        t_meeting.removeParticipator(participator);
    };
    m_storage->updateMeeting(meeting_fliter, meeting_switch);
    return true;
}
bool AgendaService::quitMeeting(const std::string &userName, const std::string &title)
{
    auto meeting_fliter = [&title](const Meeting &t_meeting) {
        return t_meeting.getTitle() == title;
    };
    auto meeting_switcher = [&userName](Meeting &t_meeting) {
        if (t_meeting.isParticipator(userName))
        {
            t_meeting.removeParticipator(userName);
        }
    };
    m_storage->updateMeeting(meeting_fliter, meeting_switcher);
    return true;
}
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                               const std::string &title) const
{
    auto meeting_fliter = [&userName, &title](const Meeting &t_meeting) {
        return t_meeting.getTitle() == title && (t_meeting.getSponsor() == userName || t_meeting.isParticipator(userName));
    };
    return m_storage->queryMeeting(meeting_fliter);
}
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                               const std::string &startDate,
                                               const std::string &endDate) const
{
    auto meeting_fliter = [&userName, &startDate, &endDate](const Meeting &t_meeting) {
        return (!(t_meeting.getStartDate() > endDate) && !(t_meeting.getEndDate() > startDate)) && (t_meeting.getSponsor() == userName || t_meeting.isParticipator(userName));
    };
    return m_storage->queryMeeting(meeting_fliter);
}
std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const
{
    return m_storage->queryMeeting([](const Meeting t_meeting) { return true; });
}
std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const
{
    auto meeting_fliter = [&userName](const Meeting &t_meeting) {
        return t_meeting.getSponsor() == userName;
    };
    return m_storage->queryMeeting(meeting_fliter);
}
std::list<Meeting> AgendaService::listAllParticipateMeetings(const std::string &userName) const
{
    auto meeting_fliter = [&userName](const Meeting &t_meeting) {
        return t_meeting.isParticipator(userName);
    };
    return m_storage->queryMeeting(meeting_fliter);
}
bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title)
{
    m_storage->deleteMeeting([&userName, &title](const Meeting &t_meeting) {
        return userName == t_meeting.getSponsor() && title == t_meeting.getTitle();
    });
    return true;
}
bool AgendaService::deleteAllMeetings(const std::string &userName)
{
    m_storage->deleteMeeting([&userName](const Meeting &t_meeting) {
        return userName == t_meeting.getSponsor();
    });
    return true;
}
void AgendaService::startAgenda(void)
{
    m_storage = Storage::getInstance();
}
void AgendaService::quitAgenda(void)
{
    m_storage->sync();
}