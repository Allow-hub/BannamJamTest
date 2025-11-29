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
}