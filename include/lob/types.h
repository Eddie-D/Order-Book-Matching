#pragma once

#include <cstdint>
#include <iostream>

namespace lob {

using OrderId = std::uint64_t;
using Price = std::uint32_t;
using Qty = std::uint32_t;

enum class Type : std::uint8_t { New, Cancel };
enum class Side : std::uint8_t { Buy, Sell };

struct Message {
  OrderId id;
  Price price;
  Qty qty;
  Type type;
  Side side;
};

enum class Kind : std::uint8_t { Trade, Cancel, Reject };
struct Event {
  Kind kind;
  Price price;
  Qty qty;
  OrderId sellId;
  OrderId buyId;
};

struct Order {
  OrderId id;
  Qty qty;
};
struct Location {
  Side side;
  Price price;
};

inline std::ostream &operator<<(std::ostream &os, const Message &m) {
  const std::string t = (m.type == Type::New) ? "NEW" : "CANCEL";
  const std::string s = (m.side == Side::Buy) ? "B" : "S";
  os << t << "," << m.id << "," << s << "," << m.price << "," << m.qty;
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Event &t) {
  switch (t.kind) {
  case Kind::Trade:
    os << "TRADE," << t.price << "," << t.qty << "," << t.sellId << ","
       << t.buyId;
    break;
  case Kind::Cancel:
    os << "CANCEL," << t.sellId << "," << t.qty;
    break;
  case Kind::Reject:
    os << "REJECT";
    break;
  }
  return os;
}
} // namespace lob
