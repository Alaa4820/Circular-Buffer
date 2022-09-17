#include "negative_queue_size_exception.h"

negative_queue_size_exception::negative_queue_size_exception(const std::string &message) 
    : std::runtime_error(message) {}




