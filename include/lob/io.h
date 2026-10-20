// include/lob/message_reader.hpp
#pragma once
#include "lob/types.h"
#include <string>
#include <vector>

namespace lob {
std::vector<Message> load_messages(const std::string &path);
void save_trades(const std::string &path, const std::vector<Event> &trades);
} // namespace lob
