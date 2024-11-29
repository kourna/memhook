#define APPLICATION_SCAN_STRING "./app_stacktest"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <sstream>

struct stackAddresses{
  unsigned long start_addr;
  unsigned long end_addr;
};

void read_memory(pid_t pid, unsigned long address, size_t num_bytes) {
    std::ostringstream mem_file_path;
    mem_file_path << "/proc/" << pid << "/mem";

    int mem_fd = open(mem_file_path.str().c_str(), O_RDONLY);
    if (mem_fd == -1) {
        perror("Error opening memory file");
        return;
    }

    std::ofstream output;

    std::vector<char> buffer(num_bytes);
    ssize_t bytes_read = pread(mem_fd, buffer.data(), num_bytes, address);

    std::cout << bytes_read << " - bytes_read" << std::endl;
    std::cout << buffer[1] << " - buffer pre" << std::endl;
    
    if (bytes_read <= 0) {
        std::cerr << "Error reading memory or no bytes read." << std::endl;
    } else {

        for (ssize_t i = 0; i < bytes_read; ++i) {
	  std::cout << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i])));
	  std::cout << static_cast<unsigned int>(static_cast<unsigned char>(buffer[i])) << " - buffer aft" << std::endl;

	  
	  if (i < bytes_read - 1) {
	      std::cout << " ";
            }
        }
	std::cout << std::endl;
    }

    close(mem_fd);
    output.close();
}

std::vector<pid_t> findPIDsByName(const std::string& processName) {
  std::vector<pid_t> pids;
  DIR* dir = opendir("/proc");
  struct dirent* entry;
  
  if (dir == nullptr) {
    perror("opendir");
    return pids;
  }
  
  while ((entry = readdir(dir)) != nullptr) {
    
    if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
      
      std::string pid = entry->d_name;
      std::string cmdlinePath = "/proc/" + pid + "/cmdline";
      std::ifstream cmdlineFile(cmdlinePath);
      
      if (cmdlineFile) {
	std::string cmdline;
	std::getline(cmdlineFile, cmdline);
	
	if (cmdline.find(processName) != std::string::npos) {
	  pids.push_back(static_cast<pid_t>(std::stoi(pid)));
	}
      }
    }
  }
  
  closedir(dir);
  return pids;
}

void printBar() {
  std::cout << "----------" << std::endl;
  return;
}

pid_t getLowestPid(std::vector<pid_t> pids) {
  pid_t lowestPid = (pid_t)pids[0];
  for(int i = 0; i < pids.size(); i++) {
    if(reinterpret_cast<int>(lowestPid) > reinterpret_cast<int>(pids[i])) {lowestPid = pids[i];}
  }
  return lowestPid;
}

std::string getPidMemoryMap(pid_t pid) {

  std::cout << "Application master pid : " << std::to_string(pid) << "\n";
  
  std::string toprint = "/proc/";
  toprint += std::to_string(pid);
  toprint += "/maps";

  std::cout << "Trying to open memory map:" << toprint << "\n";
  
  std::ifstream inputPidMap(toprint);

  if (!inputPidMap.is_open()) {

    std::cerr << "Error opening file!" << std::endl;
    return "0";
    
  } else {

    std::stringstream buffer;
    buffer << inputPidMap.rdbuf();

    std::string pidMap = buffer.str();

    inputPidMap.close();

     return pidMap;
     
  }
  
}

stackAddresses* readStackAddresses(std::string memoryMap) {

  std::istringstream stream(memoryMap);
  std::string address_range = "";
  std::string token;
  
  while(std::getline(stream, token)) {

    address_range = token;
    if (address_range.find((std::string)"[stack]") != std::string::npos) {
      std::cout << "found stack!" << '\n';
      break;
    }
    
  }

  printBar();
  std::cout << "Potentially identified stack: " << address_range << std::endl;
  printBar();
  
  stackAddresses *returnstruct = new stackAddresses;

  unsigned long tmp_start_addr = 0,  tmp_end_addr = 0;
  
  std::sscanf(address_range.c_str(), "%lx-%lx", &tmp_start_addr, &tmp_end_addr);

  returnstruct -> start_addr = tmp_start_addr;
  returnstruct -> end_addr = tmp_end_addr;
  
  return returnstruct;
  
}

int main() {
  
  std::string processName = APPLICATION_SCAN_STRING;
  
  std::vector<pid_t> pids = findPIDsByName(processName);
  
  if (pids.empty()) {

    std::cout << "No processes found with the name: " << processName << std::endl;

  } else {

    std::cout << "Found PIDs for '" << processName << "': ";

    for (pid_t pid : pids) {
      std::cout << pid << " ";
    }
    
    std::cout << std::endl;
    printBar();
    
    stackAddresses *activeStackAddressStruct;
    pid_t targetPid = getLowestPid(pids);
    
    activeStackAddressStruct = readStackAddresses(getPidMemoryMap(targetPid));

    std::cout << activeStackAddressStruct->start_addr << std::endl;
    std::cout << activeStackAddressStruct->end_addr << std::endl;
    printBar();

    read_memory(targetPid, activeStackAddressStruct->end_addr-20, (size_t)10);

  }
  
  return 0;
}
