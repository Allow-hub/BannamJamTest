#pragma once
#include "StageTypes.h"

namespace Jam::Domain::Stage {
    class IStageLoader {
    public:
        virtual ~IStageLoader() = default;
        virtual bool loadStageData(const String& path, StageInfo& outInfo) = 0;
    };
}