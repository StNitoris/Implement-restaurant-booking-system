#pragma once

#include "Statuses.hpp"
#include <string>

namespace restaurant {

class Table {
public:
    Table() = default;
    Table(int id, int seats)
        : id_(id), seats_(seats), status_(TableStatus::Free) {}

    int id() const { return id_; }
    int seats() const { return seats_; }
    TableStatus status() const { return status_; }
    const std::string &label() const { return label_; }

    void setLabel(std::string label) { label_ = std::move(label); }
    void setSeats(int seats) { seats_ = seats; }

    void setStatus(TableStatus status) { status_ = status; }

    bool isAvailable() const {
        return status_ == TableStatus::Free;
    }

private:
    int id_ = 0;
    int seats_ = 0;
    TableStatus status_ = TableStatus::Free;
    std::string label_;
};

} // namespace restaurant
