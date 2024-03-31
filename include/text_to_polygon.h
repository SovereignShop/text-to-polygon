// TextToPolygon.h
#ifndef TEXT_TO_POLYGON_H
#define TEXT_TO_POLYGON_H

#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>


using namespace std;

namespace TextToPolygon {

// Declare the interpolateQuadraticBezier function
glm::vec2 interpolateQuadraticBezier(const glm::vec2& P0, const glm::vec2& P1, const glm::vec2& P2, float t);

// Declare the interpolateQuadraticBezierSegment function
std::vector<glm::vec2> interpolateQuadraticBezierSegment(const glm::vec2& start, const glm::vec2& control, const glm::vec2& end, int resolution);

// Declare the processContour function
std::vector<glm::vec2> processContour(const std::vector<FT_Vector>& outlinePoints, const std::vector<char>& outlineTags, int contourStartIdx, int contourEndIdx, int resolution, float xOffset);

// Declare the processOutline function
std::vector<std::vector<glm::vec2>> processOutline(const FT_Outline& outline, int resolution, float xOffset);

// Declare the textToPolygons function
std::vector<std::vector<glm::vec2>> textToPolygons(const std::string& fontFile, const std::string& text, u_int32_t pixelHeight, int interpRes);

} // namespace TextToPolygon

#endif // TEXT_TO_POLYGON_H
