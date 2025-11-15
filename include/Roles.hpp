#pragma once

#include <set>
#include <string>

namespace restaurant {

class Permission {
public:
    Permission() = default;
    explicit Permission(std::string name) : name_(std::move(name)) {}

    const std::string &name() const { return name_; }

    bool operator<(const Permission &other) const { return name_ < other.name_; }
    bool operator==(const Permission &other) const { return name_ == other.name_; }

private:
    std::string name_;
};

class Role {
public:
    Role() = default;
    explicit Role(std::string name) : name_(std::move(name)) {}

    const std::string &name() const { return name_; }

    void addPermission(const Permission &permission) { permissions_.insert(permission); }

    bool hasPermission(const Permission &permission) const {
        return permissions_.count(permission) > 0;
    }

private:
    std::string name_;
    std::set<Permission> permissions_;
};

class Staff {
public:
    Staff() = default;
    Staff(std::string name, Role role) : name_(std::move(name)), role_(std::move(role)) {}

    const std::string &name() const { return name_; }
    const Role &role() const { return role_; }

    bool can(const Permission &permission) const { return role_.hasPermission(permission); }

private:
    std::string name_;
    Role role_;
};

class FrontDeskStaff : public Staff {
public:
    using Staff::Staff;
};

class Manager : public Staff {
public:
    using Staff::Staff;
};

} // namespace restaurant
