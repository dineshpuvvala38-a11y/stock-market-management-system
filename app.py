
import streamlit as st
from datetime import datetime

st.set_page_config(
    page_title="Stock Market Management System",
    page_icon="📈",
    layout="wide"
)

st.title("📈 Stock Market Management System")
st.caption("C++ OOP Project | Simulated Stock & Mutual Fund Trading")

STOCKS = {
    "ALPH": {"name": "Alpha Technologies", "price": 184.50},
    "NOVA": {"name": "Nova Motors", "price": 92.75},
    "ORBT": {"name": "Orbit Energy", "price": 246.20}
}

FUNDS = {
    "EQF01": {"name": "Sample Equity Fund", "type": "Equity",
              "risk": "High", "nav": 50.00},
    "HYF02": {"name": "Sample Hybrid Fund", "type": "Hybrid",
              "risk": "Moderate", "nav": 32.40},
    "DBF03": {"name": "Sample Debt Fund", "type": "Debt",
              "risk": "Low", "nav": 25.00}
}

if "cash" not in st.session_state:
    st.session_state.cash = 10000.0
    st.session_state.stocks = {}
    st.session_state.funds = {}
    st.session_state.transactions = []
    st.session_state.txn_count = 0

def record(action, symbol, qty, amount):
    st.session_state.txn_count += 1
    st.session_state.transactions.append({
        "Transaction ID": f"TXN{st.session_state.txn_count:06d}",
        "Date": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "Action": action,
        "Symbol": symbol,
        "Quantity": round(qty, 4),
        "Amount (Rs)": round(amount, 2)
    })

def money(value):
    return f"₹{value:,.2f}"

menu = st.sidebar.selectbox(
    "Navigation",
    [
        "Dashboard",
        "Stocks",
        "Mutual Funds",
        "Buy Shares",
        "Sell Shares",
        "Invest in Fund",
        "Redeem Fund",
        "Portfolio",
        "Transaction History"
    ]
)

st.sidebar.metric("Available Cash", money(st.session_state.cash))

if menu == "Dashboard":
    st.subheader("Investor Dashboard")

    c1, c2, c3 = st.columns(3)
    c1.metric("Cash Balance", money(st.session_state.cash))
    c2.metric("Stock Holdings", len(st.session_state.stocks))
    c3.metric("Mutual Funds", len(st.session_state.funds))

    st.info("This application uses simulated prices and sample NAVs. No real trades are executed.")

    st.subheader("Available Stocks")
    st.dataframe([
        {"Symbol": s, "Company": d["name"], "Price": money(d["price"])}
        for s, d in STOCKS.items()
    ], use_container_width=True)

    st.subheader("Available Mutual Funds")
    st.dataframe([
        {"Symbol": s, "Fund": d["name"], "Type": d["type"],
         "Risk": d["risk"], "NAV": money(d["nav"])}
        for s, d in FUNDS.items()
    ], use_container_width=True)

elif menu == "Stocks":
    st.subheader("Stock Listing and Search")
    query = st.text_input("Search stock symbol or company name").lower()

    rows = []
    for symbol, data in STOCKS.items():
        if query in symbol.lower() or query in data["name"].lower():
            rows.append({
                "Symbol": symbol,
                "Company": data["name"],
                "Price": money(data["price"])
            })

    st.dataframe(rows, use_container_width=True)

elif menu == "Mutual Funds":
    st.subheader("Mutual Fund Listing and Search")
    query = st.text_input("Search fund symbol or name").lower()

    rows = []
    for symbol, data in FUNDS.items():
        if query in symbol.lower() or query in data["name"].lower():
            rows.append({
                "Symbol": symbol,
                "Fund": data["name"],
                "Category": data["type"],
                "Risk": data["risk"],
                "NAV": money(data["nav"])
            })

    st.dataframe(rows, use_container_width=True)

