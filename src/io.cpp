#include "lob/io.h"
#include "lob/OrderBook.h"
#include <charconv>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace lob {
namespace {
std::string_view next_field(std::string_view &sv) {
  auto comma = sv.find(',');
  auto field = sv.substr(0, comma);
  sv = (comma == sv.npos) ? std::string_view{} : sv.substr(comma + 1);
  return field;
}

template <typename T> T parse(std::string_view s) {
  T value{};
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
  if (ec != std::errc{})
    throw std::runtime_error("bad number: " + std::string(s));
  return value;
}

} // namespace

std::vector<Message> load_messages(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("cannot open file: " + path);

  std::vector<Message> messages;
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#' || line.rfind("Type", 0) == 0)
      continue;

    std::string_view sv = line;
    auto type = next_field(sv);

    Message m{};
    m.id = parse<OrderId>(next_field(sv));
    if (type == "NEW") {
      m.type = Type::New;
      m.side = next_field(sv) == "B" ? Side::Buy : Side::Sell;
      m.price = parse<Price>(next_field(sv));
      m.qty = parse<Qty>(next_field(sv));
    } else if (type == "CANCEL") {
      m.type = Type::Cancel;
    } else {
      throw std::runtime_error("bad line: " + line);
    }
    messages.push_back(m);
  }
  return messages;
}

void save_trades(const std::string &path, const std::vector<Event> &trades) {
  std::ofstream file(path);
  if (!file.is_open())
    throw std::runtime_error("cannot open file: " + path);

  file << "# TRADE,Price,Quantity,Seller_Id,Buyer_Id" << "\n"
       << "# CANCEL,Id,Remaining_Quantity" << "\n";

  for (auto it = trades.begin(); it != trades.end(); ++it) {
    file << *it << "\n";
  }

  file.close();
}

} // namespace lob
