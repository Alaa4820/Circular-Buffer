#include "empty_queue_exception.h"

empty_queue_exception::empty_queue_exception(const std::string &message) 
    : std::runtime_error(message) {}




