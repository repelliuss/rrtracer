#pragma once

/// Parses XML description for objects.

#include "scene.hpp"


namespace xml {
int to_scene(Scene &scene, char *xml);
}
