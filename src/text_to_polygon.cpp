#include <iostream>
#include <ft2build.h>
#include <sys/types.h>
#include FT_FREETYPE_H
#include "linalg.h"
#include <vector>
#include <iostream>
#include <cmath> // For pow function

using namespace std;
using vec2 = linalg::vec<double, 2>;
using vec3 = linalg::vec<double, 3>;
using vec4 = linalg::vec<double, 4>;

namespace TextToPolygon {

vec2 interpolateQuadraticBezier(const vec2& P0, const vec2& P1, const vec2& P2, double t) {
    double one_minus_t = 1.0d - t;
    return one_minus_t * one_minus_t * P0 + 2.0d * one_minus_t * t * P1 + t * t * P2;
}

std::vector<vec2> interpolateQuadraticBezierSegment(
    const vec2& start,
    const vec2& control,
    const vec2& end,
    int resolution
) {
    std::vector<vec2> points;
    for (int step = 0; step <= resolution; ++step) {
        double t = step / static_cast<double>(resolution);
        vec2 point = interpolateQuadraticBezier(start, control, end, t);
        points.push_back(point);
    }
    return points;
}

std::vector<vec2> processContour(
    const std::vector<FT_Vector>& outlinePoints,
    const std::vector<char>& outlineTags,
    int contourStartIdx,
    int contourEndIdx,
    int resolution,
    double xOffset) {

    int currentIndex = contourStartIdx;
    std::vector<vec2> contourPoints;
    while (currentIndex <= contourEndIdx) {

       vec2 start(outlinePoints[currentIndex].x + xOffset, outlinePoints[currentIndex].y);
       contourPoints.push_back(start);
       currentIndex++;

       std::vector<vec2> controlPoints;
       while (FT_CURVE_TAG(outlineTags[currentIndex]) == FT_CURVE_TAG_CONIC && currentIndex <= contourEndIdx) {
           vec2 control(outlinePoints[currentIndex].x + xOffset, outlinePoints[currentIndex].y);
           controlPoints.push_back(control);
           currentIndex++;
       }

       // Add midpoint if even number of control points
       if (controlPoints.size() > 0 && controlPoints.size() % 2 == 0) {
           size_t lastControlIndex = controlPoints.size() - 1;
           vec2 midpoint = (controlPoints[lastControlIndex] + controlPoints[lastControlIndex - 1]) * 0.5d;
           controlPoints.insert(controlPoints.end() - 1, midpoint);
       }

       int idx = currentIndex;
       if (idx > contourEndIdx)
           idx = contourStartIdx;
       vec2 end(outlinePoints[idx].x + xOffset, outlinePoints[idx].y);

       for (size_t i = 0; i < controlPoints.size(); i += 2) {
           vec2 control = controlPoints[i];
           vec2 onCurve = (i + 1 < controlPoints.size()) ? controlPoints[i + 1] : end;
           std::vector<vec2> bezierPoints = interpolateQuadraticBezierSegment(start, control, onCurve, resolution);
           contourPoints.insert(contourPoints.end(), bezierPoints.begin(), bezierPoints.end());
           start = onCurve;
       }
    }
    return contourPoints;

}

std::vector<std::vector<vec2>> processOutline(const FT_Outline& outline, int resolution, double xOffset) {
    std::vector<std::vector<vec2>> points;
    std::vector<FT_Vector> outlinePoints(outline.points, outline.points + outline.n_points);
    std::vector<char> outlineTags(outline.tags, outline.tags + outline.n_points);

    int contourStartIdx = 0;
    for (int contourIndex = 0; contourIndex < outline.n_contours; ++contourIndex) {
        int contourEndIdx = outline.contours[contourIndex];
        std::vector<vec2> contourPoints = processContour(outlinePoints, outlineTags, contourStartIdx, contourEndIdx, resolution, xOffset);
        points.push_back(contourPoints);
        contourStartIdx = contourEndIdx+1;
    }

    return points;
}


std::vector<std::vector<vec2>> textToPolygons(const std::string& fontFile, const std::string& text, u_int32_t pixelHeight, int interpRes) {
    std::vector<std::vector<vec2>> result;
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

    FT_Set_Pixel_Sizes(face, 0, pixelHeight);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    double xOffset = 0;
    for (char c : text) {
        if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            std::cerr << "Failed to load Glyph" << std::endl;
            continue;
        }

        auto outline = processOutline(face->glyph->outline, interpRes, xOffset);
        result.insert(result.end(), outline.begin(), outline.end());

        xOffset += face->glyph->advance.x;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return result;
}

}

int main(int argc, char *argv[]) {

    auto polys = TextToPolygon::textToPolygons("DejaVuSans.ttf", "abc\ndef", 12, 6);

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
