#pragma once

#include "Statuses.hpp"
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace restaurant {

class MenuItem {
public:
    MenuItem() = default;
    MenuItem(std::string name, std::string description, double price, std::string category)
        : name_(std::move(name)), description_(std::move(description)), price_(price),
          category_(std::move(category)) {}

    const std::string &name() const { return name_; }
    const std::string &description() const { return description_; }
    double price() const { return price_; }
    const std::string &category() const { return category_; }

private:
    std::string name_;
    std::string description_;
    double price_ = 0.0;
    std::string category_;
};

class OrderItem {
public:
    OrderItem(const MenuItem &item, int quantity) : item_(item), quantity_(quantity) {}

    const MenuItem &item() const { return item_; }
    int quantity() const { return quantity_; }
    double total() const { return item_.price() * quantity_; }

private:
    MenuItem item_;
    int quantity_ = 0;
};

class Order {
public:
    Order() = default;
    Order(int id, int tableId) : id_(id), tableId_(tableId) {}

    int id() const { return id_; }
    int tableId() const { return tableId_; }
    OrderStatus status() const { return status_; }

    void addItem(const MenuItem &item, int quantity) {
        items_.emplace_back(item, quantity);
    }

    double total() const {
        double sum = 0.0;
        for (const auto &item : items_) {
            sum += item.total();
        }
        return sum;
    }

    void close() { status_ = OrderStatus::Closed; }
    void cancel() { status_ = OrderStatus::Cancelled; }

    std::string summary() const {
        std::ostringstream oss;
        oss << "Order " << id_ << " for table " << tableId_ << "\n";
        for (const auto &item : items_) {
            oss << "  - " << item.item().name() << " x" << item.quantity() << "\n";
        }
        oss << "Total: " << std::fixed << std::setprecision(2) << total() << "\n";
        return oss.str();
    }

private:
    int id_ = 0;
    int tableId_ = 0;
    OrderStatus status_ = OrderStatus::Open;
    std::vector<OrderItem> items_;
};

} // namespace restaurant
