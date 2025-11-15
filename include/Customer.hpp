#pragma once

#include <string>

namespace restaurant {

class Customer {
public:
    Customer() = default;
    Customer(std::string name, std::string phone, std::string email)
        : name_(std::move(name)), phone_(std::move(phone)), email_(std::move(email)) {}

    const std::string &name() const { return name_; }
    const std::string &phone() const { return phone_; }
    const std::string &email() const { return email_; }

    void setName(const std::string &name) { name_ = name; }
    void setPhone(const std::string &phone) { phone_ = phone; }
    void setEmail(const std::string &email) { email_ = email; }

private:
    std::string name_;
    std::string phone_;
    std::string email_;
};

} // namespace restaurant
