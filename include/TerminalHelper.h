// TerminalHelper.h - Simple Shell Terminal
// Commands: ls, cat, mem, sysinfo, uptime, help, clear
#ifndef TERMINAL_HELPER_H
#define TERMINAL_HELPER_H

#include <Arduino.h>
#include <vector>

#define TERM_BUFFER_SIZE 100
#define TERM_MAX_COMMAND_LEN 128
#define TERM_MAX_OUTPUT_LEN 2048

class TerminalHelper {
public:
    TerminalHelper();
    
    // Input handling
    void addCharacter(char c);
    void backspace();
    void execute();
    String getCurrentInput();
    
    // Output buffer
    String getOutput();
    std::vector<String> getOutputLines();
    void clearOutput();
    
    // History
    void historyUp();
    void historyDown();
    
private:
    String currentInput;
    String outputBuffer;
    std::vector<String> outputLines;
    std::vector<String> commandHistory;
    int historyIndex;
    
    void executeCommand(String cmd);
    void addOutput(String line);
    
    // Commands
    void cmd_ls(String args);
    void cmd_cat(String args);
    void cmd_mem();
    void cmd_sysinfo();
    void cmd_uptime();
    void cmd_help();
    void cmd_clear();
    void cmd_sd();
    void cmd_gps();
    void cmd_echo(String args);
};

#endif // TERMINAL_HELPER_H
