/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** ThreadCreated
*/

#include "ThreadCreated.hpp"

namespace AEL {

void ThreadCreated::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, thread_uuid, user_uuid, timestamp_str;
    iss >> code >> thread_uuid >> user_uuid >> timestamp_str;
    
    time_t thread_timestamp = 0;
    try {
        thread_timestamp = static_cast<time_t>(std::stoll(timestamp_str));
    } catch (...) {
        thread_timestamp = 0;
    }
    
    std::string thread_title;
    std::string thread_body;
    std::size_t q1 = response.find('"');
    std::size_t q2 = (q1 == std::string::npos) ? std::string::npos : response.find('"', q1 + 1);
    std::size_t q3 = (q2 == std::string::npos) ? std::string::npos : response.find('"', q2 + 1);
    std::size_t q4 = (q3 == std::string::npos) ? std::string::npos : response.find('"', q3 + 1);
    if (q1 != std::string::npos && q2 != std::string::npos && q3 != std::string::npos && q4 != std::string::npos) {
        thread_title = response.substr(q1 + 1, q2 - q1 - 1);
        thread_body = response.substr(q3 + 1, q4 - q3 - 1);
    } else {
        iss >> thread_title;
        std::getline(iss, thread_body);
        if (!thread_body.empty() && thread_body[0] == ' ')
            thread_body.erase(0, 1);
    }
    
    if (!thread_uuid.empty() && !user_uuid.empty() && !thread_title.empty()) {
        std::cout << std::endl;
        client_event_thread_created(thread_uuid.c_str(), user_uuid.c_str(), thread_timestamp, thread_title.c_str(), thread_body.c_str());
    }
    std::cout.flush();
}

}
