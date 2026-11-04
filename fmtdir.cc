#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace fs = std::filesystem;

bool isHidden(const fs::path& p) {
#ifdef _WIN32
    // On Windows, check if the file is hidden based on its attributes
    DWORD attrs = GetFileAttributesW(p.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES &&
           (attrs & FILE_ATTRIBUTE_HIDDEN) != 0;
#else
    // On Unix-like systems, hidden files start with a dot
    return p.filename().string()[0] == '.';
#endif
}

// Lowercase ASCII letters only; other bytes (including UTF-8) are unchanged
std::string toLowerAscii(std::string s) {
    for (auto& c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

// Order names ignoring case, then by exact spelling so the order is stable
bool lessIgnoringCase(const std::string& a, const std::string& b) {
    const std::string lowerA = toLowerAscii(a);
    const std::string lowerB = toLowerAscii(b);
    return lowerA != lowerB ? lowerA < lowerB : a < b;
}

void printUsage(std::ostream& out, const std::string& program) {
    out << "Usage: " << program << " [options] <directory>\n"
        << "Options:\n"
        << "  -a, --all, --hidden   Include hidden files\n"
        << "  -h, --help            Show this help\n";
}

// Arguments are UTF-8, including the program name in args[0]
int run(const std::vector<std::string>& args) {
    std::string path;
    bool includeHidden = false;
    bool optionsDone = false;

    // Options may appear before or after the directory; "--" ends options
    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (!optionsDone && arg.size() > 1 && arg[0] == '-') {
            if (arg == "--") {
                optionsDone = true;
            } else if (arg == "-a" || arg == "--all" || arg == "--hidden") {
                includeHidden = true;
            } else if (arg == "-h" || arg == "--help") {
                printUsage(std::cout, args[0]);
                return 0;
            } else {
                std::cerr << "Unknown option: " << arg << "\n";
                return 1;
            }
        } else if (path.empty()) {
            path = arg;
        } else {
            std::cerr << "Only one directory may be given\n";
            return 1;
        }
    }

    if (path.empty()) {
        printUsage(std::cerr, args[0]);
        return 1;
    }

    try {
        std::map<std::string, std::vector<std::string>> fileGroups;

        for (const auto& entry : fs::directory_iterator(fs::u8path(path))) {
            if (entry.is_regular_file() &&
                (includeHidden || !isHidden(entry.path()))) {
                // Group files by extension, ignoring case. Use UTF-8 so names
                // that the Windows code page can't represent don't throw.
                std::string extension =
                    toLowerAscii(entry.path().extension().u8string());
                fileGroups[extension].push_back(
                    entry.path().filename().u8string());
            }
        }

        // Sort each group by file name, ignoring case, and output it, with a
        // blank line between groups
        bool firstGroup = true;
        for (auto& group : fileGroups) {
            std::sort(group.second.begin(), group.second.end(),
                      lessIgnoringCase);

            if (!firstGroup) {
                std::cout << "\n";
            }
            firstGroup = false;

            std::cout << (group.first.empty() ? "(no extension)" : group.first)
                      << ":\n";
            for (const auto& file : group.second) {
                std::cout << file << "\n";
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

#ifdef _WIN32
// Use wmain so arguments outside the Windows code page arrive intact
int wmain(int argc, wchar_t* argv[]) {
    std::vector<std::string> args;
    try {
        for (int i = 0; i < argc; ++i) {
            args.push_back(fs::path(argv[i]).u8string());
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    // Output is UTF-8, so have the console show it as UTF-8, and restore the
    // console's code page afterwards since it's shared with the parent shell
    const UINT oldCodePage = GetConsoleOutputCP();
    if (oldCodePage != 0) {
        SetConsoleOutputCP(CP_UTF8);
    }
    const int result = run(args);
    if (oldCodePage != 0) {
        SetConsoleOutputCP(oldCodePage);
    }
    return result;
}
#else
int main(int argc, char* argv[]) {
    return run(std::vector<std::string>(argv, argv + argc));
}
#endif
