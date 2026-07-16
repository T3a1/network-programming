/*
** EPITECH PROJECT, 2026
** G-NWP-400-LIL-4-1-myteams-2
** File description:
** UnknownThread
*/

#include "UnknownThread.hpp"

namespace AEL {

void UnknownThread::handle([[maybe_unused]]const std::string& response) {
    std::istringstream iss(response);
    std::string code, thread_uuid;
    iss >> code >> thread_uuid;
    
    if (!thread_uuid.empty())
        client_error_unknown_thread(thread_uuid.c_str());
}

}
