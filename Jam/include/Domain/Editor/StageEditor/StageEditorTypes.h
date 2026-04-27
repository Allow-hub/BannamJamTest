#pragma once
#include <Siv3D.hpp>
#include "Domain/Block/BlockTypes.h"

namespace Jam::Domain::Editor
{
    enum class StageEditorMode {
        Select,
        Place,
        Delete
    };
}