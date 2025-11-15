#pragma once

#include "Reservation.hpp"
#include "Table.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

namespace restaurant {

class BookingSheet {
public:
    BookingSheet() = default;

    std::shared_ptr<Table> addTable(int id, int seats, std::string label = {}) {
        auto table = std::make_shared<Table>(id, seats);
        table->setLabel(std::move(label));
        tables_.push_back(table);
        return table;
    }

    Reservation &createReservation(const Customer &customer, const DateTime &dateTime,
                                   int partySize, const std::string &notes = {}) {
        Reservation reservation(++reservationSequence_, customer, dateTime, partySize);
        reservation.addNotes(notes);
        reservations_.push_back(std::move(reservation));
        return reservations_.back();
    }

    std::vector<std::shared_ptr<Table>> availableTables(int partySize) const {
        std::vector<std::shared_ptr<Table>> result;
        for (const auto &table : tables_) {
            if (table->isAvailable() && table->seats() >= partySize) {
                result.push_back(table);
            }
        }
        std::sort(result.begin(), result.end(),
                  [](const auto &a, const auto &b) { return a->seats() < b->seats(); });
        return result;
    }

    bool assignTable(int reservationId, int tableId) {
        auto reservation = findReservation(reservationId);
        auto table = findTable(tableId);
        if (!reservation || !table) {
            return false;
        }
        reservation->get().assignTable(table);
        return true;
    }

    std::optional<std::reference_wrapper<Reservation>> findReservation(int reservationId) {
        for (auto &reservation : reservations_) {
            if (reservation.id() == reservationId) {
                return reservation;
            }
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<const Reservation>> findReservation(
        int reservationId) const {
        for (const auto &reservation : reservations_) {
            if (reservation.id() == reservationId) {
                return reservation;
            }
        }
        return std::nullopt;
    }

    std::shared_ptr<Table> findTable(int tableId) const {
        for (const auto &table : tables_) {
            if (table->id() == tableId) {
                return table;
            }
        }
        return nullptr;
    }

    std::string summary() const {
        std::ostringstream oss;
        oss << "Reservations:\n";
        for (const auto &reservation : reservations_) {
            oss << "#" << reservation.id() << " - " << reservation.customer().name() << " | "
                << reservation.partySize() << " guests | "
                << formatDateTime(reservation.dateTime()) << " | "
                << to_string(reservation.status());
            if (auto table = reservation.table()) {
                oss << " | Table " << table->id();
            }
            oss << "\n";
        }
        return oss.str();
    }

    void autoAssign(Reservation &reservation) {
        auto tables = availableTables(reservation.partySize());
        if (!tables.empty()) {
            reservation.assignTable(tables.front());
        }
    }

    void cancelReservation(int reservationId) {
        auto reservation = findReservation(reservationId);
        if (reservation) {
            reservation->get().updateStatus(ReservationStatus::Cancelled);
        }
    }

    const std::vector<Reservation> &reservations() const { return reservations_; }
    const std::vector<std::shared_ptr<Table>> &tables() const { return tables_; }

private:
    int reservationSequence_ = 0;
    std::vector<std::shared_ptr<Table>> tables_;
    std::vector<Reservation> reservations_;
};

} // namespace restaurant
