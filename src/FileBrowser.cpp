// FileBrowser.cpp - SD Card File Browser Implementation
#include "FileBrowser.h"
#include "SDHelper.h"

FileBrowser::FileBrowser() : currentPath("/"), initialized(false) {}

bool FileBrowser::begin() {
    if (!SDHelper::isReady()) {
        Serial.println("FileBrowser: SD card initialization failed");
        return false;
    }
    
    initialized = true;
    Serial.println("FileBrowser: Initialized");
    return true;
}

bool FileBrowser::changeDirectory(const char* path) {
    if (!initialized) return false;
    
    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        Serial.printf("FileBrowser: Invalid directory: %s\n", path);
        return false;
    }
    dir.close();
    
    currentPath = String(path);
    if (!currentPath.endsWith("/")) {
        currentPath += "/";
    }
    
    Serial.printf("FileBrowser: Changed to %s\n", currentPath.c_str());
    return true;
}

String FileBrowser::getCurrentPath() {
    return currentPath;
}

std::vector<FileEntry> FileBrowser::listDirectory() {
    std::vector<FileEntry> entries;
    
    if (!initialized) return entries;
    
    File dir = SD.open(currentPath.c_str());
    if (!dir || !dir.isDirectory()) {
        return entries;
    }
    
    File file = dir.openNextFile();
    while (file) {
        FileEntry entry;
        entry.name = String(file.name());
        entry.path = combinePath(currentPath.c_str(), file.name());
        entry.isDirectory = file.isDirectory();
        entry.size = file.size();
        
        entries.push_back(entry);
        file = dir.openNextFile();
    }
    
    dir.close();
    return entries;
}

bool FileBrowser::goUp() {
    if (currentPath == "/" || currentPath == "") return false;
    
    // Remove trailing slash
    String path = currentPath;
    if (path.endsWith("/")) {
        path = path.substring(0, path.length() - 1);
    }
    
    // Find last slash
    int lastSlash = path.lastIndexOf('/');
    if (lastSlash < 0) {
        currentPath = "/";
    } else if (lastSlash == 0) {
        currentPath = "/";
    } else {
        currentPath = path.substring(0, lastSlash + 1);
    }
    
    Serial.printf("FileBrowser: Up to %s\n", currentPath.c_str());
    return true;
}

bool FileBrowser::deleteFile(const char* filename) {
    if (!initialized) return false;
    
    String path = combinePath(currentPath.c_str(), filename);
    
    if (SD.remove(path.c_str())) {
        Serial.printf("FileBrowser: Deleted %s\n", path.c_str());
        return true;
    }
    
    Serial.printf("FileBrowser: Failed to delete %s\n", path.c_str());
    return false;
}

bool FileBrowser::renameFile(const char* oldName, const char* newName) {
    if (!initialized) return false;
    
    String oldPath = combinePath(currentPath.c_str(), oldName);
    String newPath = combinePath(currentPath.c_str(), newName);
    
    if (SD.rename(oldPath.c_str(), newPath.c_str())) {
        Serial.printf("FileBrowser: Renamed %s -> %s\n", oldPath.c_str(), newPath.c_str());
        return true;
    }
    
    Serial.printf("FileBrowser: Failed to rename %s\n", oldPath.c_str());
    return false;
}

bool FileBrowser::createDirectory(const char* dirname) {
    if (!initialized) return false;
    
    String path = combinePath(currentPath.c_str(), dirname);
    
    if (SD.mkdir(path.c_str())) {
        Serial.printf("FileBrowser: Created directory %s\n", path.c_str());
        return true;
    }
    
    Serial.printf("FileBrowser: Failed to create directory %s\n", path.c_str());
    return false;
}

String FileBrowser::readFilePreview(const char* filename, size_t maxBytes) {
    if (!initialized) return "";
    
    String path = combinePath(currentPath.c_str(), filename);
    
    File file = SD.open(path.c_str(), FILE_READ);
    if (!file) {
        return "";
    }
    
    String content = "";
    size_t bytesRead = 0;
    
    while (file.available() && bytesRead < maxBytes) {
        char c = file.read();
        content += c;
        bytesRead++;
    }
    
    file.close();
    return content;
}

size_t FileBrowser::getFileSize(const char* filename) {
    if (!initialized) return 0;
    
    String path = combinePath(currentPath.c_str(), filename);
    
    File file = SD.open(path.c_str(), FILE_READ);
    if (!file) return 0;
    
    size_t size = file.size();
    file.close();
    
    return size;
}

bool FileBrowser::fileExists(const char* filename) {
    if (!initialized) return false;
    
    String path = combinePath(currentPath.c_str(), filename);
    return SD.exists(path.c_str());
}

bool FileBrowser::isDirectory(const char* path) {
    if (!initialized) return false;
    
    File file = SD.open(path);
    if (!file) return false;
    
    bool isDir = file.isDirectory();
    file.close();
    
    return isDir;
}

uint64_t FileBrowser::getTotalSpace() {
    if (!initialized) return 0;
    return SD.totalBytes();
}

uint64_t FileBrowser::getUsedSpace() {
    if (!initialized) return 0;
    return SD.usedBytes();
}

uint64_t FileBrowser::getFreeSpace() {
    if (!initialized) return 0;
    return SD.totalBytes() - SD.usedBytes();
}

String FileBrowser::combinePath(const char* base, const char* name) {
    String path = String(base);
    
    if (!path.endsWith("/")) {
        path += "/";
    }
    
    path += String(name);
    
    return path;
}
