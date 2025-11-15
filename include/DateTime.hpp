#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace restaurant {

class DateTime {
public:
    DateTime() = default;

    explicit DateTime(const std::string &iso8601) {
        if (!iso8601.empty()) {
            if (!parse(iso8601)) {
                throw std::invalid_argument("Invalid ISO-8601 date time: " + iso8601);
            }
        }
    }

    static DateTime now() {
        DateTime dt;
        dt.timePoint_ = std::chrono::system_clock::now();
        return dt;
    }

    std::string toString() const {
        if (!hasValue()) {
            return "";
        }
        std::time_t tt = std::chrono::system_clock::to_time_t(timePoint_);
        std::tm tm = *std::gmtime(&tt);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

    bool hasValue() const {
        return initialized_;
    }

    const std::chrono::system_clock::time_point &value() const {
        return timePoint_;
    }

    bool operator<(const DateTime &other) const {
        return timePoint_ < other.timePoint_;
    }

    bool operator==(const DateTime &other) const {
        return timePoint_ == other.timePoint_;
    }

private:
    bool parse(const std::string &iso8601) {
        std::istringstream iss(iso8601);
        std::tm tm = {};
        iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M");
        if (iss.fail()) {
            return false;
        }
        auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        timePoint_ = tp;
        initialized_ = true;
        return true;
    }

    bool initialized_ = false;
    std::chrono::system_clock::time_point timePoint_{};
};

inline std::string formatDateTime(const DateTime &dt) {
    return dt.toString();
}

} // namespace restaurant
