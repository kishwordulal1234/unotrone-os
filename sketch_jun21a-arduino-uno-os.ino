
/*
  Nanotrone OS v0.4 - Memory Optimized Edition
  Powered by Cupit Kernel
  Designed for Arduino Uno (2KB RAM limit)
*/

// Reduced configuration for Arduino Uno
#define MAX_FILES 8
#define MAX_DIRS 4
#define MAX_CONTENT_LEN 32
#define MAX_CMD_LEN 32
#define VERSION "0.4"

// Compact structures using minimal memory
struct File {
  char name[12];
  char content[MAX_CONTENT_LEN];
  byte parentDir; // 0=root, 1-4=dir index
  bool exists;
};

struct Dir {
  char name[8];
  bool exists;
};

// Global variables - minimized
char input[MAX_CMD_LEN];
byte inputLen = 0;
byte currentDir = 0; // 0=root, 1-4=dir indices
char userName[8] = "root";
unsigned long bootTime;

File files[MAX_FILES];
Dir dirs[MAX_DIRS];

void setup() {
  Serial.begin(9600);
  bootTime = millis();
  
  // Initialize arrays
  for(int i = 0; i < MAX_FILES; i++) files[i].exists = false;
  for(int i = 0; i < MAX_DIRS; i++) dirs[i].exists = false;
  
  // Boot sequence with ASCII art
  showBootScreen();
  
  // Create default directories
  strcpy(dirs[0].name, "bin");
  dirs[0].exists = true;
  strcpy(dirs[1].name, "tmp");
  dirs[1].exists = true;
  
  Serial.println(F("System ready. Type 'help' for commands."));
  prompt();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      input[inputLen] = '\0';
      if (inputLen > 0) {
        executeCommand();
        inputLen = 0;
      }
      prompt();
    } 
    else if (c == '\b' || c == 127) { // Backspace
      if (inputLen > 0) {
        inputLen--;
        Serial.print(F("\b \b"));
      }
    }
    else if (inputLen < MAX_CMD_LEN - 1) {
      input[inputLen++] = c;
      Serial.print(c);
    }
  }
}

void showBootScreen() {
  delay(500);
  Serial.println();
  Serial.println(F("    ███╗   ██╗ █████╗ ███╗   ██╗ ██████╗ "));
  Serial.println(F("    ████╗  ██║██╔══██╗████╗  ██║██╔═══██╗"));
  Serial.println(F("    ██╔██╗ ██║███████║██╔██╗ ██║██║   ██║"));
  Serial.println(F("    ██║╚██╗██║██╔══██║██║╚██╗██║██║   ██║"));
  Serial.println(F("    ██║ ╚████║██║  ██║██║ ╚████║╚██████╔╝"));
  Serial.println(F("    ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ "));
  Serial.println();
  Serial.println(F("  ┌─────────────────────────────────────┐"));
  Serial.println(F("  │        Nanotrone OS v" VERSION "           │"));
  Serial.println(F("  │      Powered by Cupit Kernel       │"));
  Serial.println(F("  │     Optimized for Arduino Uno      │"));
  Serial.println(F("  └─────────────────────────────────────┘"));
  Serial.println();
  
  // Boot animation
  Serial.print(F("Initializing"));
  for(int i = 0; i < 8; i++) {
    delay(200);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("Boot complete!"));
  Serial.println();
}

void prompt() {
  Serial.print(F("["));
  Serial.print(userName);
  Serial.print(F("@nano "));
  if (currentDir == 0) {
    Serial.print(F("/"));
  } else {
    Serial.print(F("/"));
    Serial.print(dirs[currentDir-1].name);
  }
  Serial.print(F("]$ "));
}

