#include "ReservationSystem.hpp"
#include "SeedData.hpp"
#include "WebServer.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
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

    auto containsIndex = [](const std::filesystem::path &p) {
        return std::filesystem::exists(p / "index.html");
    };

    auto addAncestorWebDirs = [](std::vector<std::filesystem::path> &list,
                                 const std::filesystem::path &base) {
        if (base.empty()) {
            return;
        }
        std::filesystem::path cursor = std::filesystem::absolute(base);
        while (!cursor.empty()) {
            list.push_back(cursor / "web");
            if (cursor == cursor.root_path()) {
                break;
            }
            cursor = cursor.parent_path();
        }
    };

    auto findStaticDir = [&](const std::filesystem::path &initial) -> std::optional<std::filesystem::path> {
        std::vector<std::filesystem::path> candidates;
        if (!initial.empty()) {
            candidates.push_back(initial);
        }

        addAncestorWebDirs(candidates, std::filesystem::path(argv[0]).parent_path());
        addAncestorWebDirs(candidates, std::filesystem::current_path());

        // Remove duplicate paths while preserving order
        std::vector<std::filesystem::path> deduped;
        for (const auto &candidate : candidates) {
            auto absCandidate = std::filesystem::absolute(candidate);
            if (std::find(deduped.begin(), deduped.end(), absCandidate) == deduped.end()) {
                deduped.push_back(absCandidate);
            }
        }

        for (const auto &candidate : deduped) {
            if (existsDir(candidate) && containsIndex(candidate)) {
                return candidate;
            }
        }
        std::cerr << "Static directory not found. Tried: " << std::endl;
        for (const auto &candidate : deduped) {
            std::cerr << "  - " << candidate << std::endl;
        }
        return std::nullopt;
    };

    auto resolved = findStaticDir(userProvidedStaticDir ? staticDir : std::filesystem::path{});
    if (!resolved) {
        return 1;
    }
    staticDir = *resolved;
    std::cout << "Serving static files from: " << staticDir << std::endl;

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

