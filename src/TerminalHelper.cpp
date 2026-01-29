// TerminalHelper.cpp - Shell Terminal Implementation
#include "TerminalHelper.h"
#include "FileBrowser.h"
#include "GPSHelper.h"
#include <SD.h>
#include <M5Unified.h>

extern FileBrowser fileBrowser;
extern GPSHelper gpsHelper;

TerminalHelper::TerminalHelper() : historyIndex(-1) {
    addOutput("Pip-Boy Terminal v1.0");
    addOutput("Type 'help' for commands");
    addOutput("");
}

void TerminalHelper::addCharacter(char c) {
    if (currentInput.length() < TERM_MAX_COMMAND_LEN) {
        currentInput += c;
    }
}

void TerminalHelper::backspace() {
    if (currentInput.length() > 0) {
        currentInput.remove(currentInput.length() - 1);
    }
}

void TerminalHelper::execute() {
    if (currentInput.length() == 0) return;
    
    // Add to history
    commandHistory.push_back(currentInput);
    if (commandHistory.size() > 20) {
        commandHistory.erase(commandHistory.begin());
    }
    historyIndex = commandHistory.size();
    
    // Echo command
    addOutput("> " + currentInput);
    
    // Execute
    executeCommand(currentInput);
    
    // Clear input
    currentInput = "";
}

String TerminalHelper::getCurrentInput() {
    return currentInput;
}

String TerminalHelper::getOutput() {
    return outputBuffer;
}

std::vector<String> TerminalHelper::getOutputLines() {
    return outputLines;
}

void TerminalHelper::clearOutput() {
    outputBuffer = "";
    outputLines.clear();
}

void TerminalHelper::historyUp() {
    if (historyIndex > 0 && commandHistory.size() > 0) {
        historyIndex--;
        currentInput = commandHistory[historyIndex];
    }
}

void TerminalHelper::historyDown() {
    if (historyIndex < (int)commandHistory.size() - 1) {
        historyIndex++;
        currentInput = commandHistory[historyIndex];
    } else if (historyIndex == (int)commandHistory.size() - 1) {
        historyIndex = commandHistory.size();
        currentInput = "";
    }
}

void TerminalHelper::executeCommand(String cmd) {
    cmd.trim();
    
    // Split command and args
    int spacePos = cmd.indexOf(' ');
    String command = (spacePos > 0) ? cmd.substring(0, spacePos) : cmd;
    String args = (spacePos > 0) ? cmd.substring(spacePos + 1) : "";
    
    command.toLowerCase();
    
    // Execute command
    if (command == "help") {
        cmd_help();
    } else if (command == "ls") {
        cmd_ls(args);
    } else if (command == "cat") {
        cmd_cat(args);
    } else if (command == "mem") {
        cmd_mem();
    } else if (command == "sysinfo") {
        cmd_sysinfo();
    } else if (command == "uptime") {
        cmd_uptime();
    } else if (command == "clear") {
        cmd_clear();
    } else if (command == "sd") {
        cmd_sd();
    } else if (command == "gps") {
        cmd_gps();
    } else if (command == "echo") {
        cmd_echo(args);
    } else {
        addOutput("Unknown command: " + command);
        addOutput("Type 'help' for commands");
    }
    
    addOutput("");
}

void TerminalHelper::addOutput(String line) {
    outputLines.push_back(line);
    outputBuffer += line + "\n";
    
    // Keep buffer size manageable
    if (outputLines.size() > TERM_BUFFER_SIZE) {
        outputLines.erase(outputLines.begin());
    }
    
    if (outputBuffer.length() > TERM_MAX_OUTPUT_LEN) {
        outputBuffer = outputBuffer.substring(outputBuffer.length() - TERM_MAX_OUTPUT_LEN);
    }
}

void TerminalHelper::cmd_help() {
    addOutput("Available commands:");
    addOutput("  ls [path]    - List directory");
    addOutput("  cat <file>   - Display file");
    addOutput("  mem          - Memory info");
    addOutput("  sysinfo      - System info");
    addOutput("  uptime       - System uptime");
    addOutput("  sd           - SD card stats");
    addOutput("  gps          - GPS status");
    addOutput("  echo <text>  - Echo text");
    addOutput("  clear        - Clear screen");
    addOutput("  help         - Show this help");
}

