#include "AgendaUI.hpp"
using namespace std;
std::string ui_helper =
    "---------------------------Agenda---------------------------\n"
    "Action :\n"
    "l    - log in Agenda by username and password\n"
    "r    - register an Agenda account\n"
    "q    - quit Agenda\n"
    "------------------------------------------------------------\n";
std::string ui_logged_helper =
    "---------------------------Agenda---------------------------\n"
    "o    - log out Agenda"
    "dc   - delete Agenda account"
    "lu   - list all Agenda user"
    "cm   - create a meeting"
    "amp  - add meeting participator"
    "rmp  - remove meeting participator"
    "rqm  - request to quit meeting"
    "la   - list all meetings"
    "las  - list all sponsor meetings"
    "lap  - list all participator meetings"
    "qm   - query meeting by title"
    "qt   - query meeting by time interval"
    "dm   - delete meeting by title"
    "da   - delete all meetings"
    "------------------------------------------------------------\n";
AgendaUI::AgendaUI() : m_userName(""),
                       m_userPassword("")
{
    startAgenda();
}
void AgendaUI::OperationLoop(void)
{
    string op = "";
    while (true)
    {
        if (m_userName == "")
        {
            cout << ui_helper;
            if (op == "q")
            {
                quitAgenda();
                return;
            }
            else if (op == "l")
            {
                userLogIn();
            }
            else if (op == "r")
            {
                userRegister();
            }
        }
        else
        {
            cout << ui_logged_helper;
            executeOperation(op);
        }
    }
}
void AgendaUI::startAgenda(void)
{
    OperationLoop();
}
std::string AgendaUI::getOperation()
{
    cout << "Agenda";
    if (m_userName == "")
    {
        cout << " : ~$";
    }
    else
    {
        cout << "@" << m_userName << " : ~#";
    }
    string op;
    cin >> op;
    return op;
}
bool AgendaUI::executeOperation(std::string t_operation)
{
    if (t_operation == "o")
        userLogOut();
    else if (t_operation == "dc")
        deleteUser();
    else if (t_operation == "lu")
        listAllUsers();
    else if (t_operation == "cm")
        createMeeting();
    else if (t_operation == "amp")
        addMeetingParticipator();
    else if (t_operation == "rmp")
        removeMeetingParticipator();
    else if (t_operation == "rqm")
        requestQuitMeeting();
    else if (t_operation == "la")
        listAllMeetings();
    else if (t_operation == "las")
        listAllSponsorMeetings();
    else if (t_operation == "lap")
        listAllParticipateMeetings();
    else if (t_operation == "qt")
        queryMeetingByTimeInterval();
    else if (t_operation == "qm")
        queryMeetingByTitle();
    else if (t_operation == "dm")
        deleteMeetingByTitle();
    else if (t_operation == "da")
        deleteAllMeetings();
    else
    {
        cout << "error opration" << endl;
    }
}
void AgendaUI::userLogIn(void)
{
    string module_str = "[log in] ";
    string username, password;
    cout << module_str << "[username] [password]" << endl;
    cout << module_str;
    cin >> username >> password;
    if (m_agendaService.userLogIn(username, password))
    {
        m_userName == username;
        m_userPassword == password;
        cout << module_str << "Succed!" << endl;
        OperationLoop();
    }
    else
    {
        cout << module_str << "Password error or user doesn't exist" << endl;
    }
}
void AgendaUI::userRegister(void)
{
    string module_str = "[register] ";
    string username, password, email, phone;
    cout << module_str << "[username] [password] [email] [phone]" << endl;
    cout << module_str;
    cin >> username >> password >> email >> phone;
    if (m_agendaService.userRegister(username, password, email, phone))
    {
        cout << module_str << "Succed!" << endl;
    }
    else
    {
        cout << module_str << "This username has been registered!" << endl;
    }
}
void AgendaUI::userLogOut(void)
{
    m_userName = "";
    m_userPassword = "";
}
void AgendaUI::quitAgenda(void)
{
    m_agendaService.quitAgenda();
}
void AgendaUI::deleteUser(void)
{
    string module_str = "[delete agenda account] ";
    m_agendaService.deleteUser(m_userName, m_userPassword);
    cout << module_str << "Succed!" << endl;
    m_userName = "";
    m_userPassword = "";
}
void AgendaUI::listAllUsers(void)
{
    string module_str = "[list all users] ";
    auto user_list = m_agendaService.listAllUsers();
    cout << module_str << "name\t\temail\t\tphone" << endl;
    for (auto user : user_list)
    {
        cout << module_str << user.getName() << "\t\t" << user.getEmail() << "\t\t" << user.getPhone() << "\t\t";
    }
}
void AgendaUI::createMeeting(void)
{
    string module_str = "[create meeting] ";
    int number;
    vector<string> participators;
    string title, startTime, endTime;
    cout << module_str << "[the number of participators]" << endl;
    cout << module_str;
    cin >> number;
    for (int i = 0; i < number; i++)
    {
        cout << module_str << "please enter the participator " << i << "]" << endl;
        cout << module_str;
        participators.push_back([]() -> string { string temp; cin>>temp; return temp; }());
    }
    cout << module_str << "[title] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]" << endl;
    cout << module_str;
    cin >> title >> startTime >> endTime;
    if (m_agendaService.createMeeting(m_userName, title, startTime, endTime, participators))
    {
        cout << module_str << "Succed!" << endl;
    }
    else
    {
        cout << "[create meeting] error!" << endl;
    }
}
void AgendaUI::listAllMeetings(void)
{
    printMeetings(m_agendaService.listAllMeetings(m_userName));
}
void AgendaUI::listAllSponsorMeetings(void)
{
    printMeetings(m_agendaService.listAllSponsorMeetings(m_userName));
}
void AgendaUI::listAllParticipateMeetings(void)
{
    printMeetings(m_agendaService.listAllParticipateMeetings(m_userName));
}
void AgendaUI::queryMeetingByTitle(void)
{
    string module_str = "[query meeting] ";
    cout << module_str << "[title]:" << endl;
    cout << "[query meeting] ";
    printMeetings(m_agendaService.meetingQuery(m_userName, []() -> string { string temp; cin>>temp; return temp; }()));
}
void AgendaUI::queryMeetingByTimeInterval(void)
{
    string module_str = "[query meeting] ";
    string startTime, endTime;
    cout << module_str << "[start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]" << endl;
    cin >> startTime >> endTime;
    printMeetings(m_agendaService.meetingQuery(m_userName, startTime, endTime));
}
void AgendaUI::deleteMeetingByTitle(void)
{
    string module_str = "[delete meeting] ";
    cout << module_str;
    m_agendaService.deleteMeeting(m_userName, []() -> string { string temp; cin>>temp; return temp; }());
    cout << module_str << "Succed!" << endl;
}
void AgendaUI::deleteAllMeetings(void)
{
    string module_str = "[delete all meeting] ";
    m_agendaService.deleteAllMeetings(m_userName);
    cout << module_str << "Succed!" << endl;
}
void AgendaUI::printMeetings(std::list<Meeting> t_meetings)
{
    string module_str = "[print meeting] ";
    cout << module_str << "title\t\tsponsor\t\tstart time\t\tend time\t\tparticipator" << endl;
    for (auto &meeting : t_meetings)
    {
        cout << module_str << meeting.getTitle() << "\t\t" << meeting.getSponsor() << "\t\t" << Date::dateToString(meeting.getStartDate()) << "\t\t" << Date::dateToString(meeting.getEndDate()) << "\t\t"
             << [&meeting]() {
                    string ret;
                    for (auto part : meeting.getParticipator())
                    {
                        ret += part + ",";
                    }
                    ret.erase(ret.end() - 1);
                    return ret;
                }()
             << endl;
    }
}
void AgendaUI::requestQuitMeeting(void)
{
    string module_str = "[quit meeting] ";
    cout << module_str << "[meeting title]" << endl;
    cout << module_str;
    string meeting;
    cin >> meeting;
    m_agendaService.quitMeeting(m_userName, meeting);
    cout << module_str << "Succed!" << endl;
}
void AgendaUI::addMeetingParticipator(void)
{
    string module_str = "[add participator] ";
    cout << module_str << "[meeting title] [participator username]";
    cout << module_str;
    string title, user;
    cin >> title >> user;
    m_agendaService.addMeetingParticipator(m_userName, title, user);
    cout << module_str << "Succed!" << endl;
}
void AgendaUI::removeMeetingParticipator(void)
{
    string module_str = "[remove participator] ";
    cout << module_str << "[meeting title] [participator username]";
    cout << module_str;
    string title, user;
    cin >> title >> user;
    m_agendaService.addMeetingParticipator(m_userName, title, user);
    cout << module_str << "Succed!" << endl;
}
