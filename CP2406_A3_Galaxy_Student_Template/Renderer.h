#ifndef RENDERER_H
#define RENDERER_H
#include "Types.h"
#include <string>
#include <vector>
void renderSnapshot(const std::vector<body>& bodies, const std::string& filename, double viewHalfAU);
#endif
