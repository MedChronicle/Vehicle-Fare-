# 🚗 SmartFare — Smart Vehicle Fare Management System

A feature-rich **C++ console-based vehicle booking and fare management system** that calculates ride fares, compares vehicle prices, manages customer wallets, applies discounts, tracks trips, and generates revenue analytics. Built using object-oriented programming, STL, file handling, templates, lambda expressions, and exception handling.

## 📑 Table of Contents

1. [Overview](#-overview)
2. [Key Features](#-key-features)
3. [Vehicle Fleet](#-vehicle-fleet)
4. [Fare Calculation](#-fare-calculation)
5. [Technology Stack](#-technology-stack)
6. [OOP Concepts](#-object-oriented-programming-concepts)
7. [Project Structure](#-project-structure)
8. [Requirements](#-requirements)
9. [Installation and Execution](#-installation-and-execution)
10. [Application Workflow](#-application-workflow)
11. [Main Menu](#-main-menu)
12. [Data Storage and Export](#-data-storage-and-export)
13. [Sample Usage](#-sample-usage)
14. [Error Handling](#-error-handling)
15. [Limitations](#-limitations)
16. [Future Enhancements](#-future-enhancements)
17. [Contributing](#-contributing)
18. [License](#-license)

---

## 📌 Overview

SmartFare simulates a smart transportation booking platform through an interactive terminal interface. Customers can register, explore vehicles, estimate fares, book rides, manage wallet balances, apply coupons, and review their travel history.

The system calculates fares using vehicle-specific pricing rules and adjustments such as surge pricing, night charges, peak-hour charges, waiting time, luggage, discounts, GST, and card payment fees.

An integrated administration panel provides booking records, revenue reports, driver ratings, coupon management, surge pricing controls, and CSV exports.

### Project Objectives

- Automate vehicle fare calculations.
- Provide transparent fare breakdowns before booking.
- Support multiple vehicle categories and payment methods.
- Maintain customer profiles, trip history, and wallet balances.
- Demonstrate advanced C++ programming concepts.
- Generate customer and administrative analytics.

## ✨ Key Features

### 1. Customer Management
- Register customers and switch between existing customers.
- Support regular, student, senior citizen, and member categories.
- Associate saved bookings with matching customer names.
- Maintain trip history and calculate loyalty tiers.
- Combine customer trip histories.

### 2. Smart Fare Estimation
- Estimate fares across six vehicle categories.
- Sort vehicle estimates from cheapest to most expensive.
- Compare fares between two vehicles.
- Display fare differences and estimated CO₂ emissions.

### 3. Ride Booking
- Select a vehicle and enter travel distance.
- Specify passenger count, waiting time, and luggage weight.
- Apply night and peak-hour pricing.
- Select a payment method.
- Generate booking IDs and detailed receipts.
- Rate drivers after completed bookings.

### 4. Dynamic Pricing
- Configure a surge multiplier between 0.5× and 5.0×.
- Apply a 20% night surcharge.
- Apply a 15% peak-hour surcharge.
- Detect night and peak hours using the system clock.

### 5. Coupons and Discounts

Built-in coupon codes:

| Coupon | Discount | Condition |
|---|---:|---|
| `SAVE10` | 10% | Minimum fare of ₹100 |
| `FIRST50` | 50% | First ride only |
| `VIT25` | 25% | Minimum fare of ₹150 |

Additional coupon functionality:
- Create custom coupons.
- Configure usage limits and minimum fares.
- Enable or disable coupons.
- Apply eligible customer and loyalty discounts.
- Limit the combined discount to 50%.

### 6. Digital Wallet
- View customer wallet balances.
- Add funds.
- Pay for bookings using wallet funds.
- Validate available balance before booking confirmation.
- Process wallet refunds for eligible cancellations.

### 7. Cancellation and Refunds
- Cancel eligible bookings.
- Calculate cancellation fees and refunds.
- Update booking status and refund records.
- Record events in the trip log.

### 8. Customer Analytics
- Calculate total and average fare.
- Identify the cheapest and most expensive trips.
- Calculate average travel distance and standard deviation.
- Identify frequently used vehicles.
- Estimate trip-related CO₂ emissions.
- Display loyalty tiers, points, and wallet balance.

### 9. Administration Panel
- View fleet and customer statistics.
- Inspect booking records and trip logs.
- Manage coupons and surge pricing.
- Generate revenue reports by vehicle category.
- View top customers and driver rankings.
- Change the admin password during the session.
- Export booking records to CSV.
- Toggle object lifecycle messages.

## 🚘 Vehicle Fleet

| Vehicle | Minimum Fare | Pricing Rule | Capacity |
|---|---:|---|---:|
| Car | ₹80 | First 5 km included; ₹15/km up to 15 km, then ₹12/km | 4 |
| Bus | ₹30 | First 10 km included; ₹5/km thereafter | 40 |
| Bike | ₹40 | First 3 km included; ₹8/km thereafter | 1 |
| Auto | ₹50 | First 4 km included; ₹10/km thereafter | 3 |
| Luxury Car | ₹200 | ₹200 base + ₹25/km | 4 |
| Electric Car | ₹90 | First 5 km included; ₹12/km thereafter | 4 |

These are the default base-fare rules defined in the source code. Final booking fares may include additional charges, discounts, taxes, and payment fees.

## 💰 Fare Calculation

The fare calculation engine follows these steps:

1. Calculate the base fare using the vehicle's pricing rule.
2. Apply the surge multiplier.
3. Apply night and peak-hour surcharges.
4. Add waiting charges at ₹2 per minute.
5. Add luggage charges at ₹10 per kilogram.
6. Apply eligible customer-category and loyalty discounts.
7. Apply a valid coupon, subject to the 50% total discount cap.
8. Calculate GST at 5% on the discounted fare.
9. Add a 2% fee for card payments.
10. Round the final booking amount to a whole rupee.

**Formula overview**

\[
\text{Subtotal} =
\text{Time-Adjusted Fare}+
\text{Waiting Charge}+
\text{Luggage Charge}
\]

\[
\text{Discounted Fare} =
\text{Subtotal}\times(1-\text{Discount Rate})
\]

\[
\text{Final Fare} =
\text{Discounted Fare}+
\text{GST}+
\text{Card Fee}
\]

The per-person amount is calculated by dividing the final fare by the passenger count.

## 🛠 Technology Stack

| Technology | Purpose |
|---|---|
| C++ | Core application logic |
| Standard Template Library (STL) | Containers, algorithms, and utilities |
| Object-Oriented Programming | Modeling customers, vehicles, and bookings |
| File Handling | Local persistence of bookings, wallets, and logs |
| CSV | Booking data export |
| Exception Handling | Error detection and recovery |
| Lambda Expressions | Custom sorting and comparison |
| Smart Pointers | Object lifetime management |

## 🧠 Object-Oriented Programming Concepts

SmartFare demonstrates the following C++ concepts:

- **Inheritance:** Specialized vehicle classes implement different fare calculations.
- **Polymorphism:** Virtual functions provide vehicle-specific behavior.
- **Encapsulation:** Classes manage their own data and operations.
- **Abstraction:** Common vehicle behavior is exposed through a shared interface.
- **Operator Overloading:** Customer objects support wallet top-ups and combined trip histories.
- **Friend Functions:** Vehicle fare comparison accesses the required vehicle details.
- **Templates:** Generic statistics calculations.
- **Lambda Expressions:** Custom sorting and comparison operations.
- **STL Containers:** Vectors and maps store records, customers, wallets, and coupons.
- **Smart Pointers:** `unique_ptr` manages vehicle and customer object lifetimes.
- **Exception Handling:** Custom exceptions manage invalid input, distance, coupons, and wallet balances.

## 📂 Project Structure

The current implementation is contained in a single C++ source file. The following structure is recommended for repository organization:

```text
SmartFare/
├── README.md
├── LICENSE
├── .gitignore
├── src/
│   └── main.cpp
├── data/
│   ├── trip_ledger.dat
│   └── wallets.dat
├── logs/
│   └── trip_history.txt
├── exports/
│   └── trips.csv
├── docs/
│   ├── architecture.md
│   ├── fare-calculation.md
│   └── screenshots/
└── tests/
    ├── fare-tests.md
    └── test-cases.csv
```

**Note:** This is a recommended structure. The application currently writes runtime data files to its working directory and does not automatically create all the directories listed above.

## ⚙️ Requirements

- C++ compiler supporting C++11 or later.
- Terminal or command-line interface.
- macOS, Linux, or Windows with a compatible compiler.
- Write permissions in the working directory.

Recommended compilers include Apple Clang, GCC, and MinGW-w64.

## 🚀 Installation and Execution

### Step 1: Clone the Repository

```bash
git clone https://github.com/YOUR-USERNAME/SmartFare.git
cd SmartFare
```

Replace `YOUR-USERNAME/SmartFare` with your actual GitHub repository URL.

### Step 2: Compile the Program

If your source file is named `main.cpp`:

```bash
g++ -std=c++11 -Wall -Wextra main.cpp -o smartfare
```

### Step 3: Run the Application

On macOS or Linux:

```bash
./smartfare
```

On Windows with GCC:

```bash
g++ -std=c++11 -Wall -Wextra main.cpp -o smartfare.exe
smartfare.exe
```

### First Run

The application initializes its vehicle fleet and built-in coupons. If saved booking or wallet files do not exist, the application starts without previous records and creates the relevant files when save operations run.

## 🔄 Application Workflow

```text
Start Application
       |
       v
Load Saved Bookings and Wallets
       |
       v
Initialize Vehicles and Coupons
       |
       v
Register / Select Customer
       |
       v
Estimate Fare or Book a Ride
       |
       v
Calculate Charges and Discounts
       |
       v
Display Fare Breakdown
       |
       v
Confirm Booking
       |
       v
Assign Driver and Save Booking
       |
       v
Generate Receipt and Collect Rating
       |
       v
Update History and Analytics
       |
       v
Save Data and Exit
```

## 📋 Main Menu

The main menu provides access to the application's customer-facing features.

| Option | Function |
|---:|---|
| 1 | Register or switch customer |
| 2 | Book a ride |
| 3 | Estimate fares across vehicles |
| 4 | View available vehicles |
| 5 | View and sort trip history |
| 6 | Cancel a ride |
| 7 | Manage wallet |
| 8 | Compare vehicle fares |
| 9 | Filter personal trip history |
| 10 | View customer analytics |
| 11 | Merge customer histories |
| 12 | Open admin panel |
| 13 | Exit the application |

The admin panel provides additional functionality for pricing, coupons, reports, booking records, and CSV exports.

## 💾 Data Storage and Export

SmartFare uses local files instead of a database server.

| File | Purpose |
|---|---|
| `trip_ledger.dat` | Pipe-delimited booking records |
| `wallets.dat` | Customer wallet balances |
| `trip_history.txt` | Readable trip logs and event messages |
| `trips.csv` | Exported booking data |

### CSV Export

From the admin panel, select **Export trips to CSV**. The program generates `trips.csv`, which can be opened in Microsoft Excel, Google Sheets, or other compatible applications.

The booking ledger includes booking ID, customer, vehicle, driver, distance, base fare, final fare, refund, payment method, status, timestamp, rating, and passenger count.

**Important:** Back up your local records before modifying or deleting data files.

## 🧪 Sample Usage

A typical user session follows this sequence:

1. Launch SmartFare.
2. Register a customer, such as `Aarav`.
3. Select a customer category, such as `student`.
4. Enter a coupon code or skip the coupon.
5. Estimate fares across the available vehicles.
6. Select a vehicle and enter the travel distance.
7. Enter passenger count, waiting time, and luggage weight.
8. Select a payment method.
9. Review the fare breakdown and confirm the booking.
10. View the receipt, rate the driver, and inspect trip history.

The final fare depends on the selected vehicle, pricing settings, applicable discounts, and payment method.

## 🛡 Error Handling

SmartFare uses custom exceptions to handle common errors:

- `InvalidDistanceException` — invalid or excessive travel distance.
- `InvalidInputException` — invalid menu choices or booking parameters.
- `InvalidCouponException` — invalid or ineligible coupon codes.
- `InsufficientBalanceException` — insufficient wallet funds.
- `BookingNotFoundException` — missing or ineligible booking records.
- `InputClosedException` — closed terminal input stream.

Input validation includes distance, passenger capacity, waiting time, luggage weight, coupon configuration, and wallet top-up amounts.

## ⚠️ Limitations

- The application is a console-based simulation, not a live ride-hailing service.
- It does not connect to GPS, maps, real payment gateways, or live driver tracking.
- Customer identification relies on names rather than authenticated accounts.
- The admin password uses `std::hash`, which is unsuitable for secure password storage.
- Wallets and trip records are stored in local files without database transaction guarantees.
- Custom coupon definitions and usage counters are not persisted across restarts.
- Driver ratings are maintained in memory during the session.
- Surge pricing is manually configured.
- CO₂ emissions are estimated using fixed values rather than live measurements.
- The application does not provide multi-user concurrency control.

## 🔮 Future Enhancements

### Phase 1 — Core Improvements
- Refactor the source into modular header and implementation files.
- Add unit tests for fare calculations, discounts, refunds, and edge cases.
- Improve validation and duplicate-record handling.
- Persist coupon definitions and driver ratings.
- Add structured logging.

### Phase 2 — Database Integration
- Integrate SQLite or PostgreSQL.
- Create tables for customers, vehicles, drivers, bookings, payments, coupons, and ratings.
- Introduce database transactions for booking confirmation, wallet deductions, and refunds.
- Add indexing, constraints, and backups.

### Phase 3 — Security and Reliability
- Use a secure password-hashing algorithm.
- Add authenticated customer and administrator accounts.
- Introduce role-based access control and audit logs.
- Protect sensitive records and improve recovery procedures.

### Phase 4 — Smart Transportation
- Integrate a maps API for route distance and estimated travel time.
- Add driver availability and dispatch simulation.
- Implement demand-based surge pricing.
- Introduce estimated arrival times and trip status updates.
- Improve vehicle-specific emissions estimates.
- Support configurable currency and multiple languages.

### Phase 5 — User Interface and Analytics
- Build a web interface using React and a backend API.
- Create customer and administrator dashboards.
- Visualize revenue, bookings, vehicle usage, driver ratings, and emissions.
- Generate PDF receipts and reports.
- Add automated testing and CI/CD.

## 🤝 Contributing

Contributions, bug reports, and feature suggestions are welcome.

1. Fork the repository.
2. Create a feature branch.
3. Implement and test your changes.
4. Commit your work with a descriptive commit message.
5. Open a pull request explaining the changes.

For major changes, open an issue first to discuss the proposed implementation.

## 📄 License

This project is licensed under the **MIT License**.

You are free to use, copy, modify, merge, publish, distribute, sublicense, and sell copies of this software, subject to the terms and conditions of the MIT License.

See the [`LICENSE`](LICENSE) file for the complete license text.

The MIT License requires the copyright notice and permission notice to be included in copies or substantial portions of the software.

## 👨‍💻 Project Information

- **Project:** SmartFare — Smart Vehicle Fare Management System
- **Language:** C++
- **Category:** Object-Oriented Programming, Transportation, Fare Analytics
- **License:** MIT

---

⭐ If you find this project useful for learning C++ and object-oriented programming, consider starring the repository.
