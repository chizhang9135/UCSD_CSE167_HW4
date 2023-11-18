#pragma once

#include <vector>
#include <string>
#include "MyCamera.h"
#include "matrix.h"


void hw_3_1(const std::vector<std::string> &params);
void hw_3_2(const std::vector<std::string> &params);
void hw_3_3(const std::vector<std::string> &params);
void hw_3_4(const std::vector<std::string> &params);
void hw_3_5(const std::vector<std::string> &params);

glm::mat4 convertToGLMmat4(const Matrix4x4f& m);

