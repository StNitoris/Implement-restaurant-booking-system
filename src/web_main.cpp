#include "ReservationSystem.hpp"
#include "SeedData.hpp"
#include "WebServer.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using booking::BookingSheet;
using booking::Restaurant;

int main(int argc, char **argv) {
    int port = 8080;
    std::filesystem::path staticDir = "web";
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Invalid port, fallback to 8080" << std::endl;
            port = 8080;
        }
    }
    if (argc > 2) {
        staticDir = argv[2];
    }

    bool userProvidedStaticDir = argc > 2;
    auto existsDir = [](const std::filesystem::path &p) {
        return std::filesystem::exists(p) && std::filesystem::is_directory(p);
    };

    if (!existsDir(staticDir)) {
        if (!userProvidedStaticDir) {
            std::filesystem::path exePath = std::filesystem::absolute(argv[0]).parent_path();
            std::vector<std::filesystem::path> candidates = {
                exePath / "web",
                exePath.parent_path() / "web",
                std::filesystem::current_path() / "web"
            };

            for (const auto &candidate : candidates) {
                if (existsDir(candidate)) {
                    staticDir = candidate;
                    break;
                }
            }
        }
    }

    if (!existsDir(staticDir)) {
        std::cerr << "Static directory not found: " << staticDir << std::endl;
        return 1;
    }

    Restaurant restaurant{"美味餐厅", "上海市黄浦区中山东一路12号", BookingSheet{"2024-05-20"}};
    booking::seedRestaurant(restaurant);

    try {
        booking::runWebServer(restaurant, staticDir.string(), port);
    } catch (const std::exception &ex) {
        std::cerr << "Failed to start web server: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

