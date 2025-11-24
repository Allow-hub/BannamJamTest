#pragma once
#include <Siv3D.hpp>
#include "../../Stage/StageTypes.h"

namespace Jam::Domain::Editor
{
    enum class StageEditorMode {
        Select,
        Place,
        Delete
    };

    struct StageEditorConfig {
        int gridSize = 50;
        bool snapToGrid = true;
        bool showGrid = true;
        double cameraSpeed = 5.0;
    };
}