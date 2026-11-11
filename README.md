# Stock Market Management System

An educational C++17 console project for simulated stock and mutual-fund investing.
Prices and NAVs are sample values; this project does not connect to a live market
or execute real trades.

## Current implementation stage

The instrument module defines the shared `Instrument` abstraction, plus `Stock`
and `MutualFund` types. The portfolio module tracks share and unit holdings,
weighted average purchase cost, and current value. The user and trading modules
manage simulated cash, trades, and redemptions. The catalogue provides sample
stocks and funds with case-insensitive lookup. File storage saves and restores a
demo user's account, holdings, and transaction history. `main.cpp` provides the
menu-driven application workflow.

## Run in VS Code on Windows

### 1. Install the tools

- Install **Visual Studio Code**.
- Install a C++ compiler. Choose either **Visual Studio Build Tools** (MSVC) or
  **MinGW-w64** (GCC). Make sure the compiler is available in the terminal.
- Install **CMake** and make sure `cmake` is available in the terminal.
- In VS Code, install the Microsoft **C/C++** extension. The **CMake Tools**
  extension is optional; the commands below work in the integrated terminal.

### 2. Open the project

Extract the ZIP, then in VS Code choose **File → Open Folder** and select the
extracted `stock_market_project` folder (the folder containing `CMakeLists.txt`).
Open **Terminal → New Terminal**.

### 3. Configure and build

Run these commands in the VS Code terminal:

```powershell
cmake -S . -B build
cmake --build build
```

If CMake cannot find a compiler, install or configure MSVC/MinGW and restart VS
Code so its terminal sees the compiler on `PATH`.

### 4. Start the application

For the usual single-configuration MinGW build:

```powershell
.\build\stock_market.exe
```

For a Visual Studio (MSVC) build:

```powershell
.\build\Debug\stock_market.exe
```

If you selected a different CMake configuration, run the `stock_market.exe`
inside the corresponding configuration folder under `build`.

The program starts with a demo investor balance of Rs 10,000.00. Quotes and
mutual-fund NAVs are simulated sample values. Choose menu option `0` to save and
exit. The account file `stock_market_data.txt` is created in the program's
current working directory and loaded on the next run.

## Build from a terminal without VS Code

The same CMake configure/build commands work in PowerShell, Command Prompt, or a
compatible terminal, provided CMake and a C++17 compiler are installed.

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Instrument.cpp src/Portfolio.cpp src/User.cpp src/TradingService.cpp src/MarketCatalog.cpp src/FileStorage.cpp src/main.cpp -o stock_market
```

