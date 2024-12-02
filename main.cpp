#define APPLICATION_SCAN_STRING "./app_stacktest"

#include "components/gui.h"

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

enum scanMode {
  SCAN8BYTE,
  SCAN4BYTE,
  SCAN1BYTE
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


//i fucking hate my life why do i exist

std::vector<std::array<unsigned long,2>> read_memory(pid_t pid, unsigned long address, unsigned long num_bytes, scanMode scan_mode) {
  
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


    //++++++++ SCANNING MULTI BYTE ++++++++
    
    if(scan_mode == SCAN4BYTE) {

      for (ssize_t i = bytes_read; i > 0; i = i-4) {

	unsigned int value_at_addr = 
	  (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i-3])) << 24) |
	  (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i-2])) << 16) |
	  (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i-1])) << 8) |
	  (static_cast<unsigned int>(static_cast<unsigned char>(buffer[i])));;
	
	//scanning all addresses with 0 removed for performance reasons will rmeove later.
	if(value_at_addr == 0){
	  continue;
	}
	
	std::cout << std::dec << value_at_addr << " - value at address: " << std::hex << address + i << std::endl;
	
	if (address + i < address) {
	  std::cerr << "Address overflow detected!" << std::endl;
	  break;
	}
	
	address_list.push_back({address+i, value_at_addr});
	
	if (i < bytes_read - 1) {
	  std::cout << " ";
	}
        
	std::cout << std::endl;
	
      }

    }

    //++++++++ SCANNING MULTI BYTE END ++++++++

    
    //++++++++ SCANNING SINGLE BYTE ++++++++

    if(scan_mode == SCAN1BYTE) {
    
      for (ssize_t i = bytes_read; i > 0; --i) {
	
	unsigned int value_at_addr = static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));
	
	//scanning all addresses with 0 removed for performance reasons will rmeove later.
	if(value_at_addr == 0){
	  continue;
	}
	
	std::cout << std::dec << value_at_addr << " - value at address: " << std::hex << address + i << std::endl;
	
	if (address + i < address) {
	  std::cerr << "Address overflow detected!" << std::endl;
	  break;
	}
	
	address_list.push_back({address+i, value_at_addr});
	
	if (i < bytes_read - 1) {
	  std::cout << " ";
	}
        
	std::cout << std::endl;
	
      }
    }

    //++++++++ SCANNING SINGLE BYTE END ++++++++
    
    close(mem_fd);
    output.close();  
  
  }
  
  return address_list;
}

std::vector<pid_t> find_pids_by_name(const std::string& processName) {
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

pid_t get_lowest_pid(std::vector<pid_t> pids) {
  pid_t lowestPid = (pid_t)pids[0];
  for(int i = 0; i < pids.size(); i++) {
    if(reinterpret_cast<int>(lowestPid) > reinterpret_cast<int>(pids[i])) {lowestPid = pids[i];}
  }
  return lowestPid;
}

std::string get_pid_memory_map(pid_t pid) {

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

stackAddresses* read_stack_addresses(std::string memoryMap) {

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
  
  std::vector<pid_t> pids = find_pids_by_name(processName);
  
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
    pid_t targetPid = get_lowest_pid(pids);
    
    activeStackAddressStruct = read_stack_addresses(get_pid_memory_map(targetPid));

    std::cout << "start address :" << activeStackAddressStruct->start_addr << std::endl;
    std::cout << "end address :" << activeStackAddressStruct->end_addr << std::endl;
    printBar();

    unsigned long bytes_to_read = activeStackAddressStruct->end_addr-activeStackAddressStruct->start_addr;

    std::cout << "Trying to read: " << bytes_to_read << " bytes of stack." << std::endl;
    
    std::vector<std::array<unsigned long, 2>> address_list = read_memory(targetPid, activeStackAddressStruct->start_addr, bytes_to_read, SCAN4BYTE);

    log_address_list(address_list);
    
    filter_address_list(address_list, (unsigned long)100);

    wruff_gui gui_manager;
    
    std::cout << gui_manager.init_gui() << std::endl;
    
  }
  
  return 0;
}
