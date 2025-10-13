#pragma once
#include "../Domain/Stage/IStageLoader.h"

namespace Jam::Infrastructure {
    class JsonStageLoader : public Domain::Stage::IStageLoader {
    public:
        bool loadStageData(const String& path, Domain::Stage::StageInfo& outInfo) override;
        
    private:
        // JSON解析用ヘルパーメソッド
        Array<Domain::Stage::StageObject> parseObjects(const JSON& objectsJson);
        Domain::Stage::CollisionType stringToCollisionType(const String& typeStr);
        Color parseColor(const String& colorStr);
    };
}