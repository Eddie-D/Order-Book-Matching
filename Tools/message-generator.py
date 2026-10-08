from pathlib import Path
import sys
import random
from typing import NamedTuple
from collections import deque

class Order:
    t: str
    price: int
    qty: int

class Trade(NamedTuple):
    price: int
    qty: int
    seller_id: int
    buyer_id: int

class Book :
    def __init__(self) :
        self.asks: dict[int, deque[int]]= {}
        self.bids: dict[int, deque[int]]= {}
        self.live_orders: dict[int, Order] = {} # Maps from id to quantity
        self.lowest_ask = sys.maxsize
        self.highest_bid = -1
        self.next_trade_id = 0
        self.trades = []
        self.book_log = ["Type,Id,Side,Price,Quantity"]
        self.trade_log = ["# TRADE,Price,Quantity,Seller_Id,Buyer_Id", "# CANCEL,Id,Remaining_Quantity"]

    def add_order(self, id, t, price, qty) :
        self.book_log.append(f"NEW,{id},{t},{price},{qty}")
        self.live_orders[id] = (t, price, qty)
        book = self.get_book(t)
        self.modify_high_low(t, price)

        if price in book :
            book[price].append(id)
        else :
            book[price] = deque([id])
        self.process(t)


    def get_book(self, t) -> dict[int, deque[Order]] :
        return self.asks if t == "S" else self.bids

    def modify_high_low(self, t, v) -> dict[int, deque[Order]] :
        if t == "B" :
            if v > self.highest_bid :
                self.highest_bid = v
        elif v < self.lowest_ask :
            self.lowest_ask = v

    def cancel_order(self, id) -> bool:
        if id not in self.live_orders :
            return False

        t, p, q = self.live_orders.pop(id)
        if len(self.get_book(t)[p]) > 1 :
            self.get_book(t)[p].remove(id)
        else :
            self.get_book(t).pop(p)
        self.update_extreme(t)

        self.trade_log.append(f"CANCEL,{id},{q}")
        self.book_log.append(f"CANCEL,{id},,,")

        return True

    def update_extreme(self, t) :
        if t == "S" : 
            self.lowest_ask = min(self.asks, default=sys.maxsize)
        else :
            self.highest_bid = max(self.bids, default=-1)


    def add_trade(self, p, qty, a_id, b_id) :
        self.trade_log.append(f"TRADE,{p},{qty},{a_id},{b_id}")
        self.trades.append(Trade(p, qty, a_id, b_id))

    def process(self, t) :
        while self.highest_bid >= self.lowest_ask and not (self.asks == {} or self.bids == {}):
            a_id, b_id = self.asks[self.lowest_ask][0], self.bids[self.highest_bid][0]
            a_t, a_p, a_qty  = self.live_orders[a_id]
            b_t, b_p, b_qty  = self.live_orders[b_id]

            qty = min(a_qty, b_qty)
            self.add_trade(self.highest_bid if t == "S" else self.lowest_ask, qty, a_id, b_id)

            self.reduce_order_at_start(a_id, qty)
            self.reduce_order_at_start(b_id, qty)

    def reduce_order_at_start(self, id, qty) :
        t, p, q = self.live_orders[id]
        if (q - qty > 0) :
            self.live_orders[id] = (t, p, q - qty)
        else :
            self.live_orders.pop(id)
            if len(self.get_book(t)[p]) > 1 :
                self.get_book(t)[p].popleft()
            else :
                self.get_book(t).pop(p)
            self.update_extreme(t)

    def publish(self) :
        # Make the direcory
        file_path = Path("../Books/Python_Trades.csv")
        if not file_path.parent.is_dir() :
            file_path.parent.mkdir()

        with file_path.open(mode="w") as file:
            for l in self.trade_log :
                file.write(f"{l}\n")

        file_path = Path("../Books/Book.csv")
        with file_path.open(mode="w") as file:
            for l in self.book_log:
                file.write(f"{l}\n")



random.seed(12345)

# Get number to be generated
n  = int(sys.argv[1]) if len(sys.argv) > 1 else 100


b = Book()
centre = 1220
rough_price = 1220
oid = 0
for i in range(n) :
    r = random.randrange(5)
    q = random.randrange(1,15)

    step = random.randrange(-3, 4) - (rough_price - centre) // 25   # weak pull to centre
    rough_price = max(0, rough_price + step)
    p = max(0,rough_price + step)
    rough_price += random.randrange(-4,6)

    if r == 0 :
        if not b.live_orders == {} :
            b.cancel_order(random.choice(list(b.live_orders)))
    elif r < 3 :
        b.add_order(oid, "B", p, q)
        oid += 1
    else :
        b.add_order(oid, "S", p, q)
        oid += 1

b.publish()


'''
b = Book()
b.process()
print(f"Orders: {b.live_orders}")
b.add_order(1,"B",15,10)
print(f"Orders: {b.live_orders}")
b.add_order(2,"S",12,2)
print(f"Orders: {b.live_orders}")
b.add_order(3,"S",15,16)
print(f"Orders: {b.live_orders}")
b.cancel_order(3)
print(f"Orders: {b.live_orders}")

b.publish()
'''
