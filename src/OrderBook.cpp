#include "lob/OrderBook.h"
#include "lob/types.h"
#include <algorithm>
#include <deque>
#include <optional>

namespace lob {
void OrderBook::process(const Message &m, std::vector<Event> &out) {
  m.type == Type::New ? add(m, out) : cancel(m.id, out);
}

void OrderBook::add(const Message &m, std::vector<Event> &out) {
  auto qty = match(m, out);
  rest(m, qty);
}

Qty OrderBook::match(const Message &m, std::vector<Event> &out) {
  Qty remaining = m.qty;
  while (remaining > 0) {
    auto &target = m.side == Side::Sell ? bids_ : asks_;
    auto b = best_price(opposite(m.side));

    if (!(b.has_value() && ((m.side == Side::Buy && b.value() <= m.price) ||
                            (m.side == Side::Sell && b.value() >= m.price)))) {
      return remaining;
    }

    auto &other = target[b.value()].front();

    const Qty trade_qty = std::min(other.qty, remaining);
    other.qty -= trade_qty;
    remaining -= trade_qty;

    auto sell_id = m.side == Side::Sell ? m.id : other.id;
    auto buy_id = m.side == Side::Buy ? m.id : other.id;
    out.push_back({Kind::Trade, b.value(), trade_qty, sell_id, buy_id});

    if (other.qty <= 0) {
      where_.erase(other.id);
      target[b.value()].pop_front();
      if (target[b.value()].empty()) {
        target.erase(b.value());
      }
    }
  }
  return 0;
}

void OrderBook::rest(const Message &m, Qty remaining) {
  if (remaining > 0) {
    auto &target = m.side == Side::Sell ? asks_ : bids_;
    target[m.price].push_back({m.id, remaining});
    where_[m.id] = {m.side, m.price};
  }
}

Side OrderBook::opposite(const Side s) {
  return s == Side::Buy ? Side::Sell : Side::Buy;
}

std::optional<Price> OrderBook::best_price(const Side &s) const {
  if (s == Side::Buy) {
    // Best bid is the highest price (end of the map)
    return bids_.empty() ? std::nullopt
                         : std::optional<Price>(std::prev(bids_.end())->first);
  } else {
    // Best ask is the lowest price (beginning of the map)
    return asks_.empty() ? std::nullopt
                         : std::optional<Price>(asks_.begin()->first);
  }
}

void OrderBook::cancel(OrderId id, std::vector<Event> &out) {
  auto loc_it = where_.find(id);
  if (loc_it == where_.end()) {
    out.push_back({Kind::Reject, 0, 0, id, 0});
    return;
  }

  auto loc = loc_it->second;
  auto &queue = (loc.side == Side::Buy ? bids_ : asks_)[loc.price];

  for (auto it = queue.begin(); it != queue.end(); ++it) {
    if (it->id == id) {
      out.push_back({Kind::Cancel, 0, it->qty, id, 0});
      queue.erase(it);

      where_.erase(id);

      if (queue.empty()) {
        (loc.side == Side::Buy ? bids_ : asks_).erase(loc.price);
      }
      break;
    }
  }
}
} // namespace lob
