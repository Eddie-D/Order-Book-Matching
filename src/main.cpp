#include "lob/OrderBook.h"
#include "lob/io.h"
#include "lob/types.h"

int main() {
  auto messages = lob::load_messages("Books/Book.csv");

  auto book = new lob::OrderBook();
  auto out = std::vector<lob::Event>();
  for (auto it = messages.begin(); it != messages.end(); ++it) {
    book->process(*it, out);
  }

  lob::save_trades("Books/CPP_Trades.csv", out);

  return 0;
}
