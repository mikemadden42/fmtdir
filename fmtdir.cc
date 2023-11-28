#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool isHidden(const fs::path& p) {
#ifdef _WIN32
    // On Windows, hidden files start with a dot or are marked as hidden
    return p.filename().string()[0] == '.' ||
           (fs::status(p).permissions() & fs::perms::hidden) != fs::perms::none;
#else
    // On Unix-like systems, hidden files start with a dot
    return p.filename().string()[0] == '.';
#endif
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <directory> [option]\n"
                  << "Options:\n"
                  << "  -h, --hidden   Include hidden files\n";
        return 1;
    }

    const std::string path = argv[1];
    bool includeHidden = false;

    if (argc == 3) {
        std::string option = argv[2];
        if (option == "-h" || option == "--hidden") {
            includeHidden = true;
        } else {
            std::cerr << "Unknown option: " << option << "\n";
            return 1;
        }
    }

    try {
        std::map<std::string, std::vector<std::string>> fileGroups;

        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry.path()) &&
                (includeHidden || !isHidden(entry.path()))) {
                // Group files by extension
                std::string extension = entry.path().extension().string();
                fileGroups[extension].push_back(
                    entry.path().filename().string());
            }
        }

        // Sort each group by file name
        for (auto& group : fileGroups) {
            std::sort(group.second.begin(), group.second.end());
        }

        // Output the sorted groups
        for (const auto& group : fileGroups) {
            std::cout << group.first << ":\n";
            for (const auto& file : group.second) {
                std::cout << file << "\n";
            }
            std::cout << "\n";
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
