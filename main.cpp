#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <cstring>
#include <vector>

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
  }
  
  return 0;
}
