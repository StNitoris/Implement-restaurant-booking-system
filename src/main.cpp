#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace utils {
struct Date {
    int year{0};
    int month{0};
    int day{0};

    bool operator<(const Date &other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator==(const Date &other) const {
        return year == other.year && month == other.month && day == other.day;
    }
};

inline std::string toString(const Date &date) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << date.year << "-" << std::setw(2) << date.month << "-" << std::setw(2)
        << date.day;
    return oss.str();
}

inline Date today() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&tt);
    return {local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday};
}
}  // namespace utils

enum class TableStatus { Free, Reserved, Occupied, OutOfService };

enum class ReservationStatus { Open, Booked, Seated, Completed, Cancelled };

enum class OrderStatus { Open, Served, Completed, Cancelled };

static std::string toString(TableStatus status) {
    switch (status) {
        case TableStatus::Free:
            return "Free";
        case TableStatus::Reserved:
            return "Reserved";
        case TableStatus::Occupied:
            return "Occupied";
        case TableStatus::OutOfService:
            return "OutOfService";
    }
    return "Unknown";
}

static std::string toString(ReservationStatus status) {
    switch (status) {
        case ReservationStatus::Open:
            return "Open";
        case ReservationStatus::Booked:
            return "Booked";
        case ReservationStatus::Seated:
            return "Seated";
        case ReservationStatus::Completed:
            return "Completed";
        case ReservationStatus::Cancelled:
            return "Cancelled";
    }
    return "Unknown";
}

[[maybe_unused]] static std::string toString(OrderStatus status) {
    switch (status) {
        case OrderStatus::Open:
            return "Open";
        case OrderStatus::Served:
            return "Served";
        case OrderStatus::Completed:
            return "Completed";
        case OrderStatus::Cancelled:
            return "Cancelled";
    }
    return "Unknown";
}

class Permission {
   public:
    Permission() = default;
    explicit Permission(std::string name) : name_(std::move(name)) {}

    const std::string &name() const { return name_; }

   private:
    std::string name_;
};

class Role {
   public:
    Role() = default;
    explicit Role(std::string name) : name_(std::move(name)) {}

    void addPermission(const Permission &permission) { permissions_.insert(permission.name()); }

    bool hasPermission(const std::string &permission) const { return permissions_.count(permission) > 0; }

    const std::string &name() const { return name_; }

   private:
    std::string name_;
    std::set<std::string> permissions_;
};

class Customer {
   public:
    Customer() = default;
    Customer(std::string name, std::string contact) : name_(std::move(name)), contact_(std::move(contact)) {}

    const std::string &name() const { return name_; }
    const std::string &contact() const { return contact_; }

   private:
    std::string name_;
    std::string contact_;
};

class Table {
   public:
    Table(int id, int capacity) : id_(id), capacity_(capacity) {}

    int id() const { return id_; }
    int capacity() const { return capacity_; }
    TableStatus status() const { return status_; }

    void updateStatus(TableStatus status) { status_ = status; }

   private:
    int id_;
    int capacity_;
    TableStatus status_{TableStatus::Free};
};

class MenuItem {
   public:
    MenuItem() = default;
    MenuItem(std::string name, double price) : name_(std::move(name)), price_(price) {}

    const std::string &name() const { return name_; }
    double price() const { return price_; }

   private:
    std::string name_;
    double price_{0.0};
};

class OrderItem {
   public:
    OrderItem(MenuItem item, int qty) : item_(std::move(item)), quantity_(qty) {}

    const MenuItem &item() const { return item_; }
    int quantity() const { return quantity_; }

   private:
    MenuItem item_;
    int quantity_{0};
};

class Order {
   public:
    Order() = default;
    Order(int id, int tableId) : id_(id), tableId_(tableId) {}

    void addItem(const MenuItem &item, int quantity) { items_.emplace_back(item, quantity); }

    double total() const {
        double sum = 0.0;
        for (const auto &entry : items_) {
            sum += entry.item().price() * entry.quantity();
        }
        return sum;
    }

    int id() const { return id_; }
    int tableId() const { return tableId_; }
    OrderStatus status() const { return status_; }

    void updateStatus(OrderStatus status) { status_ = status; }

   private:
    int id_{0};
    int tableId_{0};
    std::vector<OrderItem> items_;
    OrderStatus status_{OrderStatus::Open};
};