void executeCommand() {
  // Basic commands
  if (strcmp(input, "help") == 0) {
    showHelp();
  }
  else if (strcmp(input, "clear") == 0) {
    Serial.print(F("\033[2J\033[H"));
  }
  else if (strcmp(input, "neofetch") == 0) {
    neofetch();
  }
  else if (strcmp(input, "reboot") == 0) {
    Serial.println(F("Rebooting..."));
    delay(1000);
    asm volatile ("  jmp 0");
  }
  else if (strcmp(input, "uptime") == 0) {
    Serial.print(F("Uptime: "));
    Serial.print((millis() - bootTime) / 1000);
    Serial.println(F(" seconds"));
  }
  else if (strcmp(input, "meminfo") == 0) {
    memInfo();
  }
  
  // File system commands
  else if (strcmp(input, "pwd") == 0) {
    pwd();
  }
  else if (strcmp(input, "ls") == 0) {
    listDir();
  }
  else if (strncmp(input, "mkdir ", 6) == 0) {
    mkdir(input + 6);
  }
  else if (strncmp(input, "cd ", 3) == 0) {
    cd(input + 3);
  }
  else if (strncmp(input, "touch ", 6) == 0) {
    createFile(input + 6);
  }
  else if (strncmp(input, "rm ", 3) == 0) {
    deleteFile(input + 3);
  }
  else if (strncmp(input, "cat ", 4) == 0) {
    cat(input + 4);
  }
  else if (strncmp(input, "echo ", 5) == 0) {
    handleEcho(input + 5);
  }
  else if (strcmp(input, "df") == 0) {
    diskUsage();
  }
  
  // System info
  else if (strcmp(input, "whoami") == 0) {
    Serial.println(userName);
  }
  else if (strcmp(input, "date") == 0) {
    Serial.print(F("System time: "));
    Serial.print(millis() / 1000);
    Serial.println(F(" sec since boot"));
  }
  
  else {
    Serial.println(F("Command not found. Type 'help'."));
  }
}

void showHelp() {
  Serial.println(F("Nanotrone OS v" VERSION " Commands:"));
  Serial.println();
  Serial.println(F("System:"));
  Serial.println(F("  help     - Show commands"));
  Serial.println(F("  clear    - Clear screen"));
  Serial.println(F("  neofetch - System info"));
  Serial.println(F("  uptime   - System uptime"));
  Serial.println(F("  meminfo  - Memory info"));
  Serial.println(F("  reboot   - Restart"));
  Serial.println(F("  whoami   - Current user"));
  Serial.println(F("  date     - System time"));
  Serial.println();
  Serial.println(F("Files:"));
  Serial.println(F("  ls       - List files"));
  Serial.println(F("  pwd      - Current dir"));
  Serial.println(F("  cd <dir> - Change dir"));
  Serial.println(F("  mkdir <n>- Create dir"));
  Serial.println(F("  touch <n>- Create file"));
  Serial.println(F("  rm <file>- Delete file"));
  Serial.println(F("  cat <f>  - Show file"));
  Serial.println(F("  echo <t> - Print text"));
  Serial.println(F("  df       - Disk usage"));
}

void neofetch() {
  Serial.println(F("     ▄▄▄▄▄▄▄"));
  Serial.println(F("   ▄█████████▄"));
  Serial.println(F("  ███████████████"));
  Serial.println(F("  ███████████████  OS: Nanotrone v" VERSION));
  Serial.println(F("  ███████████████  Kernel: Cupit"));
  Serial.println(F("  ▀█████████▀███  CPU: ATmega328P"));
  Serial.println(F("    ▀▀▀▀▀▀▀  ███  RAM: 2KB SRAM"));
  Serial.println(F("            ▀▀▀   Flash: 32KB"));
  Serial.print(F("                  Uptime: "));
  Serial.print((millis() - bootTime) / 1000);
  Serial.println(F("s"));
}

void memInfo() {
  extern int __heap_start, *__brkval;
  int v;
  int free = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  
  Serial.println(F("Memory Info:"));
  Serial.print(F("Free RAM: "));
  Serial.print(free);
  Serial.println(F(" bytes"));
  Serial.print(F("Files: "));
  Serial.print(countFiles());
  Serial.print(F("/"));
  Serial.println(MAX_FILES);
  Serial.print(F("Dirs: "));
  Serial.print(countDirs());
  Serial.print(F("/"));
  Serial.println(MAX_DIRS);
}

void pwd() {
  if (currentDir == 0) {
    Serial.println(F("/"));
  } else {
    Serial.print(F("/"));
    Serial.println(dirs[currentDir-1].name);
  }
}

void listDir() {
  Serial.println(F("Contents:"));
  
  // Show directories if in root
  if (currentDir == 0) {
    for (int i = 0; i < MAX_DIRS; i++) {
      if (dirs[i].exists) {
        Serial.print(dirs[i].name);
        Serial.println(F("/"));
      }
    }
  }
  
  // Show files in current directory
  bool found = false;
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].exists && files[i].parentDir == currentDir) {
      Serial.print(files[i].name);
      Serial.print(F(" ("));
      Serial.print(strlen(files[i].content));
      Serial.println(F("B)"));
      found = true;
    }
  }
  
  if (!found && currentDir != 0) {
    Serial.println(F("(empty)"));
  }
}

