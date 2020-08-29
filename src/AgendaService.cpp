#include "AgendaService.hpp"
#include <iostream>
AgendaService::AgendaService()
{
    //std::cout << "service start" << std::endl;
    startAgenda();
}
AgendaService::~AgendaService()
{
    quitAgenda();
}
bool AgendaService::userLogIn(const std::string &userName, const std::string &password)
{
    auto user_filter = [&userName](const User &t_user) {
        return t_user.getName() == userName;
    };
    auto user_list = m_storage->queryUser(user_filter);
    if (user_list.size() == 0)
    {
        return false;
    }
    else
    {
        if (user_list.front().getPassword() == password)
        {
            return true;
        }
        else
            return false;
    }
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

    auto user_list = m_storage->queryUser([&](const User &t_user) {
        return t_user.getName() == userName;
    });
    if (user_list.size() == 0)
    {
        return false;
    }
    else
    {
        if (user_list.front().getPassword() == password)
        {
            m_storage->deleteMeeting([&](const Meeting &t_meeting) {
                return t_meeting.getSponsor() == userName;
            });
            auto a_meetings = m_storage->queryMeeting([&](const Meeting &t_meeting) { return true; });
            for (auto &meeting : a_meetings)
            {
                for (auto &part : meeting.getParticipator())
                {
                    if (userName == part)
                    {
                        removeMeetingParticipator(meeting.getSponsor(), meeting.getTitle(), userName);
                    }
                }
            }
            m_storage->deleteUser([&](const User &user) {
                return user.getName() == userName;
            });
            return true;
        }
        else
            return false;
    }
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
        //std::cout << "check time fail" << std::endl;
        return false;
    }
    if (t_startDate >= t_endDate)
    {
        //std::cout << "meetings time error" << std::endl;
        return false;
    }
    auto title_fliter = [&title](Meeting t_meeting) { return t_meeting.getTitle() == title; };
    //check part exist
    if (participator.size() == 0)
    {
        return false;
    }

    //check title unique
    if (!(m_storage->queryMeeting(title_fliter).empty()))
    {
        return false;
    }
    //check user all register
    auto username_fliter = [&userName](User t_user) {
        if (t_user.getName() == userName)
            return true;
    };
    if (m_storage->queryUser(username_fliter).size() != 1)
    {
        return false;
    }
    for (auto &part : participator)
    {
        auto part_fliter = [&part](User t_user) {
            if (t_user.getName() == part)
                return true;
        };
        if (m_storage->queryUser(part_fliter).size() != 1)
        {
            return false;
        }
    }

    //check participater avaliable
    /*for (auto &part : participator)
    {
        auto meet_list = listAllMeetings(part);
        for (auto &meet : meet_list)
        {
            if (!(meet.getStartDate() >= endDate || meet.getEndDate() <= startDate))
            {
                return false;
            }
        }
    }
    //check sponsor avalible
    auto sp_meet_list = listAllMeetings(userName);
    for (auto &meet : sp_meet_list)
    {
        if (!(meet.getStartDate() >= endDate || meet.getEndDate() <= startDate))
        {
            return false;
        }
    }*/

    m_storage->createMeeting(Meeting(userName, participator, t_startDate, t_endDate, title));
    return true;
}
bool AgendaService::addMeetingParticipator(const std::string &userName,
                                           const std::string &title,
                                           const std::string &participator)
{
    //check userName register
    auto users_fliter = [&userName](const User &user) {
        return userName == user.getName();
    };
    if (m_storage->queryUser(users_fliter).empty())
        return false;
    //check participator register
    auto user_fliter = [&participator](const User &user) {
        return participator == user.getName();
    };
    if (m_storage->queryUser(user_fliter).empty())
        return false;

    //check meeting exist
    auto meeting_title_fliter = [&title](const Meeting t_meeting) {
        return t_meeting.getTitle() == title;
    };
    auto meeting_list = m_storage->queryMeeting(meeting_title_fliter);
    if (meeting_list.empty())
        return false;
    auto the_meeting = meeting_list.front();
    //check part time avaible
    auto part_meetings = listAllMeetings(participator);
    for (auto meet : part_meetings)
    {
        if (!(the_meeting.getEndDate() <= meet.getStartDate() || the_meeting.getStartDate() >= meet.getEndDate()))
            return false;
    }
    //check is a participator
    if (the_meeting.isParticipator(participator))
    {
        return false;
    }
    if (the_meeting.getSponsor() == participator)
    {
        return false;
    }
    if (the_meeting.getSponsor() != userName)
    {
        return false;
    }
    auto op_meet_fliter = [&](const Meeting &t_meeting) {
        return t_meeting.getTitle() == the_meeting.getTitle();
    };
    auto op_meet_switch = [&](Meeting &t_meeting) {
        return t_meeting.addParticipator(participator);
    };
    m_storage->updateMeeting(op_meet_fliter, op_meet_switch);
    return true;
}
bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                              const std::string &title,
                                              const std::string &participator)
{
    //check userName register
    auto users_fliter = [&userName](const User &user) {
        return userName == user.getName();
    };
    if (m_storage->queryUser(users_fliter).empty())
        return false;
    //check participator register
    auto user_fliter = [&participator](const User &user) {
        return participator == user.getName();
    };
    if (m_storage->queryUser(user_fliter).empty())
        return false;

    //check meeting exist
    auto meeting_title_fliter = [&title](const Meeting t_meeting) {
        return t_meeting.getTitle() == title;
    };
    auto meeting_list = m_storage->queryMeeting(meeting_title_fliter);
    if (meeting_list.empty())
        return false;
    auto the_meeting = meeting_list.front();
    if (the_meeting.getSponsor() != userName)
    {
        return false;
    }
    //check part is a part
    if (!the_meeting.isParticipator(participator))
    {
        return false;
    }

    auto op_meet_fliter = [&](const Meeting &t_meeting) {
        return t_meeting.getTitle() == the_meeting.getTitle();
    };
    auto op_meet_switch = [&](Meeting &t_meeting) {
        t_meeting.removeParticipator(participator);
    };
    m_storage->updateMeeting(op_meet_fliter, op_meet_switch);

    if (the_meeting.getParticipator().size() == 1)
    {
        deleteMeeting(userName, title);
    }
    return true;
}
bool AgendaService::quitMeeting(const std::string &userName, const std::string &title)
{
    auto list_meetings = m_storage->queryMeeting([&](const Meeting &t_meeting) {
        return t_meeting.isParticipator(userName) && t_meeting.getTitle() == title;
    });
    Meeting the_meeting;
    if (list_meetings.empty())
    {
        return false;
    }
    else
    {
        the_meeting = list_meetings.front();
        auto meet_fliter = [&](const Meeting &t_meeting) { return t_meeting.getTitle() == the_meeting.getTitle(); };
        auto meet_switch = [&](Meeting &t_meeting) {
            t_meeting.removeParticipator(userName);
        };
        m_storage->updateMeeting(meet_fliter, meet_switch);
        if (the_meeting.getParticipator().size() == 1)
        {
            deleteMeeting(the_meeting.getSponsor(), title);
        }
    }
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
    std::list<Meeting> ret;
    Date start = Date::stringToDate(startDate);
    Date end = Date::stringToDate(endDate);
    if ((!Date::isValid(start)) || (!Date::isValid(end)) || (start > end))
        return ret;
    auto meeting_fliter = [&](const Meeting &t_meeting) {
        if (t_meeting.getSponsor() == userName || t_meeting.isParticipator(userName))
        {
            if (!(t_meeting.getEndDate() < start || t_meeting.getStartDate() > end))
                return true;
        }
        return false;
    };
    ret = m_storage->queryMeeting(meeting_fliter);
    return ret;
}
std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const
{
    return m_storage->queryMeeting([userName](const Meeting t_meeting) {
        return t_meeting.getSponsor() == userName || t_meeting.isParticipator(userName);
    });
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
    int ret = m_storage->deleteMeeting([&userName, &title](const Meeting &t_meeting) {
        return userName == t_meeting.getSponsor() && title == t_meeting.getTitle();
    });
    if (ret > 0)
        return true;
    else
        return false;
}
bool AgendaService::deleteAllMeetings(const std::string &userName)
{
    int ret = m_storage->deleteMeeting([&userName](const Meeting &t_meeting) {
        return userName == t_meeting.getSponsor();
    });
    if (ret > 0)
        return true;
    else
        return false;
}
void AgendaService::startAgenda(void)
{
    m_storage = Storage::getInstance();
}
void AgendaService::quitAgenda(void)
{
    m_storage->sync();
}
