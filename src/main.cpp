#include "BookingSheet.hpp"
#include "Restaurant.hpp"
#include <iostream>

using namespace restaurant;

Permission makePermission(const std::string &name) {
    return Permission(name);
}

int main() {
    Restaurant restaurant("Ocean Breeze", "123 Harbor Road");

    // Configure staff and permissions
    Permission createReservationPerm = makePermission("create_reservation");
    Permission manageOrdersPerm = makePermission("manage_orders");
    Permission runReportsPerm = makePermission("run_reports");

    Role frontDeskRole("Front Desk");
    frontDeskRole.addPermission(createReservationPerm);

    Role managerRole("Manager");
    managerRole.addPermission(createReservationPerm);
    managerRole.addPermission(manageOrdersPerm);
    managerRole.addPermission(runReportsPerm);

    restaurant.addStaff(FrontDeskStaff("Alice", frontDeskRole));
    restaurant.addStaff(Manager("Bob", managerRole));

    // Setup tables
    restaurant.bookingSheet().addTable(1, 2, "Window");
    restaurant.bookingSheet().addTable(2, 4, "Center");
    restaurant.bookingSheet().addTable(3, 6, "Patio");

    // Setup menu
    restaurant.addMenuItem(MenuItem("Lobster Bisque", "Creamy lobster soup", 14.5, "Starter"));
    restaurant.addMenuItem(MenuItem("Grilled Salmon", "Served with seasonal vegetables", 28.0,
                                      "Main"));
    restaurant.addMenuItem(MenuItem("Chocolate Lava Cake", "Rich chocolate dessert", 9.5, "Dessert"));

    // Create reservations
    Customer customer1("Chen Li", "+86 18800001111", "chen@example.com");
    Customer customer2("Wang Wei", "+86 18800002222", "wang@example.com");

    int reservation1Id = restaurant
                              .bookReservation(customer1, DateTime("2024-08-08T19:00"), 2,
                                              "Birthday celebration")
                              .id();
    int reservation2Id = restaurant
                              .bookReservation(customer2, DateTime("2024-08-08T20:00"), 5,
                                              "Family dinner")
                              .id();

    std::cout << "Initial booking sheet:\n" << restaurant.bookingSheet().summary() << '\n';

    // Check-in first reservation and create an order
    restaurant.checkInReservation(reservation1Id);
    auto reservation1Ref = restaurant.bookingSheet().findReservation(reservation1Id);
    if (!reservation1Ref) {
        std::cerr << "Reservation not found" << std::endl;
        return 1;
    }
    auto tablePtr = reservation1Ref->get().table();
    if (!tablePtr) {
        std::cerr << "Reservation is not assigned to a table" << std::endl;
        return 1;
    }

    auto orderOpt = restaurant.findOpenOrderByTable(tablePtr->id());
    Order *order = nullptr;
    if (!orderOpt) {
        order = &restaurant.createOrder(tablePtr->id());
    } else {
        order = &orderOpt->get();
    }

    if (order) {
        order->addItem(restaurant.menu().at(0), 2); // two soups
        order->addItem(restaurant.menu().at(1), 2); // two mains
        std::cout << order->summary() << '\n';
        order->close();
    }

    restaurant.completeReservation(reservation1Id);

    std::cout << "Booking sheet after seating reservation 1:\n"
              << restaurant.bookingSheet().summary() << '\n';

    // Cancel second reservation to free table
    restaurant.cancelReservation(reservation2Id);

    std::cout << "Booking sheet after cancellation:\n" << restaurant.bookingSheet().summary() << '\n';

    // Generate report
    Report report = restaurant.generateDailyReport();
    std::cout << report.content() << '\n';

    return 0;
}
