#pragma once

struct Vector2 {
    float x;
    float y;
};

struct Rectangle {
    float x;
    float y;
    float width;
    float height;
};

struct Texture2D {
    unsigned int id;
    int width;
    int height;
    int mipmaps;
    int format;
};

struct GlyphInfo;

struct Font {
    int baseSize;
    int glyphCount;
    int glyphPadding;
    Texture2D texture;
    Rectangle* recs;
    GlyphInfo* glyphs;
};
