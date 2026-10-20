#include "lob/types.h"
#include <deque>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace lob {

class OrderBook {
public:
  void process(const Message &m, std::vector<Event> &out);

  std::optional<Price> best_price(const Side &s) const;

private:
  void add(const Message &m, std::vector<Event> &out);
  void cancel(OrderId id, std::vector<Event> &out);

  // Match an incoming order. Returns the quantity left over.
  Qty match(const Message &m, std::vector<Event> &out);

  // Place unfilled quantity on the book.
  void rest(const Message &m, Qty remaining);

  Side opposite(const Side s);

  std::map<Price, std::deque<Order>> bids_;     // begin() = best
  std::map<Price, std::deque<Order>> asks_;     // end() = best
  std::unordered_map<OrderId, Location> where_; // resting orders only
};

} // namespace lob
