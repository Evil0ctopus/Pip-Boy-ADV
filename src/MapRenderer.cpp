// MapRenderer.cpp - Map Renderer Implementation
#include "MapRenderer.h"
#include "ui_theme.h"

MapRenderer::MapRenderer() : centerLat(0.0), centerLon(0.0), zoomLevel(12),
    offsetX(0), offsetY(0), markerLat(0.0), markerLon(0.0),
    markerVisible(false), autoCenter(false), tileCount(0) {}

bool MapRenderer::begin() {
    Serial.println("MapRenderer: Initialized");
    return true;
}

bool MapRenderer::loadMap(const char* path) {
    mapPath = String(path);
    Serial.printf("MapRenderer: Loaded map from %s\n", path);
    return true;
}

void MapRenderer::setCenter(float lat, float lon) {
    centerLat = lat;
    centerLon = lon;
}

void MapRenderer::setZoom(uint8_t level) {
    if (level < 1) level = 1;
    if (level > 20) level = 20;
    zoomLevel = level;
}

void MapRenderer::pan(int16_t dx, int16_t dy) {
    offsetX += dx;
    offsetY += dy;
}

void MapRenderer::setMarkerPosition(float lat, float lon) {
    markerLat = lat;
    markerLon = lon;
    
    if (autoCenter) {
        setCenter(lat, lon);
    }
}

void MapRenderer::setMarkerVisible(bool visible) {
    markerVisible = visible;
}

void MapRenderer::setAutoCenter(bool enabled) {
    autoCenter = enabled;
}

void MapRenderer::render(lv_obj_t* canvas) {
    if (!canvas) return;
    
    // Clear canvas
    lv_canvas_fill_bg(canvas, PIPBOY_BLACK, LV_OPA_COVER);
    
    // Draw simple grid as placeholder
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = PIPBOY_GREEN_DARK;
    line_dsc.width = 1;
    
    // Vertical lines
    for (int x = 0; x < 240; x += 20) {
        lv_point_t points[2] = {{(lv_coord_t)x, 0}, {(lv_coord_t)x, 135}};
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
    
    // Horizontal lines
    for (int y = 0; y < 135; y += 20) {
        lv_point_t points[2] = {{0, (lv_coord_t)y}, {240, (lv_coord_t)y}};
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
    
    // Render GPS marker if visible
    if (markerVisible) {
        renderMarker(canvas);
    }
    
    // Apply CRT overlay
    applyCRTOverlay(canvas);
}

void MapRenderer::renderMarker(lv_obj_t* canvas) {
    // Draw simple crosshair marker at center
    int16_t cx = 120;
    int16_t cy = 67;
    
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x00FF00);
    line_dsc.width = 2;
    
    // Crosshair
    lv_point_t h_points[2] = {{(lv_coord_t)(cx - 10), (lv_coord_t)cy}, {(lv_coord_t)(cx + 10), (lv_coord_t)cy}};
    lv_canvas_draw_line(canvas, h_points, 2, &line_dsc);
    
    lv_point_t v_points[2] = {{(lv_coord_t)cx, (lv_coord_t)(cy - 10)}, {(lv_coord_t)cx, (lv_coord_t)(cy + 10)}};
    lv_canvas_draw_line(canvas, v_points, 2, &line_dsc);
    
    // Outer circle using canvas_draw_rect with coordinates
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    rect_dsc.border_color = lv_color_hex(0x00FF00);
    rect_dsc.border_width = 2;
    rect_dsc.radius = LV_RADIUS_CIRCLE;
    
    lv_canvas_draw_rect(canvas, cx - 8, cy - 8, 16, 16, &rect_dsc);
}

void MapRenderer::applyCRTOverlay(lv_obj_t* canvas) {
    // Add scanline effect
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x000000);
    line_dsc.width = 1;
    line_dsc.opa = LV_OPA_10;
    
    for (int y = 0; y < 135; y += 2) {
        lv_point_t points[2] = {{0, (lv_coord_t)y}, {240, (lv_coord_t)y}};
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
}

void MapRenderer::latLonToPixel(float lat, float lon, int16_t* x, int16_t* y) {
    // Simple Mercator projection (simplified for small areas)
    float scale = pow(2, zoomLevel);
    
    *x = (int16_t)((lon - centerLon) * scale * 100) + 120 + offsetX;
    *y = (int16_t)((centerLat - lat) * scale * 100) + 67 + offsetY;
}

void MapRenderer::pixelToLatLon(int16_t x, int16_t y, float* lat, float* lon) {
    float scale = pow(2, zoomLevel);
    
    *lon = centerLon + (x - 120 - offsetX) / (scale * 100);
    *lat = centerLat - (y - 67 - offsetY) / (scale * 100);
}
