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
#include <array>

struct stackAddresses{
  unsigned long start_addr;
  unsigned long end_addr;
};

std::vector<std::array<unsigned long, 2>> filter_address_list(std::vector<std::array<unsigned long, 2>> address_list, unsigned long search_filter) {

  unsigned long filtered_list_length = 0;
  std::vector<std::array<unsigned long, 2>> filtered_list;

  std::cout << "Items matching search filter: " << std::endl;

  for (int i = 0; i < address_list.size(); ++i) {
    
    if (address_list[i][1] == search_filter) {

      std::cout << "{ ";

      filtered_list.push_back({address_list[i][0],address_list[i][1]});
      std::cout << std::hex << address_list[i][0] << std::dec << "-" << address_list[i][1];

      std::cout << "}" << std::endl; 

    }
    
  }
  
  return filtered_list;
}

void log_address_list(std::vector<std::array<unsigned long,2>> address_list) {

  std::cout << "Vector of arrays: " << std::endl;
  for (int i = 0; i < address_list.size(); ++i) {
    std::cout << "{ ";

    std::cout << std::hex << address_list[i][0] << std::dec << "-" << address_list[i][1];
    
    std::cout << "}" << std::endl; 
  }
  
}

std::vector<std::array<unsigned long, 2>> read_memory(pid_t pid, unsigned long address, unsigned long num_bytes) {

  std::vector<std::array<unsigned long,2>> address_list;
  
  std::ostringstream mem_file_path;
  mem_file_path << "/proc/" << pid << "/mem";
  
  int mem_fd = open(mem_file_path.str().c_str(), O_RDONLY);
  if (mem_fd == -1) {
    perror("Error opening memory file");
    return address_list;
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

      unsigned int byte_at_addr = static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));

      //scanning all addresses with 0 removed for performance reasons will rmeove later.
      if(byte_at_addr == 0){
	continue;
      }
      
      std::cout << std::dec << byte_at_addr << " - value at address: " << std::hex << address + i << std::endl;
      
      if (address + i < address) {
	std::cerr << "Address overflow detected!" << std::endl;
	break;
      }
      
      address_list.push_back({address+i, byte_at_addr});
      
      if (i < bytes_read - 1) {
	std::cout << " ";
      }

    }
    
    log_address_list(address_list);
    
    std::cout << std::endl;

  }
  
  close(mem_fd);
  output.close();

  return address_list;
  
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

    unsigned long bytes_to_read = activeStackAddressStruct->end_addr-activeStackAddressStruct->start_addr;

    std::cout << "Trying to read: " << bytes_to_read << " bytes of stack." << std::endl;
    
    std::vector<std::array<unsigned long, 2>> address_list = read_memory(targetPid, activeStackAddressStruct->start_addr, bytes_to_read);

    filter_address_list(address_list, (unsigned long)100);
    
  }
  
  return 0;
}
