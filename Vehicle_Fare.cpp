
#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace Util {

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return s;
}

string sanitize(string s) {
    replace(s.begin(), s.end(), '|', '/');
    replace(s.begin(), s.end(), '\n', ' ');
    replace(s.begin(), s.end(), '\r', ' ');
    return s;
}

string fix2(double v) {
    if (fabs(v) < 0.005) v = 0.0;
    ostringstream o;
    o << fixed << setprecision(2) << v;
    return o.str();
}

string money(double v) {
    if (fabs(v) < 0.005) v = 0.0;
    return string(v < 0 ? "-" : "") + "Rs." + fix2(fabs(v));
}

double round2(double v) { return round(v * 100.0) / 100.0; }

string pad(const string& s, size_t width) {
    string r = s;
    if (r.size() < width) r.append(width - r.size(), ' ');
    return r;
}

void row(const string& label, const string& value) {
    cout << "  " << pad(label, 24) << ": " << value << "\n";
}

string currentTime() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M:%S", localtime(&now));
    return string(buf);
}

int currentHour() {
    time_t now = time(nullptr);
    return localtime(&now)->tm_hour;
}

vector<string> split(const string& s, char delim) {
    vector<string> out;
    string item;
    istringstream ss(s);
    while (getline(ss, item, delim)) out.push_back(item);
    if (!s.empty() && s.back() == delim) out.push_back("");
    return out;
}

string csvEscape(const string& s) {
    string r = "\"";
    for (char c : s) {
        if (c == '"') r += "\"\"";
        else          r += c;
    }
    return r + "\"";
}

}

