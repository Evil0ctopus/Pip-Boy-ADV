// PluginSystem.h - Modular Plugin Loading System
// Loads plugins from SD card with JSON manifest
#ifndef PLUGIN_SYSTEM_H
#define PLUGIN_SYSTEM_H

#include <Arduino.h>
#include <lvgl.h>
#include <vector>

#define MAX_PLUGINS 16
#define PLUGIN_DIR "/plugins"

struct Plugin {
    String name;
    String version;
    String author;
    String description;
    String path;
    bool enabled;
    lv_obj_t* panel;
    
    // Optional script path
    String scriptPath;
};

class PluginSystem {
public:
    PluginSystem();
    bool begin();
    
    // Plugin discovery
    bool scanPlugins();
    std::vector<Plugin*> getPlugins();
    int getPluginCount();
    Plugin* getPlugin(int index);
    Plugin* getPluginByName(const char* name);
    
    // Plugin management
    bool loadPlugin(Plugin* plugin);
    bool unloadPlugin(Plugin* plugin);
    bool enablePlugin(Plugin* plugin);
    bool disablePlugin(Plugin* plugin);
    
    // UI creation
    lv_obj_t* createPluginPanel(Plugin* plugin, lv_obj_t* parent);
    
private:
    std::vector<Plugin> plugins;
    bool initialized;
    
    bool parseManifest(const char* path, Plugin* plugin);
    bool loadScript(Plugin* plugin);
};

#endif // PLUGIN_SYSTEM_H
