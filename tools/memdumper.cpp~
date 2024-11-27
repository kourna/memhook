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

    // Read the memory in chunks
    const size_t chunk_size = 16; // Number of bytes to read at a time
    unsigned long address = 0; // Start from address 0
    std::vector<char> buffer(chunk_size);

    while (true) {
        // Read memory from the process
        ssize_t bytes_read = pread(mem_fd, buffer.data(), chunk_size, address);
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

        address += bytes_read; // Move to the next address
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