void mkdir(char* name) {
  if (currentDir != 0) {
    Serial.println(F("mkdir only at root"));
    return;
  }
  
  if (strlen(name) == 0 || strlen(name) > 7) {
    Serial.println(F("Invalid name"));
    return;
  }
  
  for (int i = 0; i < MAX_DIRS; i++) {
    if (!dirs[i].exists) {
      strcpy(dirs[i].name, name);
      dirs[i].exists = true;
      Serial.print(F("Created: "));
      Serial.println(name);
      return;
    }
  }
  Serial.println(F("No space"));
}

void cd(char* name) {
  if (strcmp(name, "/") == 0) {
    currentDir = 0;
    return;
  }
  
  if (strcmp(name, "..") == 0) {
    currentDir = 0;
    return;
  }
  
  if (currentDir == 0) {
    for (int i = 0; i < MAX_DIRS; i++) {
      if (dirs[i].exists && strcmp(dirs[i].name, name) == 0) {
        currentDir = i + 1;
        return;
      }
    }
  }
  
  Serial.println(F("Directory not found"));
}

void createFile(char* name) {
  if (strlen(name) == 0 || strlen(name) > 11) {
    Serial.println(F("Invalid filename"));
    return;
  }
  
  for (int i = 0; i < MAX_FILES; i++) {
    if (!files[i].exists) {
      strcpy(files[i].name, name);
      strcpy(files[i].content, "");
      files[i].parentDir = currentDir;
      files[i].exists = true;
      Serial.print(F("Created: "));
      Serial.println(name);
      return;
    }
  }
  Serial.println(F("No space"));
}

void deleteFile(char* name) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].exists && strcmp(files[i].name, name) == 0 && 
        files[i].parentDir == currentDir) {
      files[i].exists = false;
      Serial.print(F("Deleted: "));
      Serial.println(name);
      return;
    }
  }
  Serial.println(F("File not found"));
}

void cat(char* name) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].exists && strcmp(files[i].name, name) == 0 && 
        files[i].parentDir == currentDir) {
      Serial.println(files[i].content);
      return;
    }
  }
  Serial.println(F("File not found"));
}

void handleEcho(char* text) {
  // Simple echo - check for redirection
  char* redirect = strstr(text, " > ");
  if (redirect) {
    *redirect = '\0'; // Split at >
    char* filename = redirect + 3;
    
    // Find or create file
    for (int i = 0; i < MAX_FILES; i++) {
      if (files[i].exists && strcmp(files[i].name, filename) == 0 && 
          files[i].parentDir == currentDir) {
        strncpy(files[i].content, text, MAX_CONTENT_LEN - 1);
        files[i].content[MAX_CONTENT_LEN - 1] = '\0';
        Serial.println(F("Written to file"));
        return;
      }
    }
    
    // Create new file
    for (int i = 0; i < MAX_FILES; i++) {
      if (!files[i].exists) {
        strcpy(files[i].name, filename);
        strncpy(files[i].content, text, MAX_CONTENT_LEN - 1);
        files[i].content[MAX_CONTENT_LEN - 1] = '\0';
        files[i].parentDir = currentDir;
        files[i].exists = true;
        Serial.println(F("File created and written"));
        return;
      }
    }
    Serial.println(F("No space for file"));
  } else {
    Serial.println(text);
  }
}

void diskUsage() {
  Serial.print(F("Files: "));
  Serial.print(countFiles());
  Serial.print(F("/"));
  Serial.print(MAX_FILES);
  Serial.print(F(" ("));
  Serial.print((countFiles() * 100) / MAX_FILES);
  Serial.println(F("%)"));
  
  Serial.print(F("Dirs: "));
  Serial.print(countDirs());
  Serial.print(F("/"));
  Serial.print(MAX_DIRS);
  Serial.print(F(" ("));
  Serial.print((countDirs() * 100) / MAX_DIRS);
  Serial.println(F("%)"));
  
  int totalSize = 0;
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].exists) {
      totalSize += strlen(files[i].content);
    }
  }
  Serial.print(F("Data: "));
  Serial.print(totalSize);
  Serial.println(F(" bytes"));
}

int countFiles() {
  int count = 0;
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].exists) count++;
  }
  return count;
}

int countDirs() {
  int count = 0;
  for (int i = 0; i < MAX_DIRS; i++) {
    if (dirs[i].exists) count++;
  }
  return count;
}