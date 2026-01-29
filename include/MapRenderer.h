// MapRenderer.h - Pip-Boy Style Map Renderer
// Renders map tiles with zoom/pan and GPS marker
#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include <Arduino.h>
#include <lvgl.h>
#include <SD.h>

#define MAP_TILE_SIZE 64
#define MAP_MAX_TILES 16

struct MapTile {
    int16_t x;
    int16_t y;
    lv_img_dsc_t* img;
    bool loaded;
};

class MapRenderer {
public:
    MapRenderer();
    bool begin();
    
    // Map control
    bool loadMap(const char* mapPath);
    void setCenter(float lat, float lon);
    void setZoom(uint8_t zoomLevel);
    void pan(int16_t dx, int16_t dy);
    
    // GPS marker
    void setMarkerPosition(float lat, float lon);
    void setMarkerVisible(bool visible);
    void setAutoCenter(bool enabled);
    
    // Rendering
    void render(lv_obj_t* canvas);
    void applyCRTOverlay(lv_obj_t* canvas);
    
    // Coordinate conversion
    void latLonToPixel(float lat, float lon, int16_t* x, int16_t* y);
    void pixelToLatLon(int16_t x, int16_t y, float* lat, float* lon);
    
private:
    String mapPath;
    float centerLat, centerLon;
    uint8_t zoomLevel;
    int16_t offsetX, offsetY;
    
    float markerLat, markerLon;
    bool markerVisible;
    bool autoCenter;
    
    MapTile tiles[MAP_MAX_TILES];
    int tileCount;
    
    bool loadTile(int16_t x, int16_t y);
    void renderTile(lv_obj_t* canvas, MapTile* tile);
    void renderMarker(lv_obj_t* canvas);
};

#endif // MAP_RENDERER_H
