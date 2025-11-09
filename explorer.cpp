#include "explorer.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Explorer::Explorer() {
    cwd = fs::current_path().string();
}

std::vector<std::string> Explorer::split(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string temp;
    while (iss >> temp) {
        tokens.push_back(temp);
    }
    return tokens;
}

void Explorer::run() {
    std::string input;
    while (true) {
        std::cout << "[exp] " << cwd << " $ ";
        if (!std::getline(std::cin, input)) break;

        auto parts = split(input);
        if (parts.empty()) continue;
        std::string cmd = parts[0];
        parts.erase(parts.begin());

        if (cmd == "ls") cmd_ls(parts);
        else if (cmd == "cd") cmd_cd(parts);
        else if (cmd == "pwd") cmd_pwd(parts);
        else if (cmd == "cp") cmd_cp(parts);
        else if (cmd == "mv") cmd_mv(parts);
        else if (cmd == "rm") cmd_rm(parts);
        else if (cmd == "mkdir") cmd_mkdir(parts);
        else if (cmd == "touch") cmd_touch(parts);
        else if (cmd == "chmod") cmd_chmod(parts);
        else if (cmd == "search") cmd_search(parts);
        else if (cmd == "info") cmd_info(parts);
        else if (cmd == "help") cmd_help(parts);
        else if (cmd == "clear") cmd_clear(parts);
        else if (cmd == "exit" || cmd == "quit") break;
        else std::cout << "Unknown command.\n";
    }
}

// ---------- command implementations ----------

void Explorer::cmd_ls(const std::vector<std::string> &args) {
    bool showHidden = false;
    std::string path = cwd;

    for (auto &a : args) {
        if (a == "-a") showHidden = true;
        else path = a;
    }

    try {
        for (auto &p : fs::directory_iterator(path)) {
            std::string name = p.path().filename().string();
            if (!showHidden && name.size() > 0 && name[0] == '.') continue;
            if (fs::is_directory(p.status())) std::cout << "[DIR] ";
            std::cout << name << "\n";
        }
    } catch (...) {
        std::cout << "ls: cannot open " << path << "\n";
    }
}

void Explorer::cmd_cd(const std::vector<std::string> &args) {
    if (args.empty()) {
        cwd = fs::current_path().string();
        return;
    }
    fs::path p = args[0];
    if (!p.is_absolute()) p = fs::path(cwd) / p;
    if (fs::exists(p) && fs::is_directory(p)) {
        cwd = fs::weakly_canonical(p).string();
    } else {
        std::cout << "cd: no such directory\n";
    }
}

void Explorer::cmd_pwd(const std::vector<std::string> &) {
    std::cout << cwd << "\n";
}

void Explorer::cmd_cp(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Usage: cp <src> <dst>\n";
        return;
    }

    fs::path src = fs::path(cwd) / args[0];
    fs::path dst = fs::path(cwd) / args[1];

    try {
        if (fs::is_directory(src)) {
            fs::create_directories(dst);
            for (auto &p : fs::recursive_directory_iterator(src)) {
                fs::path rel = fs::relative(p.path(), src);
                fs::path target = dst / rel;
                if (fs::is_directory(p)) {
                    fs::create_directories(target);
                } else {
                    fs::create_directories(target.parent_path());
                    fs::copy_file(p.path(), target, fs::copy_options::overwrite_existing);
                }
            }
        } else {
            // If dst is an existing directory, copy inside it
            if (fs::exists(dst) && fs::is_directory(dst)) {
                dst /= src.filename();
            }
            fs::create_directories(dst.parent_path());
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        }
        std::cout << "Copied.\n";
    } catch (...) {
        std::cout << "cp error.\n";
    }
}

void Explorer::cmd_mv(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Usage: mv <src> <dst>\n";
        return;
    }
    try {
        fs::path src = fs::path(cwd) / args[0];
        fs::path dst = fs::path(cwd) / args[1];
        if (fs::exists(dst) && fs::is_directory(dst)) {
            dst /= src.filename();
        }
        fs::create_directories(dst.parent_path());
        fs::rename(src, dst);
        std::cout << "Moved.\n";
    } catch (...) {
        std::cout << "mv failed.\n";
    }
}

void Explorer::cmd_rm(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cout << "Usage: rm <path> [-r]\n";
        return;
    }
    bool recursive = false;
    std::string target;
    for (auto &a : args) {
        if (a == "-r") recursive = true;
        else target = a;
    }

    fs::path p = fs::path(cwd) / target;
    try {
        if (recursive) {
            fs::remove_all(p);
        } else {
            if (fs::is_directory(p)) {
                std::cout << "rm: is a directory (use -r)\n";
                return;
            }
            fs::remove(p);
        }
        std::cout << "Removed.\n";
    } catch (...) {
        std::cout << "rm error.\n";
    }
}

void Explorer::cmd_mkdir(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cout << "Usage: mkdir <dir>\n";
        return;
    }
    try {
        fs::create_directories(fs::path(cwd) / args[0]);
        std::cout << "Directory created.\n";
    } catch (...) {
        std::cout << "mkdir error.\n";
    }
}

void Explorer::cmd_touch(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cout << "Usage: touch <file>\n";
        return;
    }
    try {
        std::ofstream ofs(fs::path(cwd) / args[0], std::ios::app);
        ofs.close();
        std::cout << "Touched.\n";
    } catch (...) {
        std::cout << "touch error.\n";
    }
}

void Explorer::cmd_chmod(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Usage: chmod <octal> <file>\n";
        return;
    }
    try {
        // parse octal like 755
        auto mode = static_cast<fs::perms>(std::stoi(args[0], nullptr, 8));
        fs::permissions(fs::path(cwd) / args[1], mode, fs::perm_options::replace);
        std::cout << "Permissions changed.\n";
    } catch (...) {
        std::cout << "chmod error.\n";
    }
}

void Explorer::cmd_search(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cout << "Usage: search <pattern>\n";
        return;
    }
    std::string pat = args[0];
    try {
        for (auto &p : fs::recursive_directory_iterator(cwd)) {
            if (p.path().filename().string().find(pat) != std::string::npos) {
                std::cout << p.path().string() << "\n";
            }
        }
    } catch (...) {
        std::cout << "search error.\n";
    }
}

void Explorer::cmd_info(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cout << "Usage: info <file>\n";
        return;
    }
    fs::path p = fs::path(cwd) / args[0];
    try {
        if (!fs::exists(p)) {
            std::cout << "info: not found\n";
            return;
        }
        std::cout << "Path: " << p << "\n";
        std::cout << "Type: " << (fs::is_directory(p) ? "directory" : (fs::is_regular_file(p) ? "file" : "other")) << "\n";
        if (fs::is_regular_file(p)) {
            std::error_code ec;
            auto sz = fs::file_size(p, ec);
            if (!ec) std::cout << "Size: " << sz << " bytes\n";
        }
        std::cout.flush();
    } catch (...) {
        std::cout << "info error.\n";
    }
}

void Explorer::cmd_help(const std::vector<std::string> &) {
    std::cout << "Commands:\n";
    std::cout << " ls [-a] [path]\n cd [path]\n pwd\n cp <src> <dst>\n mv <src> <dst>\n";
    std::cout << " rm <path> [-r]\n mkdir <dir>\n touch <file>\n chmod <octal> <file>\n";
    std::cout << " search <pattern>\n info <file>\n clear\n help\n exit\n";
}

void Explorer::cmd_clear(const std::vector<std::string> &) {
    std::cout << "\033[2J\033[H";
}
