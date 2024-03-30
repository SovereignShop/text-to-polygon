#include <iostream>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>
#include <vector>
#include <iostream>

using namespace std;

void convertOutlineToPolygons(FT_Outline* outline, std::vector<std::vector<glm::vec2>>& polygons, float offsetX) {
    for (int i = 0; i < outline->n_contours; ++i) {
        int contour_start = (i == 0) ? 0 : outline->contours[i - 1] + 1;
        int contour_end = outline->contours[i];

        std::vector<glm::vec2> polygon;
        for (int pointIndex = contour_start; pointIndex <= contour_end; ++pointIndex) {
            FT_Vector* point = &outline->points[pointIndex];
            // Apply the offsetX to each point
            polygon.push_back(glm::vec2(point->x + offsetX, point->y));
        }
        polygons.push_back(polygon);
    }
}

// Update the main function to include glyph spacing
std::vector<std::vector<glm::vec2>> textToPolygons(const std::string& fontFile, const std::string& text) {
    std::vector<std::vector<glm::vec2>> result;
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Could not init FreeType Library" << std::endl;
        return result;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
        std::cerr << "Failed to load font" << std::endl;
        FT_Done_FreeType(ft);
        return result;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    float offsetX = 0; // Initialize the horizontal offset
    for (char c : text) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }

        // Convert glyph outline to polygons with the current offset
        convertOutlineToPolygons(&face->glyph->outline, result, offsetX);

        // Update offsetX by the advance width of the current glyph
        offsetX += face->glyph->advance.x; // Convert from 26.6 fixed-point to integer pixels
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return result;
}

int main(int argc, char *argv[]) {

    auto polys = textToPolygons("OpenSans-Regular.ttf", "Hello World");
    std::cout << "polys: " << polys.size() << std::endl;

    std::cout << "[";
    for (auto poly : polys)  {
        std::cout << "[";
        for (auto vec: poly) {
            std::cout << "[" << vec.x << ", " << vec.y << "]";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "]" << std::endl;

    return 0;
}