template <typename T>
T clampValue(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

class FareSystemException : public exception {
protected:
    string msg;
public:
    explicit FareSystemException(const string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class InvalidDistanceException : public FareSystemException {
public:
    explicit InvalidDistanceException(const string& m = "Distance cannot be negative!")
        : FareSystemException(m) {}
};

class InvalidInputException : public FareSystemException {
public:
    explicit InvalidInputException(const string& m) : FareSystemException(m) {}
};

class InvalidCouponException : public FareSystemException {
public:
    explicit InvalidCouponException(const string& m) : FareSystemException(m) {}
};

class InsufficientBalanceException : public FareSystemException {
public:
    explicit InsufficientBalanceException(const string& m) : FareSystemException(m) {}
};

class BookingNotFoundException : public FareSystemException {
public:
    explicit BookingNotFoundException(const string& m) : FareSystemException(m) {}
};

class InputClosedException : public exception {
public:
    const char* what() const noexcept override { return "Input stream closed."; }
};

namespace Input {

string line(const string& prompt) {
    cout << prompt << flush;
    string s;
    if (!getline(cin, s)) throw InputClosedException();
    return Util::trim(s);
}

int integer(const string& prompt) {
    while (true) {
        string s = line(prompt);
        istringstream iss(s);
        int v;
        if ((iss >> v) && (iss >> ws).eof()) return v;
        cout << "  Invalid input. Please enter a whole number.\n";
    }
}

int rangedInteger(const string& prompt, int lo, int hi) {
    while (true) {
        int v = integer(prompt);
        if (v >= lo && v <= hi) return v;
        cout << "  Please enter a value between " << lo << " and " << hi << ".\n";
    }
}

double real(const string& prompt) {
    while (true) {
        string s = line(prompt);
        istringstream iss(s);
        double v;
        if ((iss >> v) && (iss >> ws).eof() && std::isfinite(v)) return v;
        cout << "  Invalid input. Please enter a number.\n";
    }
}

bool yesNo(const string& prompt) {
    while (true) {
        string s = Util::toLower(line(prompt));
        if (s == "y" || s == "yes") return true;
        if (s == "n" || s == "no")  return false;
        cout << "  Please answer y or n.\n";
    }
}

}

struct Config {
    static bool verbose;
};
bool Config::verbose = true;

enum class PaymentMethod { Cash = 1, Card = 2, UPI = 3, Wallet = 4 };

string paymentToString(PaymentMethod p) {
    switch (p) {
        case PaymentMethod::Cash:   return "Cash";
        case PaymentMethod::Card:   return "Card";
        case PaymentMethod::UPI:    return "UPI";
        case PaymentMethod::Wallet: return "Wallet";
    }
    return "Cash";
}

PaymentMethod paymentFromString(const string& s) {
    if (s == "Card")   return PaymentMethod::Card;
    if (s == "UPI")    return PaymentMethod::UPI;
    if (s == "Wallet") return PaymentMethod::Wallet;
    return PaymentMethod::Cash;
}

struct TripRecord {
    int           bookingID;
    string        customerName;
    string        vehicleType;
    string        driverName;
    double        distance;
    double        baseFare;
    double        fare;
    double        refund;
    PaymentMethod payment;
    string        status;
    string        timestamp;
    int           rating;
    int           passengers;

    TripRecord()
        : bookingID(0), distance(0), baseFare(0), fare(0), refund(0),
          payment(PaymentMethod::Cash), status("Completed"), rating(0), passengers(1) {}

    bool operator<(const TripRecord& other) const { return fare < other.fare; }

    friend ostream& operator<<(ostream& os, const TripRecord& t);

    string toFileLine() const {
        ostringstream o;
        o << bookingID << '|' << customerName << '|' << vehicleType << '|' << driverName << '|'
          << Util::fix2(distance) << '|' << Util::fix2(baseFare) << '|' << Util::fix2(fare) << '|'
          << Util::fix2(refund) << '|' << paymentToString(payment) << '|' << status << '|'
          << timestamp << '|' << rating << '|' << passengers;
        return o.str();
    }

    static bool fromFileLine(const string& line, TripRecord& t) {
        vector<string> f = Util::split(line, '|');
        if (f.size() != 13) return false;
        try {
            t.bookingID    = stoi(f[0]);
            t.customerName = f[1];
            t.vehicleType  = f[2];
            t.driverName   = f[3];
            t.distance     = stod(f[4]);
            t.baseFare     = stod(f[5]);
            t.fare         = stod(f[6]);
            t.refund       = stod(f[7]);
            t.payment      = paymentFromString(f[8]);
            t.status       = f[9];
            t.timestamp    = f[10];
            t.rating       = stoi(f[11]);
            t.passengers   = stoi(f[12]);
        } catch (...) {
            return false;
        }
        return true;
    }
};

ostream& operator<<(ostream& os, const TripRecord& t) {
    os << "#" << t.bookingID << " | " << t.vehicleType << " | " << Util::fix2(t.distance)
       << " km | " << Util::money(t.fare);
    if (t.status != "Completed") os << " (refunded " << Util::money(t.refund) << ")";
    os << " | " << paymentToString(t.payment) << " | " << t.status << " | " << t.timestamp;
    if (t.rating > 0) os << " | " << t.rating << "/5";
    return os;
}

void printRecords(const vector<TripRecord>& recs) {
    for (const auto& r : recs) cout << "  " << r << "\n";
}

class TripLedger {
    static vector<TripRecord> trips;
    static int                nextID;
public:
    static const char* file() { return "trip_ledger.dat"; }

    static int issueID() { return nextID++; }

    static void add(const TripRecord& t) { trips.push_back(t); }

    static TripRecord* find(int id) {
        for (auto& t : trips)
            if (t.bookingID == id) return &t;
        return nullptr;
    }

    static const vector<TripRecord>& all() { return trips; }

    static int load() {
        ifstream f(file());
        if (!f.is_open()) return 0;
        string line;
        int loaded = 0;
        while (getline(f, line)) {
            line = Util::trim(line);
            if (line.empty()) continue;
            TripRecord t;
            if (TripRecord::fromFileLine(line, t)) {
                trips.push_back(t);
                nextID = max(nextID, t.bookingID + 1);
                ++loaded;
            }
        }
        return loaded;
    }

    static void save() {
        ofstream f(file());
        if (!f.is_open()) {
            cout << "  [WARNING] Could not write " << file() << "\n";
            return;
        }
        for (const auto& t : trips) f << t.toFileLine() << "\n";
    }
};
vector<TripRecord> TripLedger::trips;
int                TripLedger::nextID = 1001;

class WalletStore {
    static map<string, double> balances;
public:
    static const char* file() { return "wallets.dat"; }

    static double get(const string& name) {
        auto it = balances.find(Util::toLower(name));
        return it == balances.end() ? 0.0 : it->second;
    }

    static void set(const string& name, double value) {
        balances[Util::toLower(name)] = value;
        save();
    }

    static void load() {
        ifstream f(file());
        if (!f.is_open()) return;
        string line;
        while (getline(f, line)) {
            vector<string> p = Util::split(Util::trim(line), '|');
            if (p.size() != 2) continue;
            try { balances[p[0]] = stod(p[1]); } catch (...) {}
        }
    }

    static void save() {
        ofstream f(file());
        if (!f.is_open()) return;
        for (const auto& kv : balances) f << kv.first << '|' << Util::fix2(kv.second) << "\n";
    }
};
map<string, double> WalletStore::balances;

struct Coupon {
    double percent;
    int    maxUses;
    int    used;
    bool   active;
    double minFare;
    bool   firstRideOnly;

    Coupon(double p = 0.0, int mu = 0, double mf = 0.0, bool first = false)
        : percent(p), maxUses(mu), used(0), active(true), minFare(mf), firstRideOnly(first) {}
};

class CouponManager {
    static map<string, Coupon> coupons;
public:
    static void init() {
        coupons.clear();
        coupons["SAVE10"]  = Coupon(0.10, 1000, 100.0, false);
        coupons["FIRST50"] = Coupon(0.50, 1000, 0.0,   true);
        coupons["VIT25"]   = Coupon(0.25, 500,  150.0, false);
    }

    static void add(const string& code, const Coupon& c) { coupons[Util::toUpper(code)] = c; }

    static bool toggle(const string& code) {
        auto it = coupons.find(Util::toUpper(code));
        if (it == coupons.end()) return false;
        it->second.active = !it->second.active;
        return true;
    }

    static const Coupon& validate(const string& rawCode, double subtotal, bool isFirstRide) {
        string code = Util::toUpper(rawCode);
        auto it = coupons.find(code);
        if (it == coupons.end())
            throw InvalidCouponException("Invalid coupon code '" + code + "'. Coupon ignored.");
        const Coupon& c = it->second;
        if (!c.active)
            throw InvalidCouponException("Coupon " + code + " is currently disabled.");
        if (c.used >= c.maxUses)
            throw InvalidCouponException("Coupon " + code + " has reached its usage limit.");
        if (c.firstRideOnly && !isFirstRide)
            throw InvalidCouponException("Coupon " + code + " is valid only on your first ride.");
        if (subtotal < c.minFare)
            throw InvalidCouponException("Coupon " + code + " needs a minimum fare of " +
                                         Util::money(c.minFare) + ".");
        return c;
    }

    static void markUsed(const string& code) {
        auto it = coupons.find(Util::toUpper(code));
        if (it != coupons.end()) ++it->second.used;
    }

    static void list() {
        cout << "\n  ===== COUPONS =====\n";
        for (const auto& kv : coupons) {
            const Coupon& c = kv.second;
            cout << "  " << Util::pad(kv.first, 10) << Util::pad(to_string(int(round(c.percent * 100))) + "%", 6)
                 << " | used " << c.used << "/" << c.maxUses
                 << " | min fare " << Util::money(c.minFare)
                 << (c.firstRideOnly ? " | first ride only" : "")
                 << (c.active ? " | ACTIVE" : " | DISABLED") << "\n";
        }
    }
};
map<string, Coupon> CouponManager::coupons;

class Driver {
    string name;
    int    ratingSum;
    int    ratingCount;
public:
    explicit Driver(const string& n) : name(n), ratingSum(0), ratingCount(0) {}

    const string& getName() const { return name; }
    int    getRatingCount() const { return ratingCount; }
    void   addRating(int r)       { ratingSum += r; ++ratingCount; }
    double average() const {
        return ratingCount ? static_cast<double>(ratingSum) / ratingCount : 0.0;
    }
    string ratingText() const {
        if (ratingCount == 0) return "New";
        return Util::fix2(average()) + "/5 (" + to_string(ratingCount) + ")";
    }
};

class Vehicle {
protected:
    int           vehicleID;
    string        vehicleName;
    double        ratePerKm;
    double        minimumFare;
    int           maxPassengers;
    double        co2PerKm;
    vector<Driver> drivers;
    size_t        nextDriver;

    static int    totalVehicles;
    static double totalEarnings;

    static double maxDistance() { return 1000.0; }

    static void validateDistance(double d) {
        if (!std::isfinite(d) || d < 0)
            throw InvalidDistanceException("Distance cannot be negative!");
        if (d > maxDistance())
            throw InvalidDistanceException("Distance exceeds the maximum of 1000 km per booking!");
    }

    double flatThenPerKm(double dist, double freeKm) const {
        return dist <= freeKm ? minimumFare : minimumFare + (dist - freeKm) * ratePerKm;
    }

    void printExtras() const {
        cout << " | Seats: " << maxPassengers << " | CO2: " << Util::fix2(co2PerKm) << " g/km\n";
    }

public:
    Vehicle(int id, const string& name, double rate, double minFare, int maxPax,
            double co2, const vector<string>& driverNames)
        : vehicleID(id), vehicleName(name), ratePerKm(rate), minimumFare(minFare),
          maxPassengers(maxPax), co2PerKm(co2), nextDriver(0) {
        for (const auto& n : driverNames) drivers.emplace_back(n);
        ++totalVehicles;
        if (Config::verbose) cout << "  [CREATED] Vehicle object: " << vehicleName << "\n";
    }

    virtual ~Vehicle() {
        if (Config::verbose) cout << "  [DESTROYED] Vehicle object: " << vehicleName << "\n";
    }

    Vehicle(const Vehicle&)            = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    virtual double calculateFare(double distanceKm) const = 0;

    virtual void displayDetails() const {
        cout << "  [" << vehicleName << "] Rate: Rs." << Util::fix2(ratePerKm)
             << "/km | Min: Rs." << Util::fix2(minimumFare);
        printExtras();
    }

    Driver& assignDriver() {
        Driver& d = drivers[nextDriver];
        nextDriver = (nextDriver + 1) % drivers.size();
        return d;
    }

    Driver* findDriver(const string& name) {
        for (auto& d : drivers)
            if (d.getName() == name) return &d;
        return nullptr;
    }

    string getVehicleName() const { return vehicleName; }
    int    getVehicleID()   const { return vehicleID; }
    double getRatePerKm()   const { return ratePerKm; }
    double getMinimumFare() const { return minimumFare; }
    int    getMaxPassengers() const { return maxPassengers; }
    double getCo2PerKm()    const { return co2PerKm; }
    const vector<Driver>& getDrivers() const { return drivers; }

    static int    getTotalVehicles()      { return totalVehicles; }
    static double getTotalEarnings()      { return totalEarnings; }
    static void   addEarnings(double amt) { totalEarnings += amt; }

    friend void compareFares(const Vehicle& v1, const Vehicle& v2, double distanceKm);
};

int    Vehicle::totalVehicles = 0;
double Vehicle::totalEarnings = 0.0;

void compareFares(const Vehicle& v1, const Vehicle& v2, double distanceKm) {
    if (distanceKm < 0)
        throw InvalidDistanceException("Distance for comparison cannot be negative!");

    double fare1 = v1.calculateFare(distanceKm);
    double fare2 = v2.calculateFare(distanceKm);
    double co2a  = v1.co2PerKm * distanceKm;
    double co2b  = v2.co2PerKm * distanceKm;

    cout << "\n  ===== FARE COMPARISON  (" << Util::fix2(distanceKm) << " km) =====\n";
    cout << "  " << Util::pad(v1.vehicleName, 14) << ": " << Util::money(fare1)
         << "  | CO2 " << Util::fix2(co2a) << " g\n";
    cout << "  " << Util::pad(v2.vehicleName, 14) << ": " << Util::money(fare2)
         << "  | CO2 " << Util::fix2(co2b) << " g\n";
    if (fare1 < fare2)
        cout << "  >> " << v1.vehicleName << " is cheaper by " << Util::money(fare2 - fare1) << "\n";
    else if (fare2 < fare1)
        cout << "  >> " << v2.vehicleName << " is cheaper by " << Util::money(fare1 - fare2) << "\n";
    else
        cout << "  >> Both vehicles charge the same fare!\n";
    if (co2a < co2b)      cout << "  >> " << v1.vehicleName << " is greener.\n";
    else if (co2b < co2a) cout << "  >> " << v2.vehicleName << " is greener.\n";
    cout << "  ================================================\n";
}

class Car : public Vehicle {
public:
    Car() : Vehicle(1, "Car", 15.0, 80.0, 4, 120.0, {"Ramesh", "Suresh"}) {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        if (dist <= 5)  return minimumFare;
        if (dist <= 15) return minimumFare + (dist - 5) * ratePerKm;
        return minimumFare + 10 * ratePerKm + (dist - 15) * 12.0;
    }

    void displayDetails() const override {
        cout << "  [CAR]          Rate: Rs.15/km | Min: Rs.80 | Slabs: 0-5(min), 5-15(Rs.15), 15+(Rs.12)";
        printExtras();
    }
};

class Bus : public Vehicle {
    int seatingCapacity;
public:
    Bus() : Vehicle(2, "Bus", 5.0, 30.0, 40, 30.0, {"Mahesh", "Dinesh"}), seatingCapacity(40) {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        return flatThenPerKm(dist, 10);
    }

    void displayDetails() const override {
        cout << "  [BUS]          Rate: Rs.5/km  | Min: Rs.30 | Slabs: 0-10(min), 10+(Rs.5) | Capacity: "
             << seatingCapacity << " passengers";
        cout << " | CO2: " << Util::fix2(co2PerKm) << " g/km/passenger\n";
    }
};

class Bike : public Vehicle {
public:
    Bike() : Vehicle(3, "Bike", 8.0, 40.0, 1, 70.0, {"Arjun", "Karan"}) {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        return flatThenPerKm(dist, 3);
    }

    void displayDetails() const override {
        cout << "  [BIKE]         Rate: Rs.8/km  | Min: Rs.40 | Slabs: 0-3(min), 3+(Rs.8)";
        printExtras();
    }
};

class Auto : public Vehicle {
public:
    Auto() : Vehicle(4, "Auto", 10.0, 50.0, 3, 90.0, {"Raju", "Babu"}) {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        return flatThenPerKm(dist, 4);
    }

    void displayDetails() const override {
        cout << "  [AUTO]         Rate: Rs.10/km | Min: Rs.50 | Slabs: 0-4(min), 4+(Rs.10)";
        printExtras();
    }
};

class LuxuryCar : public Vehicle {
    string carModel;
public:
    LuxuryCar()
        : Vehicle(5, "Luxury Car", 25.0, 200.0, 4, 180.0, {"Vikram", "Aditya"}),
          carModel("Premium Sedan") {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        return minimumFare + dist * ratePerKm;
    }

    void displayDetails() const override {
        cout << "  [LUXURY CAR]   Rate: Rs.25/km | Min: Rs.200 (base) | Model: " << carModel;
        printExtras();
    }
};

class ElectricCar : public Vehicle {
public:
    ElectricCar() : Vehicle(6, "Electric Car", 12.0, 90.0, 4, 0.0, {"Neha", "Priya"}) {}

    double calculateFare(double dist) const override {
        validateDistance(dist);
        return flatThenPerKm(dist, 5);
    }

    void displayDetails() const override {
        cout << "  [ELECTRIC CAR] Rate: Rs.12/km | Min: Rs.90 | Slabs: 0-5(min), 5+(Rs.12) | Zero emission";
        printExtras();
    }
};

unique_ptr<Vehicle> createVehicle(int id) {
    switch (id) {
        case 1: return unique_ptr<Vehicle>(new Car());
        case 2: return unique_ptr<Vehicle>(new Bus());
        case 3: return unique_ptr<Vehicle>(new Bike());
        case 4: return unique_ptr<Vehicle>(new Auto());
        case 5: return unique_ptr<Vehicle>(new LuxuryCar());
        case 6: return unique_ptr<Vehicle>(new ElectricCar());
    }
    throw InvalidInputException("Unknown vehicle id!");
}

template <typename T>
class Statistics {
    vector<T> data;
public:
    void   add(T v)           { data.push_back(v); }
    bool   empty()      const { return data.empty(); }
    size_t count()      const { return data.size(); }
    T      sum()        const { return accumulate(data.begin(), data.end(), T()); }
    double mean()       const { return empty() ? 0.0 : static_cast<double>(sum()) / data.size(); }
    T      minimum()    const { return empty() ? T() : *min_element(data.begin(), data.end()); }
    T      maximum()    const { return empty() ? T() : *max_element(data.begin(), data.end()); }
    double stddev()     const {
        if (data.size() < 2) return 0.0;
        double m = mean(), acc = 0.0;
        for (const T& v : data) acc += (static_cast<double>(v) - m) * (static_cast<double>(v) - m);
        return sqrt(acc / data.size());
    }
};

class Customer {
    string      name;
    string      customerType;
    string      couponCode;
    vector<int> tripIDs;
    bool        announced;

    static int totalCustomers;

public:
    Customer(const string& n, const string& type = "regular", bool announce = true)
        : name(n), customerType(type), couponCode(""), announced(announce) {
        if (announced) {
            ++totalCustomers;
            if (Config::verbose)
                cout << "  [REGISTERED] Customer: " << name << " (" << customerType << ")\n";
        }
    }

    ~Customer() {
        if (announced && Config::verbose)
            cout << "  [SESSION ENDED] Customer: " << name << "\n";
    }

    string getName()   const { return name; }
    string getType()   const { return customerType; }
    string getCoupon() const { return couponCode; }
    void   setCoupon(const string& code) { couponCode = Util::toUpper(code); }
    void   clearCoupon()                 { couponCode.clear(); }

    void addTripID(int id) { tripIDs.push_back(id); }
    bool ownsTrip(int id) const { return find(tripIDs.begin(), tripIDs.end(), id) != tripIDs.end(); }

    void loadHistoryFromLedger() {
        for (const auto& t : TripLedger::all())
            if (Util::toLower(t.customerName) == Util::toLower(name) && !ownsTrip(t.bookingID))
                tripIDs.push_back(t.bookingID);
    }

    vector<TripRecord> historyRecords() const {
        vector<TripRecord> out;
        for (int id : tripIDs)
            if (const TripRecord* t = TripLedger::find(id)) out.push_back(*t);
        return out;
    }

    int completedTrips() const {
        int n = 0;
        for (const auto& t : historyRecords())
            if (t.status == "Completed") ++n;
        return n;
    }

    double totalSpent() const {
        double sum = 0.0;
        for (const auto& t : historyRecords()) sum += (t.fare - t.refund);
        return sum;
    }

    void showHistory() const {
        vector<TripRecord> recs = historyRecords();
        if (recs.empty()) { cout << "  No trips yet.\n"; return; }
        cout << "\n  ===== TRIP HISTORY: " << name << " =====\n";
        printRecords(recs);
    }

    string tier() const {
        double s = totalSpent();
        if (s >= 15000) return "Platinum";
        if (s >= 5000)  return "Gold";
        if (s >= 1000)  return "Silver";
        return "Bronze";
    }
    double tierDiscount() const {
        double s = totalSpent();
        if (s >= 15000) return 0.10;
        if (s >= 5000)  return 0.07;
        if (s >= 1000)  return 0.03;
        return 0.0;
    }
    int loyaltyPoints() const { return static_cast<int>(totalSpent() / 10.0); }

    double getWallet() const { return WalletStore::get(name); }

    void addFunds(double amt) {
        if (amt <= 0) throw InvalidInputException("Amount must be positive!");
        WalletStore::set(name, Util::round2(getWallet() + amt));
    }

    void deductFunds(double amt) {
        double bal = getWallet();
        if (bal + 1e-9 < amt)
            throw InsufficientBalanceException("Insufficient wallet balance! Required " +
                Util::money(amt) + ", available " + Util::money(bal) + ".");
        WalletStore::set(name, Util::round2(bal - amt));
    }

    Customer& operator+=(double amount) { addFunds(amount); return *this; }

    Customer operator+(const Customer& other) const {
        Customer merged("Merged[" + name + "+" + other.name + "]", "regular", false);
        merged.tripIDs = tripIDs;
        merged.tripIDs.insert(merged.tripIDs.end(), other.tripIDs.begin(), other.tripIDs.end());
        sort(merged.tripIDs.begin(), merged.tripIDs.end());
        merged.tripIDs.erase(unique(merged.tripIDs.begin(), merged.tripIDs.end()),
                             merged.tripIDs.end());
        return merged;
    }

    static int getTotalCustomers() { return totalCustomers; }
};
int Customer::totalCustomers = 0;

struct FareInputs {
    bool          night;
    bool          peak;
    int           waitMins;
    double        luggageKg;
    int           passengers;
    PaymentMethod payment;
    FareInputs()
        : night(false), peak(false), waitMins(0), luggageKg(0.0), passengers(1),
          payment(PaymentMethod::Cash) {}
};

struct FareBreakdown {
    double base, surged, timeAdjusted, waitingCharge, luggageCharge, subtotal;
    double discountPct, discountAmt, afterDiscount, gst, paymentFee, total, perPerson;
    string appliedCoupon;
    vector<string> notes;
    FareBreakdown()
        : base(0), surged(0), timeAdjusted(0), waitingCharge(0), luggageCharge(0), subtotal(0),
          discountPct(0), discountAmt(0), afterDiscount(0), gst(0), paymentFee(0), total(0),
          perPerson(0) {}
};

class FareCalculator {
    double surgeMultiplier;
public:
    FareCalculator() : surgeMultiplier(1.0) {}

    void setSurge(double m) {
        if (m < 0.5 || m > 5.0)
            throw InvalidInputException("Surge multiplier must be between 0.5 and 5.0!");
        surgeMultiplier = m;
        cout << "  [ADMIN] Surge pricing set to " << Util::fix2(m) << "x\n";
    }
    double getSurge() const { return surgeMultiplier; }

    FareBreakdown compute(double baseFare, const FareInputs& in, const Customer& cust) const {
        FareBreakdown b;
        b.base   = baseFare;
        b.surged = baseFare * surgeMultiplier;
        if (surgeMultiplier != 1.0)
            b.notes.push_back("Surge pricing " + Util::fix2(surgeMultiplier) + "x applied");

        double f = b.surged;
        if (in.night) { f *= 1.20; b.notes.push_back("Night charge +20% applied"); }
        if (in.peak)  { f *= 1.15; b.notes.push_back("Peak hour +15% applied");    }
        b.timeAdjusted  = f;
        b.waitingCharge = in.waitMins * 2.0;
        b.luggageCharge = in.luggageKg * 10.0;
        b.subtotal      = f + b.waitingCharge + b.luggageCharge;

        double pct = 0.0;
        const string type = cust.getType();
        if      (type == "student") { pct += 0.10; b.notes.push_back("Student discount 10%");      }
        else if (type == "senior")  { pct += 0.15; b.notes.push_back("Senior citizen discount 15%"); }
        else if (type == "member")  { pct += 0.20; b.notes.push_back("Membership discount 20%");   }

        double td = cust.tierDiscount();
        if (td > 0) {
            pct += td;
            b.notes.push_back("Loyalty tier " + cust.tier() + " discount " +
                              to_string(int(round(td * 100))) + "%");
        }

        const string code = cust.getCoupon();
        if (!code.empty()) {
            try {
                const Coupon& c = CouponManager::validate(code, b.subtotal, cust.completedTrips() == 0);
                pct += c.percent;
                b.appliedCoupon = code;
                b.notes.push_back("Coupon " + code + " +" + to_string(int(round(c.percent * 100))) + "%");
            } catch (const InvalidCouponException& e) {
                b.notes.push_back(string("WARNING: ") + e.what());
            }
        }
        if (pct > 0.50) { pct = 0.50; b.notes.push_back("Total discount capped at 50%"); }

        b.discountPct   = pct;
        b.discountAmt   = b.subtotal * pct;
        b.afterDiscount = b.subtotal - b.discountAmt;
        b.gst           = b.afterDiscount * 0.05;
        if (in.payment == PaymentMethod::Card)
            b.paymentFee = (b.afterDiscount + b.gst) * 0.02;

        b.total     = round(b.afterDiscount + b.gst + b.paymentFee);
        b.perPerson = Util::round2(b.total / clampValue(in.passengers, 1, 100));
        return b;
    }
};

class ReceiptGenerator {
public:
    static void printBreakdown(const FareBreakdown& b, const FareInputs& in) {
        cout << "\n  ===== FARE BREAKDOWN =====\n";
        Util::row("Base fare", Util::money(b.base));
        for (const auto& n : b.notes) cout << "  [" << n << "]\n";
        if (b.surged != b.base)      Util::row("After surge", Util::money(b.surged));
        if (b.timeAdjusted != b.surged) Util::row("After night/peak", Util::money(b.timeAdjusted));
        if (b.waitingCharge > 0)     Util::row("Waiting charge", Util::money(b.waitingCharge));
        if (b.luggageCharge > 0)     Util::row("Luggage charge", Util::money(b.luggageCharge));
        Util::row("Subtotal", Util::money(b.subtotal));
        if (b.discountAmt > 0)
            Util::row("Discount (" + to_string(int(round(b.discountPct * 100))) + "%)",
                      Util::money(-b.discountAmt));
        Util::row("GST (5%)", Util::money(b.gst));
        if (b.paymentFee > 0)        Util::row("Card fee (2%)", Util::money(b.paymentFee));
        Util::row("TOTAL (rounded)", Util::money(b.total));
        if (in.passengers > 1)
            Util::row("Per person (" + to_string(in.passengers) + ")", Util::money(b.perPerson));
    }

    static void print(const TripRecord& t, const FareBreakdown& b, const Vehicle& v) {
        cout << "\n  +----------------------------------------------+\n";
        cout << "  |        SMART VEHICLE FARE  -  RECEIPT        |\n";
        cout << "  +----------------------------------------------+\n";
        Util::row("Booking ID", "#" + to_string(t.bookingID));
        Util::row("Customer", t.customerName);
        Util::row("Vehicle", t.vehicleType);
        Util::row("Driver", t.driverName);
        Util::row("Distance", Util::fix2(t.distance) + " km");
        Util::row("Passengers", to_string(t.passengers));
        Util::row("Base fare", Util::money(t.baseFare));
        if (b.discountAmt > 0) Util::row("Discount", Util::money(-b.discountAmt));
        Util::row("GST (5%)", Util::money(b.gst));
        if (b.paymentFee > 0)  Util::row("Card fee", Util::money(b.paymentFee));
        Util::row("FINAL FARE", Util::money(t.fare));
        if (t.passengers > 1)  Util::row("Per person", Util::money(b.perPerson));
        Util::row("Payment", paymentToString(t.payment));
        Util::row("CO2 emitted", Util::fix2(v.getCo2PerKm() * t.distance) + " g");
        Util::row("Date & time", t.timestamp);
        cout << "  +----------------------------------------------+\n";
        cout << "  |     Thank you for riding with SmartFare!     |\n";
        cout << "  +----------------------------------------------+\n\n";
    }
};

class FileHandler {
public:

    static void appendLog(const TripRecord& t) {
        ofstream f("trip_history.txt", ios::app);
        if (!f.is_open()) return;
        f << "Booking #" << t.bookingID
          << " | Customer: " << t.customerName
          << " | Vehicle: " << t.vehicleType
          << " | Driver: " << t.driverName
          << " | Distance: " << Util::fix2(t.distance) << " km"
          << " | Fare: " << Util::money(t.fare)
          << " | Payment: " << paymentToString(t.payment)
          << " | Time: " << t.timestamp << "\n";
    }

    static void logEvent(const string& text) {
        ofstream f("trip_history.txt", ios::app);
        if (f.is_open()) f << "[" << Util::currentTime() << "] " << text << "\n";
    }

    static void showAllLog() {
        ifstream f("trip_history.txt");
        if (!f.is_open()) { cout << "  No saved trip log found.\n"; return; }
        cout << "\n  ===== TRIP LOG FILE =====\n";
        string line;
        while (getline(f, line)) cout << "  " << line << "\n";
    }

    static bool exportCSV(const string& filename) {
        ofstream f(filename.c_str());
        if (!f.is_open()) return false;
        f << "BookingID,Customer,Vehicle,Driver,DistanceKm,BaseFare,Fare,Refund,Payment,Status,Timestamp,Rating,Passengers\n";
        for (const auto& t : TripLedger::all()) {
            f << t.bookingID << ',' << Util::csvEscape(t.customerName) << ','
              << Util::csvEscape(t.vehicleType) << ',' << Util::csvEscape(t.driverName) << ','
              << Util::fix2(t.distance) << ',' << Util::fix2(t.baseFare) << ','
              << Util::fix2(t.fare) << ',' << Util::fix2(t.refund) << ','
              << paymentToString(t.payment) << ',' << t.status << ','
              << Util::csvEscape(t.timestamp) << ',' << t.rating << ',' << t.passengers << "\n";
        }
        return true;
    }
};

class AdminPanel {
    FareCalculator&                calc;
    vector<unique_ptr<Vehicle>>&   fleet;
    vector<unique_ptr<Customer>>&  customers;
    size_t                         passwordHash;

    static size_t hashOf(const string& s) { return hash<string>()(s); }

    void revenueReport() const {
        map<string, double> revenue;
        map<string, int>    completed;
        double lifetime = 0.0;
        for (const auto& t : TripLedger::all()) {
            double net = t.fare - t.refund;
            revenue[t.vehicleType] += net;
            lifetime += net;
            if (t.status == "Completed") ++completed[t.vehicleType];
        }
        cout << "\n  ===== REVENUE BY VEHICLE (lifetime) =====\n";
        if (revenue.empty()) { cout << "  No trips recorded yet.\n"; return; }
        for (const auto& kv : revenue)
            cout << "  " << Util::pad(kv.first, 14) << Util::pad(Util::money(kv.second), 14)
                 << " (" << completed[kv.first] << " completed trips)\n";
        cout << "  Lifetime net revenue : " << Util::money(lifetime) << "\n";

        map<string, double> byCustomer;
        for (const auto& t : TripLedger::all()) byCustomer[t.customerName] += (t.fare - t.refund);
        vector<pair<string, double>> top(byCustomer.begin(), byCustomer.end());
        sort(top.begin(), top.end(),
             [](const pair<string, double>& a, const pair<string, double>& b) {
                 return a.second > b.second;
             });
        cout << "\n  ----- Top customers -----\n";
        for (size_t i = 0; i < top.size() && i < 5; ++i)
            cout << "  " << (i + 1) << ". " << Util::pad(top[i].first, 20) << Util::money(top[i].second) << "\n";
    }

    void driverLeaderboard() const {
        struct Row { string vehicle, driver; double avg; int cnt; };
        vector<Row> rows;
        for (const auto& v : fleet)
            for (const auto& d : v->getDrivers())
                rows.push_back({v->getVehicleName(), d.getName(), d.average(), d.getRatingCount()});
        sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
            if (a.avg != b.avg) return a.avg > b.avg;
            return a.cnt > b.cnt;
        });
        cout << "\n  ===== DRIVER LEADERBOARD =====\n";
        for (size_t i = 0; i < rows.size(); ++i)
            cout << "  " << Util::pad(to_string(i + 1) + ".", 4) << Util::pad(rows[i].driver, 10)
                 << Util::pad(rows[i].vehicle, 14)
                 << (rows[i].cnt ? Util::fix2(rows[i].avg) + "/5 (" + to_string(rows[i].cnt) + " ratings)"
                                 : string("not rated yet")) << "\n";
    }

    void couponMenu() {
        CouponManager::list();
        cout << "\n  1. Add coupon\n  2. Enable/disable coupon\n  3. Back\n";
        int c = Input::rangedInteger("  Choice: ", 1, 3);
        if (c == 1) {
            string code = Util::toUpper(Input::line("  New coupon code: "));
            if (code.empty() || code.find('|') != string::npos)
                throw InvalidInputException("Coupon code cannot be empty or contain '|'!");
            int pct   = Input::rangedInteger("  Discount percent (1-50): ", 1, 50);
            int uses  = Input::rangedInteger("  Maximum uses (1-100000): ", 1, 100000);
            double mf = Input::real("  Minimum fare required (Rs.): ");
            if (mf < 0) throw InvalidInputException("Minimum fare cannot be negative!");
            bool first = Input::yesNo("  First ride only? (y/n): ");
            CouponManager::add(code, Coupon(pct / 100.0, uses, mf, first));
            cout << "  Coupon " << code << " created.\n";
        } else if (c == 2) {
            string code = Input::line("  Coupon code: ");
            if (CouponManager::toggle(code)) cout << "  Coupon status toggled.\n";
            else cout << "  Coupon not found.\n";
        }
    }

public:
    AdminPanel(FareCalculator& c, vector<unique_ptr<Vehicle>>& f,
               vector<unique_ptr<Customer>>& cu, const string& pwd = "admin123")
        : calc(c), fleet(f), customers(cu), passwordHash(hashOf(pwd)) {}

    void run() {
        bool ok = false;
        for (int attempt = 1; attempt <= 3 && !ok; ++attempt) {
            string pwd = Input::line("  Enter admin password: ");
            if (hashOf(pwd) == passwordHash) ok = true;
            else cout << "  Wrong password (" << attempt << "/3).\n";
        }
        if (!ok) { cout << "  Access denied!\n"; return; }

        int ch = 0;
        do {
            cout << "\n  ===== ADMIN PANEL =====\n"
                 << "  1.  Fleet & customer statistics\n"
                 << "  2.  View all trip records\n"
                 << "  3.  View trip log file\n"
                 << "  4.  Set surge multiplier\n"
                 << "  5.  Manage coupons\n"
                 << "  6.  Revenue report\n"
                 << "  7.  Driver leaderboard\n"
                 << "  8.  Export trips to CSV\n"
                 << "  9.  Toggle lifecycle logs (currently " << (Config::verbose ? "ON" : "OFF") << ")\n"
                 << "  10. Change admin password\n"
                 << "  11. Exit admin\n";
            ch = Input::integer("  Choice: ");
            try {
                switch (ch) {
                    case 1:
                        cout << "  Vehicles created        : " << Vehicle::getTotalVehicles() << "\n"
                             << "  Customers registered    : " << Customer::getTotalCustomers() << "\n"
                             << "  Session earnings        : " << Util::money(Vehicle::getTotalEarnings()) << "\n"
                             << "  Saved bookings (ledger) : " << TripLedger::all().size() << "\n"
                             << "  Current surge           : " << Util::fix2(calc.getSurge()) << "x\n";
                        break;
                    case 2: {
                        if (TripLedger::all().empty()) { cout << "  No trips recorded yet.\n"; break; }
                        cout << "\n  ===== ALL BOOKINGS =====\n";
                        for (const auto& t : TripLedger::all())
                            cout << "  " << t.customerName << " | " << t << "\n";
                        break;
                    }
                    case 3: FileHandler::showAllLog(); break;
                    case 4: calc.setSurge(Input::real("  Enter surge multiplier (0.5 - 5.0): ")); break;
                    case 5: couponMenu(); break;
                    case 6: revenueReport(); break;
                    case 7: driverLeaderboard(); break;
                    case 8:
                        if (FileHandler::exportCSV("trips.csv"))
                            cout << "  Exported " << TripLedger::all().size() << " bookings to trips.csv\n";
                        else
                            cout << "  Could not write trips.csv\n";
                        break;
                    case 9:
                        Config::verbose = !Config::verbose;
                        cout << "  Lifecycle logs " << (Config::verbose ? "enabled" : "disabled") << ".\n";
                        break;
                    case 10: {
                        string p1 = Input::line("  New password (min 6 chars): ");
                        if (p1.size() < 6) throw InvalidInputException("Password is too short!");
                        string p2 = Input::line("  Confirm password: ");
                        if (p1 != p2) throw InvalidInputException("Passwords do not match!");
                        passwordHash = hashOf(p1);
                        cout << "  Password changed.\n";
                        break;
                    }
                    case 11: cout << "  Exiting admin panel.\n"; break;
                    default: cout << "  Invalid choice.\n";
                }
            } catch (const FareSystemException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            }
        } while (ch != 11);
        (void)customers;
    }
};

class SmartFareApp {
    vector<unique_ptr<Vehicle>>  fleet;
    vector<unique_ptr<Customer>> customers;
    FareCalculator               calc;
    AdminPanel                   admin;
    Customer*                    current;

    const Vehicle* findVehicle(const string& name) const {
        for (const auto& v : fleet)
            if (v->getVehicleName() == name) return v.get();
        return nullptr;
    }

    void requireCustomer() const {
        if (!current) throw InvalidInputException("Please register / select a customer first (option 1)!");
    }

    int pickVehicle(const string& prompt) const {
        int n = Input::integer(prompt);
        if (n < 1 || n > static_cast<int>(fleet.size()))
            throw InvalidInputException("Vehicle number must be between 1 and " +
                                        to_string(fleet.size()) + "!");
        return n - 1;
    }

    void offerCoupon() {
        string coupon = Input::line("  Coupon code (leave blank to skip): ");
        if (!coupon.empty()) {
            current->setCoupon(coupon);
            cout << "  Coupon saved. It will be validated at booking time.\n";
        }
    }

    void registerCustomer() {
        string name = Util::sanitize(Input::line("  Enter name: "));
        if (name.empty()) throw InvalidInputException("Name cannot be empty!");
        if (name.size() > 30) name.resize(30);

        for (auto& c : customers) {
            if (Util::toLower(c->getName()) == Util::toLower(name)) {
                current = c.get();
                cout << "  Welcome back, " << current->getName() << "! (switched to existing customer)\n";
                offerCoupon();
                return;
            }
        }

        string type = Util::toLower(Input::line("  Customer type [regular / student / senior / member]: "));
        if (type != "student" && type != "senior" && type != "member") {
            if (!type.empty() && type != "regular")
                cout << "  Unknown type, using 'regular'.\n";
            type = "regular";
        }
        customers.push_back(unique_ptr<Customer>(new Customer(name, type)));
        current = customers.back().get();
        current->loadHistoryFromLedger();
        if (current->completedTrips() > 0)
            cout << "  Found " << current->completedTrips() << " previous trip(s) - tier: "
                 << current->tier() << ", wallet " << Util::money(current->getWallet()) << "\n";
        offerCoupon();
    }

    PaymentMethod pickPayment() const {
        cout << "  Payment method:  1. Cash   2. Card (+2% fee)   3. UPI   4. Wallet (balance "
             << Util::money(current->getWallet()) << ")\n";
        return static_cast<PaymentMethod>(Input::rangedInteger("  Choose (1-4): ", 1, 4));
    }

    void bookRide() {
        requireCustomer();
        viewVehicles();

        int vi = pickVehicle("  Select vehicle (1-" + to_string(fleet.size()) + "): ");
        Vehicle& v = *fleet[vi];

        double dist = Input::real("  Enter distance (km): ");
        double base = v.calculateFare(dist);

        FareInputs in;
        in.passengers = Input::rangedInteger("  Passengers sharing the ride (1-" +
                            to_string(v.getMaxPassengers()) + "): ", 1, v.getMaxPassengers());

        if (Input::yesNo("  Auto-detect night/peak hour from current time? (y/n): ")) {
            int h = Util::currentHour();
            in.night = (h >= 22 || h < 6);
            in.peak  = ((h >= 8 && h < 10) || (h >= 17 && h < 20));
            cout << "  Detected time " << h << ":00 -> night: " << (in.night ? "yes" : "no")
                 << ", peak: " << (in.peak ? "yes" : "no") << "\n";
        } else {
            in.night = Input::yesNo("  Night time ride? (y/n): ");
            in.peak  = Input::yesNo("  Peak hour?       (y/n): ");
        }

        in.waitMins = Input::integer("  Waiting minutes (0 if none): ");
        if (in.waitMins < 0 || in.waitMins > 120)
            throw InvalidInputException("Waiting time must be between 0 and 120 minutes!");
        in.luggageKg = Input::real("  Luggage kg (0 if none): ");
        if (in.luggageKg < 0 || in.luggageKg > 100)
            throw InvalidInputException("Luggage weight must be between 0 and 100 kg!");
        in.payment = pickPayment();

        FareBreakdown fb = calc.compute(base, in, *current);
        ReceiptGenerator::printBreakdown(fb, in);

        if (in.payment == PaymentMethod::Wallet && current->getWallet() + 1e-9 < fb.total)
            throw InsufficientBalanceException("Insufficient wallet balance! Required " +
                Util::money(fb.total) + ", available " + Util::money(current->getWallet()) +
                ". Top up from the Wallet menu.");

        if (!Input::yesNo("\n  Confirm booking? (y/n): ")) {
            cout << "  Booking abandoned. Nothing was charged.\n";
            return;
        }

        Driver& drv = v.assignDriver();
        if (in.payment == PaymentMethod::Wallet) current->deductFunds(fb.total);
        if (!fb.appliedCoupon.empty()) {
            CouponManager::markUsed(fb.appliedCoupon);
            current->clearCoupon();
        }

        TripRecord tr;
        tr.bookingID    = TripLedger::issueID();
        tr.customerName = current->getName();
        tr.vehicleType  = v.getVehicleName();
        tr.driverName   = drv.getName();
        tr.distance     = dist;
        tr.baseFare     = base;
        tr.fare         = fb.total;
        tr.payment      = in.payment;
        tr.timestamp    = Util::currentTime();
        tr.passengers   = in.passengers;

        TripLedger::add(tr);
        current->addTripID(tr.bookingID);
        FileHandler::appendLog(tr);
        TripLedger::save();
        Vehicle::addEarnings(fb.total);

        ReceiptGenerator::print(tr, fb, v);
        cout << "  Your driver: " << drv.getName() << "  (rating " << drv.ratingText() << ")\n";

        int r = Input::rangedInteger("  Rate your driver 1-5 (0 to skip): ", 0, 5);
        if (r > 0) {
            drv.addRating(r);
            TripLedger::find(tr.bookingID)->rating = r;
            TripLedger::save();
            cout << "  Thanks for rating " << drv.getName() << "!\n";
        }
        cout << "  Loyalty tier: " << current->tier() << " | points: " << current->loyaltyPoints() << "\n";
    }

    void viewVehicles() const {
        cout << "\n  ===== AVAILABLE VEHICLES =====\n";
        for (size_t i = 0; i < fleet.size(); ++i) {
            cout << "  " << (i + 1) << ". ";
            fleet[i]->displayDetails();
        }
    }

    void fareEstimate() const {
        double dist = Input::real("  Distance for estimate (km): ");
        struct Est { string name; double base; double surged; };
        vector<Est> est;
        for (const auto& v : fleet) {
            double b = v->calculateFare(dist);
            est.push_back({v->getVehicleName(), b, b * calc.getSurge()});
        }
        sort(est.begin(), est.end(), [](const Est& a, const Est& b) { return a.base < b.base; });
        cout << "\n  ===== FARE ESTIMATE  (" << Util::fix2(dist) << " km, cheapest first) =====\n";
        for (size_t i = 0; i < est.size(); ++i) {
            cout << "  " << (i + 1) << ". " << Util::pad(est[i].name, 14) << Util::pad(Util::money(est[i].base), 12);
            if (calc.getSurge() != 1.0) cout << " (with surge " << Util::money(est[i].surged) << ")";
            if (i == 0) cout << "  <- cheapest";
            cout << "\n";
        }
        cout << "  Estimates exclude GST, extras, discounts and payment fees.\n";
    }

    void tripHistory() {
        requireCustomer();
        vector<TripRecord> recs = current->historyRecords();
        if (recs.empty()) { cout << "  No trips yet.\n"; return; }

        cout << "  Sort by:  1. Newest first  2. Oldest first  3. Highest fare  4. Vehicle  5. Distance\n";
        int s = Input::rangedInteger("  Choice (1-5): ", 1, 5);
        switch (s) {
            case 1: sort(recs.begin(), recs.end(),
                         [](const TripRecord& a, const TripRecord& b) { return a.bookingID > b.bookingID; }); break;
            case 2: sort(recs.begin(), recs.end(),
                         [](const TripRecord& a, const TripRecord& b) { return a.bookingID < b.bookingID; }); break;
            case 3: sort(recs.begin(), recs.end(),
                         [](const TripRecord& a, const TripRecord& b) { return b < a; }); break;
            case 4: sort(recs.begin(), recs.end(),
                         [](const TripRecord& a, const TripRecord& b) {
                             if (a.vehicleType != b.vehicleType) return a.vehicleType < b.vehicleType;
                             return a.bookingID < b.bookingID;
                         }); break;
            case 5: sort(recs.begin(), recs.end(),
                         [](const TripRecord& a, const TripRecord& b) { return a.distance > b.distance; }); break;
        }
        cout << "\n  ===== TRIP HISTORY: " << current->getName() << " =====\n";
        printRecords(recs);
        cout << "  Total spent    : " << Util::money(current->totalSpent()) << "\n";
        cout << "  Loyalty tier   : " << current->tier() << " (" << current->loyaltyPoints() << " points)\n";
    }

    void cancelTrip() {
        requireCustomer();
        vector<TripRecord> eligible;
        for (const auto& t : current->historyRecords())
            if (t.status == "Completed") eligible.push_back(t);
        if (eligible.empty()) { cout << "  You have no bookings that can be cancelled.\n"; return; }

        cout << "\n  ===== CANCELLABLE BOOKINGS =====\n";
        printRecords(eligible);
        int id = Input::integer("  Enter booking ID to cancel: ");

        TripRecord* t = TripLedger::find(id);
        if (!t || !current->ownsTrip(id) || t->status != "Completed")
            throw BookingNotFoundException("No cancellable booking with ID " + to_string(id) + " found for you.");

        double fee    = min(t->fare, max(20.0, t->fare * 0.10));
        double refund = Util::round2(t->fare - fee);
        cout << "  Cancellation fee : " << Util::money(fee) << "\n";
        cout << "  Refund amount    : " << Util::money(refund) << "\n";
        if (!Input::yesNo("  Cancel this booking? (y/n): ")) { cout << "  Cancellation aborted.\n"; return; }

        t->status = "Cancelled";
        t->refund = refund;
        if (t->payment == PaymentMethod::Wallet && refund > 0) {
            current->addFunds(refund);
            cout << "  Refund credited to your wallet. New balance: " << Util::money(current->getWallet()) << "\n";
        } else {
            cout << "  Refund of " << Util::money(refund) << " will be returned via " << paymentToString(t->payment) << ".\n";
        }
        Vehicle::addEarnings(-refund);
        TripLedger::save();
        FileHandler::logEvent("Booking #" + to_string(id) + " cancelled by " + current->getName() +
                              " | refund " + Util::money(refund));
    }

    void walletMenu() {
        requireCustomer();
        cout << "\n  ===== WALLET: " << current->getName() << " =====\n";
        cout << "  Balance: " << Util::money(current->getWallet()) << "\n";
        cout << "  1. Add money   2. Back\n";
        int c = Input::rangedInteger("  Choice (1-2): ", 1, 2);
        if (c == 1) {
            double amt = Input::real("  Amount to add (Rs.10 - Rs.10000): ");
            if (amt < 10 || amt > 10000)
                throw InvalidInputException("Top-up must be between Rs.10 and Rs.10000!");
            *current += Util::round2(amt);
            cout << "  Added. New balance: " << Util::money(current->getWallet()) << "\n";
        }
    }

    void compareTwo() const {
        double dist = Input::real("  Distance for comparison (km): ");
        int a = pickVehicle("  First vehicle  (1-" + to_string(fleet.size()) + "): ");
        int b = pickVehicle("  Second vehicle (1-" + to_string(fleet.size()) + "): ");
        compareFares(*fleet[a], *fleet[b], dist);
    }

    void filterTrips() {
        requireCustomer();
        cout << "  Filter my trips by:  1. Fare above amount  2. Vehicle  3. Payment method  4. Rated 4+ stars\n";
        int ch = Input::rangedInteger("  Choice (1-4): ", 1, 4);

        function<bool(const TripRecord&)> pred;
        string title;
        if (ch == 1) {
            double th = Input::real("  Show trips with fare > Rs.: ");
            pred  = [th](const TripRecord& t) { return t.fare > th; };
            title = "FARE > " + Util::money(th);
        } else if (ch == 2) {
            int vi = pickVehicle("  Vehicle number (1-" + to_string(fleet.size()) + "): ");
            string name = fleet[vi]->getVehicleName();
            pred  = [name](const TripRecord& t) { return t.vehicleType == name; };
            title = "VEHICLE = " + name;
        } else if (ch == 3) {
            cout << "  1. Cash  2. Card  3. UPI  4. Wallet\n";
            PaymentMethod pm = static_cast<PaymentMethod>(Input::rangedInteger("  Choice (1-4): ", 1, 4));
            pred  = [pm](const TripRecord& t) { return t.payment == pm; };
            title = "PAYMENT = " + paymentToString(pm);
        } else {
            pred  = [](const TripRecord& t) { return t.rating >= 4; };
            title = "RATED 4+ STARS";
        }

        vector<TripRecord> mine = current->historyRecords(), out;
        copy_if(mine.begin(), mine.end(), back_inserter(out), pred);

        cout << "\n  ===== TRIPS: " << title << " =====\n";
        if (out.empty()) cout << "  No matching trips.\n";
        else             printRecords(out);
    }

    void analytics() {
        requireCustomer();
        Statistics<double> fares, dists;
        map<string, int>   usage;
        double co2 = 0.0;
        int cancelled = 0;
        for (const auto& t : current->historyRecords()) {
            if (t.status == "Completed") {
                fares.add(t.fare);
                dists.add(t.distance);
                ++usage[t.vehicleType];
                if (const Vehicle* v = findVehicle(t.vehicleType)) co2 += v->getCo2PerKm() * t.distance;
            } else {
                ++cancelled;
            }
        }
        cout << "\n  ===== ANALYTICS: " << current->getName() << " =====\n";
        if (fares.empty()) { cout << "  No completed trips to analyse yet.\n"; return; }
        Util::row("Completed trips", to_string(fares.count()));
        Util::row("Cancelled trips", to_string(cancelled));
        Util::row("Total fare paid", Util::money(fares.sum()));
        Util::row("Average fare", Util::money(fares.mean()));
        Util::row("Cheapest / costliest", Util::money(fares.minimum()) + " / " + Util::money(fares.maximum()));
        Util::row("Fare std deviation", Util::money(fares.stddev()));
        Util::row("Total distance", Util::fix2(dists.sum()) + " km");
        Util::row("Average distance", Util::fix2(dists.mean()) + " km");
        auto fav = max_element(usage.begin(), usage.end(),
                               [](const pair<const string, int>& a, const pair<const string, int>& b) {
                                   return a.second < b.second;
                               });
        Util::row("Favourite vehicle", fav->first + " (" + to_string(fav->second) + " trips)");
        Util::row("CO2 footprint", Util::fix2(co2 / 1000.0) + " kg");
        Util::row("Loyalty tier", current->tier() + " (" + to_string(current->loyaltyPoints()) + " points)");
        Util::row("Wallet balance", Util::money(current->getWallet()));
    }

    void mergeCustomers() {
        if (customers.size() < 2) { cout << "  Need at least 2 registered customers.\n"; return; }
        cout << "  Registered customers:\n";
        for (size_t i = 0; i < customers.size(); ++i)
            cout << "  " << (i + 1) << ". " << customers[i]->getName() << "\n";
        int n  = static_cast<int>(customers.size());
        int c1 = Input::rangedInteger("  Select first customer:  ", 1, n);
        int c2 = Input::rangedInteger("  Select second customer: ", 1, n);
        if (c1 == c2) throw InvalidInputException("Please select two different customers!");

        Customer merged = *customers[c1 - 1] + *customers[c2 - 1];
        merged.showHistory();
        cout << "  Combined total spent: " << Util::money(merged.totalSpent()) << "\n";
    }

    void printMenu() const {
        cout << "\n  ======================================\n";
        cout << "  MAIN MENU" << (current ? "   (user: " + current->getName() + ")" : string("")) << "\n";
        cout << "  1.  Register / Switch Customer\n";
        cout << "  2.  Book a Ride\n";
        cout << "  3.  Fare Estimate (all vehicles)\n";
        cout << "  4.  View Available Vehicles\n";
        cout << "  5.  My Trip History\n";
        cout << "  6.  Cancel a Ride\n";
        cout << "  7.  Wallet\n";
        cout << "  8.  Compare Two Vehicle Fares (Friend Function)\n";
        cout << "  9.  Filter My Trips (Lambda)\n";
        cout << "  10. My Analytics (Class Template)\n";
        cout << "  11. Merge Two Customers (Operator +)\n";
        cout << "  12. Admin Panel\n";
        cout << "  13. Exit\n";
        cout << "  ======================================\n";
    }

public:
    SmartFareApp() : admin(calc, fleet, customers), current(nullptr) {
        cout << "\n  ==============================================\n";
        cout << "     SMART VEHICLE FARE SYSTEM  -  VIT 2026\n";
        cout << "  ==============================================\n\n";

        CouponManager::init();
        int loaded = TripLedger::load();
        WalletStore::load();
        if (loaded > 0) cout << "  Loaded " << loaded << " saved booking(s) from " << TripLedger::file() << "\n";

        cout << "  Initialising vehicles...\n";
        for (int id = 1; id <= 6; ++id) fleet.push_back(createVehicle(id));
        cout << "\n";

        for (const auto& t : TripLedger::all()) {
            if (t.rating <= 0) continue;
            for (auto& v : fleet)
                if (v->getVehicleName() == t.vehicleType)
                    if (Driver* d = v->findDriver(t.driverName)) d->addRating(t.rating);
        }
    }

    void run() {
        bool running = true;
        while (running) {
            printMenu();
            try {
                int choice = Input::integer("  Choice: ");
                switch (choice) {
                    case 1:  registerCustomer(); break;
                    case 2:  bookRide();         break;
                    case 3:  fareEstimate();     break;
                    case 4:  viewVehicles();     break;
                    case 5:  tripHistory();      break;
                    case 6:  cancelTrip();       break;
                    case 7:  walletMenu();       break;
                    case 8:  compareTwo();       break;
                    case 9:  filterTrips();      break;
                    case 10: analytics();        break;
                    case 11: mergeCustomers();   break;
                    case 12: admin.run();        break;
                    case 13:
                        cout << "  Goodbye! Thank you for using SmartFare.\n";
                        running = false;
                        break;
                    default: cout << "  Invalid choice. Please enter 1-13.\n";
                }
            } catch (const InputClosedException&) {
                cout << "\n  Input closed - saving and exiting.\n";
                running = false;
            } catch (const FareSystemException& e) {
                cout << "  [EXCEPTION] " << e.what() << "\n";
            } catch (const exception& e) {
                cout << "  [ERROR] " << e.what() << "\n";
            }
        }
        shutdown();
    }

    void shutdown() {
        TripLedger::save();
        WalletStore::save();

        cout << "\n  ===== SESSION SUMMARY =====\n";
        cout << "  Total Vehicle Types     : " << Vehicle::getTotalVehicles() << "\n";
        cout << "  Customers this session  : " << Customer::getTotalCustomers() << "\n";
        cout << "  Session Earnings (Rs.)  : " << Util::fix2(Vehicle::getTotalEarnings()) << "\n";
        cout << "  Bookings on record      : " << TripLedger::all().size() << "\n";
        cout << "  ===========================\n\n";
        cout << "  Trip log   : trip_history.txt\n";
        cout << "  Saved data : " << TripLedger::file() << ", " << WalletStore::file() << "\n\n";

        current = nullptr;
        cout << "  Releasing vehicle objects...\n";
        fleet.clear();
        cout << "  Releasing customer objects...\n";
        customers.clear();
    }
};

int main() {
    SmartFareApp app;
    app.run();
    return 0;
}
⋮⋮⋮⋮⋮⋮⋮⋮⋮⋮⋮⋮