class Reservation {
   public:
    Reservation() = default;
    Reservation(int id, Customer customer, utils::Date date, std::string timeSlot, int partySize,
                std::string note)
        : id_(id),
          customer_(std::move(customer)),
          date_(date),
          timeSlot_(std::move(timeSlot)),
          partySize_(partySize),
          note_(std::move(note)) {}

    int id() const { return id_; }
    const Customer &customer() const { return customer_; }
    const utils::Date &date() const { return date_; }
    const std::string &timeSlot() const { return timeSlot_; }
    int partySize() const { return partySize_; }
    ReservationStatus status() const { return status_; }
    std::optional<int> tableId() const { return tableId_; }

    void assignTable(int tableId) { tableId_ = tableId; }

    void updateStatus(ReservationStatus status) { status_ = status; }

    const std::string &note() const { return note_; }

   private:
    int id_{0};
    Customer customer_;
    utils::Date date_;
    std::string timeSlot_;
    int partySize_{0};
    ReservationStatus status_{ReservationStatus::Open};
    std::optional<int> tableId_;
    std::string note_;
};

class BookingSheet {
   public:
    Reservation &createReservation(const Customer &customer, const utils::Date &date, std::string timeSlot,
                                   int partySize, std::string note) {
        int id = ++nextReservationId_;
        reservations_.emplace(id, Reservation{id, customer, date, std::move(timeSlot), partySize, std::move(note)});
        return reservations_.at(id);
    }

    std::vector<std::reference_wrapper<const Reservation>> reservationsForDate(const utils::Date &date) const {
        std::vector<std::reference_wrapper<const Reservation>> result;
        for (const auto &pair : reservations_) {
            if (pair.second.date() == date) {
                result.emplace_back(std::cref(pair.second));
            }
        }
        std::sort(result.begin(), result.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.get().timeSlot() < rhs.get().timeSlot();
        });
        return result;
    }

    Reservation *findReservation(int id) {
        auto it = reservations_.find(id);
        if (it == reservations_.end()) return nullptr;
        return &it->second;
    }

    const Reservation *findReservation(int id) const {
        auto it = reservations_.find(id);
        if (it == reservations_.end()) return nullptr;
        return &it->second;
    }

    template <typename Predicate>
    std::vector<std::reference_wrapper<const Reservation>> filter(Predicate predicate) const {
        std::vector<std::reference_wrapper<const Reservation>> result;
        for (const auto &pair : reservations_) {
            if (predicate(pair.second)) {
                result.emplace_back(std::cref(pair.second));
            }
        }
        return result;
    }

    void cancelReservation(int id) {
        auto *reservation = findReservation(id);
        if (reservation) {
            reservation->updateStatus(ReservationStatus::Cancelled);
        }
    }

    bool isTableAvailable(int tableId, const utils::Date &date, const std::string &timeSlot, int excludeReservation) const {
        for (const auto &entry : reservations_) {
            const Reservation &reservation = entry.second;
            if (reservation.id() == excludeReservation) continue;
            if (reservation.tableId() && reservation.tableId().value() == tableId && reservation.date() == date &&
                reservation.timeSlot() == timeSlot && reservation.status() != ReservationStatus::Cancelled) {
                return false;
            }
        }
        return true;
    }

    std::map<ReservationStatus, int> statusSummary() const {
        std::map<ReservationStatus, int> summary;
        for (const auto &pair : reservations_) {
            summary[pair.second.status()]++;
        }
        return summary;
    }

   private:
    std::unordered_map<int, Reservation> reservations_;
    int nextReservationId_{0};
};

class Restaurant {
   public:
    explicit Restaurant(std::string name) : name_(std::move(name)) {}

    Table &addTable(int capacity) {
        int id = static_cast<int>(tables_.size()) + 1;
        tables_.emplace_back(id, capacity);
        return tables_.back();
    }

    const std::vector<Table> &tables() const { return tables_; }

    Table *findTable(int id) {
        auto it = std::find_if(tables_.begin(), tables_.end(), [id](const Table &table) { return table.id() == id; });
        if (it == tables_.end()) return nullptr;
        return &(*it);
    }

    BookingSheet &bookingSheet() { return bookingSheet_; }
    const BookingSheet &bookingSheet() const { return bookingSheet_; }

    std::vector<const Table *> availableTables(const utils::Date &date, const std::string &timeSlot, int partySize,
                                               int excludeReservationId = -1) const {
        std::vector<const Table *> result;
        for (const auto &table : tables_) {
            if (table.status() == TableStatus::OutOfService) continue;
            if (table.capacity() < partySize) continue;
            if (!bookingSheet_.isTableAvailable(table.id(), date, timeSlot, excludeReservationId)) continue;
            result.push_back(&table);
        }
        return result;
    }

