// FileBrowser.h - SD Card File Browser
// Full file management: browse, open, delete, rename, mkdir
#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <Arduino.h>
#include <SD.h>
#include <vector>

#define MAX_PATH_LEN 128
#define MAX_FILE_PREVIEW 512

struct FileEntry {
    String name;
    String path;
    bool isDirectory;
    size_t size;
};

class FileBrowser {
public:
    FileBrowser();
    bool begin();
    
    // Navigation
    bool changeDirectory(const char* path);
    String getCurrentPath();
    std::vector<FileEntry> listDirectory();
    bool goUp();
    
    // File operations
    bool deleteFile(const char* filename);
    bool renameFile(const char* oldName, const char* newName);
    bool createDirectory(const char* dirname);
    String readFilePreview(const char* filename, size_t maxBytes = MAX_FILE_PREVIEW);
    
    // Info
    size_t getFileSize(const char* filename);
    bool fileExists(const char* filename);
    bool isDirectory(const char* path);
    
    // SD card stats
    uint64_t getTotalSpace();
    uint64_t getUsedSpace();
    uint64_t getFreeSpace();
    
private:
    String currentPath;
    bool initialized;
    
    String combinePath(const char* base, const char* name);
};

#endif // FILE_BROWSER_H
