#pragma once

#include "BookingSheet.hpp"
#include "Menu.hpp"
#include "Report.hpp"
#include "Roles.hpp"
#include <map>
#include <optional>

namespace restaurant {

class Restaurant {
public:
    Restaurant() = default;
    Restaurant(std::string name, std::string address)
        : name_(std::move(name)), address_(std::move(address)) {}

    const std::string &name() const { return name_; }
    const std::string &address() const { return address_; }
    BookingSheet &bookingSheet() { return bookingSheet_; }
    const BookingSheet &bookingSheet() const { return bookingSheet_; }

    void addStaff(const Staff &staff) { staffMembers_.push_back(staff); }
    const std::vector<Staff> &staff() const { return staffMembers_; }

    void addMenuItem(const MenuItem &item) { menu_.push_back(item); }
    const std::vector<MenuItem> &menu() const { return menu_; }

    Order &createOrder(int tableId) {
        orders_.emplace_back(++orderSequence_, tableId);
        return orders_.back();
    }

    std::optional<std::reference_wrapper<Order>> findOpenOrderByTable(int tableId) {
        for (auto &order : orders_) {
            if (order.tableId() == tableId && order.status() == OrderStatus::Open) {
                return order;
            }
        }
        return std::nullopt;
    }

    Report generateDailyReport(const std::string &title = "Daily Report") const {
        Report report(title);
        report.generateFrom(bookingSheet_, orders_);
        return report;
    }

    Reservation &bookReservation(const Customer &customer, const DateTime &dateTime,
                                 int partySize, const std::string &notes = {}) {
        auto &reservation = bookingSheet_.createReservation(customer, dateTime, partySize, notes);
        bookingSheet_.autoAssign(reservation);
        return reservation;
    }

    bool checkInReservation(int reservationId) {
        auto reservation = bookingSheet_.findReservation(reservationId);
        if (!reservation) {
            return false;
        }
        reservation->get().updateStatus(ReservationStatus::Seated);
        return true;
    }

    bool completeReservation(int reservationId) {
        auto reservation = bookingSheet_.findReservation(reservationId);
        if (!reservation) {
            return false;
        }
        reservation->get().updateStatus(ReservationStatus::Completed);
        return true;
    }

    void cancelReservation(int reservationId) { bookingSheet_.cancelReservation(reservationId); }

    const std::vector<Order> &orders() const { return orders_; }

private:
    std::string name_;
    std::string address_;
    BookingSheet bookingSheet_;
    std::vector<MenuItem> menu_;
    std::vector<Order> orders_;
    int orderSequence_ = 0;
    std::vector<Staff> staffMembers_;
};

} // namespace restaurant