void TerminalHelper::cmd_ls(String args) {
    String path = (args.length() > 0) ? args : fileBrowser.getCurrentPath();
    
    File dir = SD.open(path.c_str());
    if (!dir || !dir.isDirectory()) {
        addOutput("Error: Invalid directory");
        return;
    }
    
    addOutput("Directory: " + path);
    
    File file = dir.openNextFile();
    int count = 0;
    while (file) {
        String name = String(file.name());
        if (file.isDirectory()) {
            addOutput("  [DIR]  " + name);
        } else {
            char sizeStr[32];
            snprintf(sizeStr, sizeof(sizeStr), "%8d", (int)file.size());
            addOutput("  " + String(sizeStr) + "  " + name);
        }
        count++;
        file = dir.openNextFile();
    }
    
    addOutput("Total: " + String(count) + " items");
}

void TerminalHelper::cmd_cat(String args) {
    if (args.length() == 0) {
        addOutput("Usage: cat <filename>");
        return;
    }
    
    String content = fileBrowser.readFilePreview(args.c_str(), 512);
    if (content.length() == 0) {
        addOutput("Error: Cannot read file");
        return;
    }
    
    // Split into lines
    int start = 0;
    while (start < content.length()) {
        int end = content.indexOf('\n', start);
        if (end < 0) end = content.length();
        
        String line = content.substring(start, end);
        addOutput(line);
        
        start = end + 1;
    }
}

void TerminalHelper::cmd_mem() {
    char buf[128];
    
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t totalHeap = ESP.getHeapSize();
    uint32_t freePsram = ESP.getFreePsram();
    uint32_t totalPsram = ESP.getPsramSize();
    
    addOutput("Memory Status:");
    snprintf(buf, sizeof(buf), "  Heap:  %u / %u KB (%.1f%% free)",
        freeHeap / 1024, totalHeap / 1024, (float)freeHeap / totalHeap * 100);
    addOutput(buf);
    
    snprintf(buf, sizeof(buf), "  PSRAM: %u / %u KB (%.1f%% free)",
        freePsram / 1024, totalPsram / 1024, (float)freePsram / totalPsram * 100);
    addOutput(buf);
}

void TerminalHelper::cmd_sysinfo() {
    char buf[128];
    
    addOutput("System Information:");
    addOutput("  Device: M5Stack Cardputer ADV");
    addOutput("  Chip: ESP32-S3");
    
    snprintf(buf, sizeof(buf), "  CPU: %u MHz", ESP.getCpuFreqMHz());
    addOutput(buf);
    
    snprintf(buf, sizeof(buf), "  Flash: %u MB", ESP.getFlashChipSize() / (1024 * 1024));
    addOutput(buf);
    
    addOutput("  Firmware: v1.0.3");
    addOutput("  Build: 2026-01-28");
}

void TerminalHelper::cmd_uptime() {
    uint32_t seconds = millis() / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;
    
    char buf[64];
    snprintf(buf, sizeof(buf), "Uptime: %ud %uh %um %us",
        days, hours % 24, minutes % 60, seconds % 60);
    addOutput(buf);
}

void TerminalHelper::cmd_clear() {
    clearOutput();
}

void TerminalHelper::cmd_sd() {
    char buf[128];
    
    uint64_t total = SD.totalBytes();
    uint64_t used = SD.usedBytes();
    uint64_t free = total - used;
    
    addOutput("SD Card Status:");
    snprintf(buf, sizeof(buf), "  Total: %llu MB", total / (1024 * 1024));
    addOutput(buf);
    
    snprintf(buf, sizeof(buf), "  Used:  %llu MB (%.1f%%)",
        used / (1024 * 1024), (float)used / total * 100);
    addOutput(buf);
    
    snprintf(buf, sizeof(buf), "  Free:  %llu MB", free / (1024 * 1024));
    addOutput(buf);
}

void TerminalHelper::cmd_gps() {
    if (!gpsHelper.isAvailable()) {
        addOutput("GPS: Not available");
        return;
    }
    
    GPSData* data = gpsHelper.getData();
    char buf[128];
    
    addOutput("GPS Status:");
    addOutput(data->valid ? "  Fix: VALID" : "  Fix: INVALID");
    
    snprintf(buf, sizeof(buf), "  Satellites: %d", data->satellites);
    addOutput(buf);
    
    if (data->valid) {
        snprintf(buf, sizeof(buf), "  Lat: %.6f %c", fabs(data->latitude), data->latDirection);
        addOutput(buf);
        
        snprintf(buf, sizeof(buf), "  Lon: %.6f %c", fabs(data->longitude), data->lonDirection);
        addOutput(buf);
        
        snprintf(buf, sizeof(buf), "  Alt: %.1f m", data->altitude);
        addOutput(buf);
        
        snprintf(buf, sizeof(buf), "  Speed: %.1f km/h", data->speed);
        addOutput(buf);
    }
}

void TerminalHelper::cmd_echo(String args) {
    addOutput(args);
}
