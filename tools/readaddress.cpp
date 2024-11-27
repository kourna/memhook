#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>

void read_memory(pid_t pid, unsigned long address, size_t num_bytes, const std::string& output_file) {
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

    std::vector<char> buffer(num_bytes);
    ssize_t bytes_read = pread(mem_fd, buffer.data(), num_bytes, address);
    if (bytes_read <= 0) {
        std::cerr << "Error reading memory or no bytes read." << std::endl;
    } else {

        output << std::hex << std::setw(8) << std::setfill('0') << address << " : ";
        for (ssize_t i = 0; i < bytes_read; ++i) {
            output << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i])));
            if (i < bytes_read - 1) {
                output << " ";
            }
        }
        output << std::endl;
    }

    close(mem_fd);
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <pid> <address> <num_bytes> <output_file>" << std::endl;
        return 1;
    }

    pid_t pid = std::stoi(argv[1]);
    unsigned long address = std::stoul(argv[2], nullptr, 16); // Read address as hex
    size_t num_bytes = std::stoi(argv[3]);
    std::string output_file = argv[4];

    read_memory(pid, address, num_bytes, output_file);

    return 0;
}
