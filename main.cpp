#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <sstream>


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

pid_t getLowestPid(std::vector<pid_t> pids) {
  pid_t lowestPid = (pid_t)pids[1];
  for(int i = 0; i < pids.size(); i++) {
    if(reinterpret_cast<int>(lowestPid) > reinterpret_cast<int>(pids[i])) {lowestPid = pids[i];}
  }
  return lowestPid;
}

std::string getPidMemoryMap(pid_t pid) {

  std::cout << "pid : " << std::to_string(pid) << "\n";
  
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

void readStack(std::string memoryMap) {

  unsigned long start_addr , end_addr;

  std::istringstream stream(memoryMap);
  std::string address_range = "";
  std::string token;
  
  while(std::getline(stream, token)) {

    address_range = token;
    
  }

  std::cout << address_range << std::endl;
  
  std::sscanf(address_range.c_str(), "%lx-%lx", &start_addr, &end_addr);

  std::cout << start_addr << std::endl;
  std::cout << end_addr << std::endl;

  
  
  return;
  
}

int main() {
  
  std::string processName = "/home/snowy/.local/share/Steam/steamapps/common/Counter-Strike";
  
  std::vector<pid_t> pids = findPIDsByName(processName);
  
  if (pids.empty()) {
    std::cout << "No processes found with the name: " << processName << std::endl;
  } else {
    std::cout << "Found PIDs for '" << processName << "': ";
    for (pid_t pid : pids) {
      std::cout << pid << " ";
    }
    
    std::cout << std::endl;
    std::cout << "---------" << std::endl;
    std::cout << getLowestPid(pids) << std::endl;

    readStack(getPidMemoryMap(getLowestPid(pids)));

  }
  
  return 0;
}