    Reservation &createReservation(const Customer &customer, const utils::Date &date, const std::string &timeSlot,
                                   int partySize, const std::string &note) {
        auto &reservation = bookingSheet_.createReservation(customer, date, timeSlot, partySize, note);
        reservation.updateStatus(ReservationStatus::Booked);
        return reservation;
    }

    bool assignTableToReservation(int reservationId, int tableId) {
        Reservation *reservation = bookingSheet_.findReservation(reservationId);
        if (!reservation) {
            std::cout << "Reservation not found.\n";
            return false;
        }
        Table *table = findTable(tableId);
        if (!table) {
            std::cout << "Table not found.\n";
            return false;
        }
        if (!bookingSheet_.isTableAvailable(tableId, reservation->date(), reservation->timeSlot(), reservationId)) {
            std::cout << "Table is not available for the selected slot.\n";
            return false;
        }
        reservation->assignTable(tableId);
        table->updateStatus(TableStatus::Reserved);
        return true;
    }

    void updateReservationStatus(int reservationId, ReservationStatus status) {
        Reservation *reservation = bookingSheet_.findReservation(reservationId);
        if (!reservation) return;
        reservation->updateStatus(status);
        if (reservation->tableId()) {
            Table *table = findTable(reservation->tableId().value());
            if (!table) return;
            if (status == ReservationStatus::Cancelled || status == ReservationStatus::Completed) {
                table->updateStatus(TableStatus::Free);
            } else if (status == ReservationStatus::Seated) {
                table->updateStatus(TableStatus::Occupied);
            }
        }
    }

    void displaySummary(const utils::Date &date) const {
        auto reservations = bookingSheet_.reservationsForDate(date);
        std::cout << "Reservations for " << utils::toString(date) << "\n";
        if (reservations.empty()) {
            std::cout << "  No reservations found.\n";
        }
        for (const auto &reservationRef : reservations) {
            const Reservation &reservation = reservationRef.get();
            std::cout << "  [#" << reservation.id() << "] " << reservation.customer().name() << " (" << reservation.timeSlot()
                      << ") - Party of " << reservation.partySize();
            if (reservation.tableId()) {
                std::cout << ", Table " << reservation.tableId().value();
            }
            std::cout << ", Status: " << toString(reservation.status());
            if (!reservation.note().empty()) {
                std::cout << ", Note: " << reservation.note();
            }
            std::cout << "\n";
        }
    }

   private:
    std::string name_;
    std::vector<Table> tables_;
    BookingSheet bookingSheet_;
};

class Report {
   public:
    void displayReservationSummary(const Restaurant &restaurant) const {
        auto summary = restaurant.bookingSheet().statusSummary();
        std::cout << "Reservation summary:\n";
        for (const auto &entry : summary) {
            std::cout << "  " << toString(entry.first) << ": " << entry.second << "\n";
        }
    }
};

class ApplicationContext {
   public:
    ApplicationContext() {
        Role frontDesk("FrontDesk");
        frontDesk.addPermission(Permission("create_reservation"));
        frontDesk.addPermission(Permission("update_reservation"));

        Role manager("Manager");
        manager.addPermission(Permission("manage_tables"));
        manager.addPermission(Permission("generate_reports"));

        roles_.emplace(frontDesk.name(), frontDesk);
        roles_.emplace(manager.name(), manager);
    }

    bool hasPermission(const std::string &roleName, const std::string &permission) const {
        auto it = roles_.find(roleName);
        if (it == roles_.end()) return false;
        return it->second.hasPermission(permission);
    }

   private:
    std::map<std::string, Role> roles_;
};

static void printMenu() {
    std::cout << "\n=== Restaurant Reservation System ===\n";
    std::cout << "1. Add table\n";
    std::cout << "2. List tables\n";
    std::cout << "3. Create reservation\n";
    std::cout << "4. Check availability\n";
    std::cout << "5. Assign table to reservation\n";
    std::cout << "6. Update reservation status\n";
    std::cout << "7. Show reservation summary for a date\n";
    std::cout << "8. Generate reservation report\n";
    std::cout << "9. Exit\n";
    std::cout << "Select option: ";
}

