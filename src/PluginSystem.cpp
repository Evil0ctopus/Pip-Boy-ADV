// PluginSystem.cpp - Plugin System Implementation
#include "PluginSystem.h"
#include <SD.h>
#include <ArduinoJson.h>

PluginSystem::PluginSystem() : initialized(false) {}

bool PluginSystem::begin() {
    // Create plugins directory if it doesn't exist
    if (!SD.exists(PLUGIN_DIR)) {
        SD.mkdir(PLUGIN_DIR);
    }
    
    initialized = true;
    Serial.println("PluginSystem: Initialized");
    
    // Scan for plugins
    scanPlugins();
    
    return true;
}

bool PluginSystem::scanPlugins() {
    if (!initialized) return false;
    
    plugins.clear();
    
    File dir = SD.open(PLUGIN_DIR);
    if (!dir || !dir.isDirectory()) {
        Serial.println("PluginSystem: Plugin directory not found");
        return false;
    }
    
    File file = dir.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            String pluginPath = String(PLUGIN_DIR) + "/" + file.name();
            String manifestPath = pluginPath + "/plugin.json";
            
            if (SD.exists(manifestPath.c_str())) {
                Plugin plugin;
                if (parseManifest(manifestPath.c_str(), &plugin)) {
                    plugin.path = pluginPath;
                    plugin.enabled = false;
                    plugin.panel = nullptr;
                    
                    plugins.push_back(plugin);
                    Serial.printf("PluginSystem: Found plugin: %s v%s\n",
                        plugin.name.c_str(), plugin.version.c_str());
                }
            }
        }
        file = dir.openNextFile();
    }
    
    Serial.printf("PluginSystem: Discovered %d plugins\n", plugins.size());
    return true;
}

bool PluginSystem::parseManifest(const char* path, Plugin* plugin) {
    File file = SD.open(path, FILE_READ);
    if (!file) return false;
    
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.printf("PluginSystem: Failed to parse %s\n", path);
        return false;
    }
    
    plugin->name = doc["name"].as<String>();
    plugin->version = doc["version"].as<String>();
    plugin->author = doc["author"] | "Unknown";
    plugin->description = doc["description"] | "";
    
    if (doc.containsKey("script")) {
        plugin->scriptPath = doc["script"].as<String>();
    }
    
    return plugin->name.length() > 0;
}

std::vector<Plugin*> PluginSystem::getPlugins() {
    std::vector<Plugin*> ptrs;
    for (size_t i = 0; i < plugins.size(); i++) {
        ptrs.push_back(&plugins[i]);
    }
    return ptrs;
}

int PluginSystem::getPluginCount() {
    return plugins.size();
}

Plugin* PluginSystem::getPlugin(int index) {
    if (index < 0 || index >= (int)plugins.size()) return nullptr;
    return &plugins[index];
}

Plugin* PluginSystem::getPluginByName(const char* name) {
    for (size_t i = 0; i < plugins.size(); i++) {
        if (plugins[i].name.equals(name)) {
            return &plugins[i];
        }
    }
    return nullptr;
}

bool PluginSystem::loadPlugin(Plugin* plugin) {
    if (!plugin) return false;
    
    Serial.printf("PluginSystem: Loading %s...\n", plugin->name.c_str());
    
    // Load script if available
    if (plugin->scriptPath.length() > 0) {
        loadScript(plugin);
    }
    
    plugin->enabled = true;
    return true;
}

bool PluginSystem::unloadPlugin(Plugin* plugin) {
    if (!plugin) return false;
    
    if (plugin->panel) {
        lv_obj_del(plugin->panel);
        plugin->panel = nullptr;
    }
    
    plugin->enabled = false;
    Serial.printf("PluginSystem: Unloaded %s\n", plugin->name.c_str());
    
    return true;
}

bool PluginSystem::enablePlugin(Plugin* plugin) {
    return loadPlugin(plugin);
}

bool PluginSystem::disablePlugin(Plugin* plugin) {
    return unloadPlugin(plugin);
}

lv_obj_t* PluginSystem::createPluginPanel(Plugin* plugin, lv_obj_t* parent) {
    if (!plugin || !parent) return nullptr;
    
    // Create simple panel for plugin
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_set_size(panel, 220, 100);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x004400), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    
    // Plugin name
    lv_obj_t* title = lv_label_create(panel);
    lv_label_set_text(title, plugin->name.c_str());
    lv_obj_set_style_text_color(title, lv_color_hex(0x00FF00), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 5, 5);
    
    // Version
    lv_obj_t* version = lv_label_create(panel);
    lv_label_set_text(version, ("v" + plugin->version).c_str());
    lv_obj_set_style_text_color(version, lv_color_hex(0x00CC00), 0);
    lv_obj_align(version, LV_ALIGN_TOP_RIGHT, -5, 5);
    
    // Description
    if (plugin->description.length() > 0) {
        lv_obj_t* desc = lv_label_create(panel);
        lv_label_set_text(desc, plugin->description.c_str());
        lv_label_set_long_mode(desc, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(desc, 210);
        lv_obj_set_style_text_color(desc, lv_color_hex(0x00AA00), 0);
        lv_obj_align(desc, LV_ALIGN_TOP_LEFT, 5, 25);
    }
    
    // Author
    lv_obj_t* author = lv_label_create(panel);
    lv_label_set_text(author, ("by " + plugin->author).c_str());
    lv_obj_set_style_text_color(author, lv_color_hex(0x004400), 0);
    lv_obj_align(author, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    
    plugin->panel = panel;
    return panel;
}

bool PluginSystem::loadScript(Plugin* plugin) {
    // Placeholder for script execution
    // Could implement Lua, JavaScript, or simple command scripting
    Serial.printf("PluginSystem: Script execution not yet implemented for %s\n",
        plugin->name.c_str());
    return false;
}
