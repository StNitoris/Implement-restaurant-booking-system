#pragma once

#include "Customer.hpp"
#include "DateTime.hpp"
#include "Statuses.hpp"
#include "Table.hpp"
#include <memory>
#include <optional>
#include <string>

namespace restaurant {

class Reservation {
public:
    Reservation() = default;
    Reservation(int id, Customer customer, DateTime dateTime, int partySize)
        : id_(id), customer_(std::move(customer)), dateTime_(std::move(dateTime)),
          partySize_(partySize) {}

    int id() const { return id_; }
    const Customer &customer() const { return customer_; }
    const DateTime &dateTime() const { return dateTime_; }
    int partySize() const { return partySize_; }
    ReservationStatus status() const { return status_; }
    const std::string &notes() const { return notes_; }

    std::shared_ptr<Table> table() const { return table_.lock(); }

    void addNotes(const std::string &note) { notes_ = note; }

    void assignTable(const std::shared_ptr<Table> &table) {
        table_ = table;
        if (table) {
            table->setStatus(TableStatus::Reserved);
            status_ = ReservationStatus::Booked;
        }
    }

    void updateStatus(ReservationStatus status) {
        status_ = status;
        if (status == ReservationStatus::Cancelled) {
            if (auto t = table_.lock()) {
                t->setStatus(TableStatus::Free);
            }
        }
        if (status == ReservationStatus::Completed) {
            if (auto t = table_.lock()) {
                t->setStatus(TableStatus::Free);
            }
        }
        if (status == ReservationStatus::Seated) {
            if (auto t = table_.lock()) {
                t->setStatus(TableStatus::Occupied);
            }
        }
    }

    void clearTable() {
        if (auto t = table_.lock()) {
            t->setStatus(TableStatus::Free);
        }
        table_.reset();
    }

private:
    int id_ = 0;
    Customer customer_;
    DateTime dateTime_;
    int partySize_ = 0;
    ReservationStatus status_ = ReservationStatus::Open;
    std::weak_ptr<Table> table_;
    std::string notes_;
};

} // namespace restaurant