static utils::Date readDate() {
    utils::Date date;
    std::cout << "Enter date (YYYY MM DD): ";
    std::cin >> date.year >> date.month >> date.day;
    return date;
}

static std::string readTimeSlot() {
    std::string timeSlot;
    std::cout << "Enter time slot (e.g., 18:30): ";
    std::cin >> timeSlot;
    return timeSlot;
}

static ReservationStatus selectReservationStatus() {
    std::cout << "Select status: 1-Open 2-Booked 3-Seated 4-Completed 5-Cancelled: ";
    int option;
    std::cin >> option;
    switch (option) {
        case 1:
            return ReservationStatus::Open;
        case 2:
            return ReservationStatus::Booked;
        case 3:
            return ReservationStatus::Seated;
        case 4:
            return ReservationStatus::Completed;
        case 5:
            return ReservationStatus::Cancelled;
        default:
            return ReservationStatus::Open;
    }
}

int main() {
    Restaurant restaurant("Demo Restaurant");
    ApplicationContext context;
    Report report;

    bool running = true;
    while (running) {
        printMenu();
        int option;
        if (!(std::cin >> option)) {
            std::cout << "Invalid input. Exiting...\n";
            break;
        }
        switch (option) {
            case 1: {
                if (!context.hasPermission("Manager", "manage_tables")) {
                    std::cout << "Permission denied.\n";
                    break;
                }
                int capacity;
                std::cout << "Enter table capacity: ";
                std::cin >> capacity;
                Table &table = restaurant.addTable(capacity);
                std::cout << "Added table " << table.id() << " with capacity " << capacity << "\n";
                break;
            }
            case 2: {
                const auto &tables = restaurant.tables();
                std::cout << "Tables:\n";
                if (tables.empty()) {
                    std::cout << "  No tables defined.\n";
                    break;
                }
                for (const auto &table : tables) {
                    std::cout << "  Table " << table.id() << " (capacity: " << table.capacity()
                              << ") status: " << toString(table.status()) << "\n";
                }
                break;
            }
            case 3: {
                if (!context.hasPermission("FrontDesk", "create_reservation")) {
                    std::cout << "Permission denied.\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::string name;
                std::cout << "Customer name: ";
                std::getline(std::cin, name);
                std::string contact;
                std::cout << "Contact info: ";
                std::getline(std::cin, contact);
                auto date = readDate();
                auto timeSlot = readTimeSlot();
                int partySize;
                std::cout << "Party size: ";
                std::cin >> partySize;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::string note;
                std::cout << "Reservation note (optional): ";
                std::getline(std::cin, note);

                Customer customer(name, contact);
                Reservation &reservation = restaurant.createReservation(customer, date, timeSlot, partySize, note);
                std::cout << "Reservation created with id " << reservation.id() << "\n";

                auto available = restaurant.availableTables(date, timeSlot, partySize);
                if (!available.empty()) {
                    std::cout << "Available tables for this reservation: ";
                    for (const auto *table : available) {
                        std::cout << table->id() << " ";
                    }
                    std::cout << "\n";
                } else {
                    std::cout << "No matching tables available right now.\n";
                }
                break;
            }
            case 4: {
                auto date = readDate();
                auto timeSlot = readTimeSlot();
                int partySize;
                std::cout << "Party size: ";
                std::cin >> partySize;
                auto available = restaurant.availableTables(date, timeSlot, partySize);
                if (available.empty()) {
                    std::cout << "No tables available for this slot.\n";
                } else {
                    std::cout << "Available tables: ";
                    for (const auto *table : available) {
                        std::cout << table->id() << " (capacity: " << table->capacity() << ") ";
                    }
                    std::cout << "\n";
                }
                break;
            }
            case 5: {
                int reservationId;
                std::cout << "Reservation id: ";
                std::cin >> reservationId;
                int tableId;
                std::cout << "Table id: ";
                std::cin >> tableId;
                restaurant.assignTableToReservation(reservationId, tableId);
                break;
            }
            case 6: {
                int reservationId;
                std::cout << "Reservation id: ";
                std::cin >> reservationId;
                ReservationStatus status = selectReservationStatus();
                restaurant.updateReservationStatus(reservationId, status);
                break;
            }
            case 7: {
                auto date = readDate();
                restaurant.displaySummary(date);
                break;
            }
            case 8: {
                report.displayReservationSummary(restaurant);
                break;
            }
            case 9: {
                running = false;
                break;
            }
            default:
                std::cout << "Unknown option.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
