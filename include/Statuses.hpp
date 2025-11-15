#pragma once

#include <string>

namespace restaurant {

enum class TableStatus {
    Free,
    Reserved,
    Occupied,
    OutOfService
};

inline std::string to_string(TableStatus status) {
    switch (status) {
    case TableStatus::Free:
        return "Free";
    case TableStatus::Reserved:
        return "Reserved";
    case TableStatus::Occupied:
        return "Occupied";
    case TableStatus::OutOfService:
        return "Out of Service";
    }
    return "Unknown";
}

enum class ReservationStatus {
    Open,
    Booked,
    Seated,
    Completed,
    Cancelled
};

inline std::string to_string(ReservationStatus status) {
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

enum class OrderStatus {
    Open,
    Closed,
    Cancelled
};

inline std::string to_string(OrderStatus status) {
    switch (status) {
    case OrderStatus::Open:
        return "Open";
    case OrderStatus::Closed:
        return "Closed";
    case OrderStatus::Cancelled:
        return "Cancelled";
    }
    return "Unknown";
}

} // namespace restaurant