elif menu in ["Buy Shares", "Sell Shares"]:
    st.subheader(menu)

    symbol = st.selectbox("Select Stock", list(STOCKS.keys()))
    quantity = st.number_input("Number of shares", min_value=1, step=1)

    price = STOCKS[symbol]["price"]
    total = price * quantity

    st.write("Price per share:", money(price))
    st.write("Total amount:", money(total))

    if st.button("Confirm " + menu):
        holding = st.session_state.stocks.get(symbol, {
            "quantity": 0, "cost": 0.0
        })

        if menu == "Buy Shares":
            if total > st.session_state.cash:
                st.error("Insufficient cash balance.")
            else:
                old_cost = holding["cost"]
                old_qty = holding["quantity"]
                holding["quantity"] = old_qty + quantity
                holding["cost"] = old_cost + total
                st.session_state.stocks[symbol] = holding
                st.session_state.cash -= total
                record("Buy shares", symbol, quantity, total)
                st.success("Purchase completed successfully!")

        else:
            if quantity > holding["quantity"]:
                st.error("You do not have enough shares.")
            else:
                holding["quantity"] -= quantity
                holding["cost"] -= (
                    holding["cost"] / (holding["quantity"] + quantity)
                ) * quantity

                st.session_state.cash += total
                record("Sell shares", symbol, quantity, total)

                if holding["quantity"] == 0:
                    del st.session_state.stocks[symbol]
                else:
                    st.session_state.stocks[symbol] = holding

                st.success("Sale completed successfully!")

elif menu == "Invest in Fund":
    st.subheader("Invest in Mutual Fund")

    symbol = st.selectbox("Select Mutual Fund", list(FUNDS.keys()))
    amount = st.number_input(
        "Investment amount (Rs)",
        min_value=1.0,
        step=100.0
    )

    nav = FUNDS[symbol]["nav"]
    units = amount / nav

    st.write("NAV:", money(nav))
    st.write("Units to receive:", f"{units:.4f}")

    if st.button("Confirm Investment"):
        if amount > st.session_state.cash:
            st.error("Insufficient cash balance.")
        else:
            holding = st.session_state.funds.get(symbol, {
                "units": 0.0, "cost": 0.0
            })
            holding["units"] += units
            holding["cost"] += amount
            st.session_state.funds[symbol] = holding
            st.session_state.cash -= amount
            record("Invest in fund", symbol, units, amount)
            st.success("Investment completed successfully!")

elif menu == "Redeem Fund":
    st.subheader("Redeem Mutual Fund Units")

    if not st.session_state.funds:
        st.warning("No mutual fund units available.")
    else:
        symbol = st.selectbox(
            "Select Fund", list(st.session_state.funds.keys())
        )
        holding = st.session_state.funds[symbol]
        units = st.number_input(
            "Units to redeem",
            min_value=0.0001,
            max_value=float(holding["units"]),
            value=min(1.0, float(holding["units"]))
        )

        nav = FUNDS[symbol]["nav"]
        amount = units * nav

        st.write("Current NAV:", money(nav))
        st.write("Redemption amount:", money(amount))

        if st.button("Confirm Redemption"):
            average_cost = holding["cost"] / holding["units"]
            holding["units"] -= units
            holding["cost"] -= average_cost * units
            st.session_state.cash += amount

            record("Redeem fund units", symbol, units, amount)

            if holding["units"] < 0.000001:
                del st.session_state.funds[symbol]

            st.success("Redemption completed successfully!")

elif menu == "Portfolio":
    st.subheader("My Portfolio")
    st.metric("Cash Balance", money(st.session_state.cash))

    st.write("### Stock Holdings")
    stock_rows = []
    for symbol, holding in st.session_state.stocks.items():
        price = STOCKS[symbol]["price"]
        value = holding["quantity"] * price
        avg = holding["cost"] / holding["quantity"]
        stock_rows.append({
            "Symbol": symbol,
            "Shares": round(holding["quantity"], 4),
            "Average Cost": money(avg),
            "Current Value": money(value),
            "Unrealized P/L": money(value - holding["cost"])
        })

    if stock_rows:
        st.dataframe(stock_rows, use_container_width=True)
    else:
        st.info("No stock holdings.")

    st.write("### Mutual Fund Holdings")
    fund_rows = []
    for symbol, holding in st.session_state.funds.items():
        nav = FUNDS[symbol]["nav"]
        value = holding["units"] * nav
        avg = holding["cost"] / holding["units"]
        fund_rows.append({
            "Symbol": symbol,
            "Units": round(holding["units"], 4),
            "Average NAV": money(avg),
            "Current Value": money(value),
            "Unrealized P/L": money(value - holding["cost"])
        })

    if fund_rows:
        st.dataframe(fund_rows, use_container_width=True)
    else:
        st.info("No mutual fund holdings.")

elif menu == "Transaction History":
    st.subheader("Transaction History")

    if st.session_state.transactions:
        st.dataframe(
            st.session_state.transactions,
            use_container_width=True
        )
    else:
        st.info("No transactions available yet.")

st.divider()
st.caption("Educational Demo | Simulated Market Data | Not Financial Advice")

