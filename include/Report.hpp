#pragma once

#include "BookingSheet.hpp"
#include "Menu.hpp"
#include <sstream>
#include <string>

namespace restaurant {

class Report {
public:
    Report() = default;
    explicit Report(std::string title) : title_(std::move(title)) {}

    const std::string &title() const { return title_; }
    const std::string &content() const { return content_; }

    void generateFrom(const BookingSheet &sheet, const std::vector<Order> &orders) {
        std::ostringstream oss;
        oss << "Report: " << title_ << "\n";
        oss << sheet.summary() << "\n";
        double revenue = 0.0;
        for (const auto &order : orders) {
            revenue += order.total();
        }
        oss << "Total revenue: " << revenue << "\n";
        content_ = oss.str();
    }

private:
    std::string title_;
    std::string content_;
};

} // namespace restaurant
