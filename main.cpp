#include <iostream>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>
#include <vector>
#include <iostream>
#include <cmath> // For pow function
#include <cmath> // Make sure this include is present for std::pow

using namespace std;

// Interpolation function remains the same
glm::vec2 interpolateQuadraticBezier(const glm::vec2& P0, const glm::vec2& P1, const glm::vec2& P2, float t) {
    float one_minus_t = 1.0f - t;
    return one_minus_t * one_minus_t * P0 + 2.0f * one_minus_t * t * P1 + t * t * P2;
}

std::vector<glm::vec2> interpolateQuadraticBezierSegment(
    const glm::vec2& start,
    const glm::vec2& control,
    const glm::vec2& end,
    int resolution
) {
    std::vector<glm::vec2> points;
    for (int step = 0; step <= resolution; ++step) {
        float t = step / static_cast<float>(resolution);
        glm::vec2 point = interpolateQuadraticBezier(start, control, end, t);
        points.push_back(point);
    }
    return points;
}

void printPoly(std::vector<glm::vec2> poly) {
    std::cout << "[";
    for (auto vec: poly) {
        std::cout << "[" << vec.x << ", " << vec.y << "]";
    }
    std::cout << "]" << std::endl;
}

std::vector<glm::vec2> processContour(
    const std::vector<FT_Vector>& outlinePoints,
    const std::vector<char>& outlineTags,
    int contourStartIdx,
    int contourEndIdx,
    int resolution,
    float xOffset
) {
    int currentIndex = contourStartIdx;
    std::vector<glm::vec2> contourPoints;
    while (currentIndex <= contourEndIdx) {

       glm::vec2 start(outlinePoints[currentIndex].x + xOffset, outlinePoints[currentIndex].y);
       contourPoints.push_back(start);
       currentIndex++;
       //std::cout << (FT_CURVE_TAG(outlineTags[currentIndex]) == FT_CURVE_TAG_CONIC) << std::endl;

       std::vector<glm::vec2> controlPoints;
       while (FT_CURVE_TAG(outlineTags[currentIndex]) == FT_CURVE_TAG_CONIC && currentIndex <= contourEndIdx) {
           glm::vec2 control(outlinePoints[currentIndex].x + xOffset, outlinePoints[currentIndex].y);
           controlPoints.push_back(control);
           currentIndex++;
       }

       // If there is an even number of control points, we need to add the midpoint to the list
       if (controlPoints.size() > 0 && controlPoints.size() % 2 == 0) {
           size_t lastControlIndex = controlPoints.size() - 1;
           glm::vec2 midpoint = (controlPoints[lastControlIndex] + controlPoints[lastControlIndex - 1]) * 0.5f;
           controlPoints.insert(controlPoints.end() - 1, midpoint);
       }

       int idx = currentIndex;
       if (idx > contourEndIdx)
           idx = contourStartIdx;
       glm::vec2 end(outlinePoints[idx].x + xOffset, outlinePoints[idx].y);

       // Interpolate each segment defined by two on-curve points and a series of control points
       for (size_t i = 0; i < controlPoints.size(); i += 2) {
           glm::vec2 control = controlPoints[i];
           glm::vec2 onCurve = (i + 1 < controlPoints.size()) ? controlPoints[i + 1] : end;
           //printPoly({start, control, onCurve});
           std::vector<glm::vec2> bezierPoints = interpolateQuadraticBezierSegment(start, control, onCurve, resolution);
           std::cout << "bezier: ";
           printPoly(bezierPoints);
           std::cout << std::endl;
           contourPoints.insert(contourPoints.end(), bezierPoints.begin(), bezierPoints.end());
           start = onCurve;
       }
    }
    return contourPoints;
}

std::vector<glm::vec2> processOutline(const FT_Outline& outline, int resolution, float xOffset) {
    std::vector<glm::vec2> points;
    std::vector<FT_Vector> outlinePoints(outline.points, outline.points + outline.n_points);
    std::vector<char> outlineTags(outline.tags, outline.tags + outline.n_points);

    int contourStartIdx = 0;
    for (int contourIndex = 0; contourIndex < outline.n_contours; ++contourIndex) {
        int contourEndIdx = outline.contours[contourIndex];
        std::cout << contourStartIdx << " " << contourEndIdx << std::endl;
        std::vector<glm::vec2> contourPoints = processContour(outlinePoints, outlineTags, contourStartIdx, contourEndIdx, resolution, xOffset);
        points.insert(points.end(), contourPoints.begin(), contourPoints.end());
        contourStartIdx = contourEndIdx+1;
    }

    return points;
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

    float xOffset = 0; // Initialize the horizontal offset
    for (char c : text) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }

        // Convert glyph outline to polygons with the current offset
        result.push_back(processOutline(face->glyph->outline, 4, xOffset));

        // Update offsetX by the advance width of the current glyph
        xOffset += face->glyph->advance.x; // Convert from 26.6 fixed-point to integer pixels
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return result;
}

int main(int argc, char *argv[]) {

    auto polys = textToPolygons("OpenSans-Regular.ttf", "i");

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
