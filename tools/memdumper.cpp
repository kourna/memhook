#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <cstring>

void dump_memory(pid_t pid, const std::string& output_file) {
    std::ostringstream mem_file_path;
    mem_file_path << "/proc/" << pid << "/mem";

    int mem_fd = open(mem_file_path.str().c_str(), O_RDONLY);
    if (mem_fd == -1) {
        perror("Error opening memory file");
        return;
    }

    std::ofstream output(output_file);
    if (!output.is_open()) {
        perror("Error opening output file");
        close(mem_fd);
        return;
    }

    // Read the memory map
    std::ifstream maps_file("/proc/" + std::to_string(pid) + "/maps");
    std::string line;
    while (std::getline(maps_file, line)) {
        std::istringstream iss(line);
        std::string address_range;
        unsigned long start_addr, end_addr;
        char permissions[5];

        // Parse the line
        if (iss >> address_range >> permissions) {
            std::sscanf(address_range.c_str(), "%lx-%lx", &start_addr, &end_addr);
            // Read memory from this range
            for (unsigned long address = start_addr; address < end_addr; address += 16) {
                std::vector<char> buffer(16);
                ssize_t bytes_read = pread(mem_fd, buffer.data(), buffer.size(), address);
                if (bytes_read <= 0) {
                    break; // End of memory or error
                }

                // Output the address and the bytes in hex format
                output << std::hex << std::setw(8) << std::setfill('0') << address << " : ";
                for (ssize_t i = 0; i < bytes_read; ++i) {
                    output << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i])));
                    if (i < bytes_read - 1) {
                        output << " ";
                    }
                }
                output << std::endl;
            }
        }
    }

    close(mem_fd);
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pid> <output_file>" << std::endl;
        return 1;
    }

    pid_t pid = std::stoi(argv[1]);
    std::string output_file = argv[2];

    dump_memory(pid, output_file);

    return 0;
}
