// TextToPolygon.h
#ifndef TEXT_TO_POLYGON_H
#define TEXT_TO_POLYGON_H

#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H


using namespace std;
using vec2 = linalg::vec<double, 2>;
using vec3 = linalg::vec<double, 3>;
using vec4 = linalg::vec<double, 4>;

namespace TextToPolygon {

vec2 interpolateQuadraticBezier(const vec2& P0, const vec2& P1, const vec2& P2, float t);

std::vector<vec2> interpolateQuadraticBezierSegment(const vec2& start, const vec2& control, const vec2& end, int resolution);

std::vector<vec2> processContour(const std::vector<FT_Vector>& outlinePoints, const std::vector<char>& outlineTags, int contourStartIdx, int contourEndIdx, int resolution, float xOffset);

std::vector<std::vector<vec2>> processOutline(const FT_Outline& outline, int resolution, float xOffset);

std::vector<std::vector<vec2>> textToPolygons(const std::string& fontFile, const std::string& text, u_int32_t pixelHeight, int interpRes);

} // namespace TextToPolygon

#endif // TEXT_TO_POLYGON_H
