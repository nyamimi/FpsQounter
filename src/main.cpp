#include "main.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/FlyingGameHUDRotation.hpp"
#include "GlobalNamespace/FPSCounter.hpp"
#include "GlobalNamespace/FPSCounterUIController.hpp"
#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "System/Array.hpp"
#include "System/Type.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

using namespace GlobalNamespace;
using namespace System;
using namespace UnityEngine;

static ModInfo modInfo;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

Vector3 fpsCounterLocalPositionFlying = { -2.5f, 2.5f, 12.0f };
Vector3 fpsCounterLocalPosition = { -3.2f, 3.2f, 7.0f };

MAKE_HOOK_OFFSETLESS(
    FPSCounterUIController_Awake,
    void,
    FPSCounterUIController* self
) {
    FPSCounterUIController_Awake(self);

    self->fpsCounter->set_enabled(true);
}

template <typename T>
T GetFirstObjectOfType() {
    ::Array<T>* objects = Resources::FindObjectsOfTypeAll<T>();
    
    if (objects != nullptr) {
        return objects->values[0];
    } else {
        return nullptr;
    }
}

MAKE_HOOK_OFFSETLESS(
    GameplayCoreInstaller_InstallBindings,
    void,
    GameplayCoreInstaller* self
) {
    GameplayCoreSceneSetupData* sceneSetupData = self->sceneSetupData;

    IDifficultyBeatmap* difficultyBeatmap = sceneSetupData->difficultyBeatmap;
    BeatmapData* beatmapData = difficultyBeatmap->get_beatmapData();

    int spawnRotationEventsCount = beatmapData->get_spawnRotationEventsCount();

    FPSCounterUIController* fpsCounterUI = GetFirstObjectOfType<FPSCounterUIController*>();
    FlyingGameHUDRotation* flyingGameHUD = GetFirstObjectOfType<FlyingGameHUDRotation*>();

    GameplayCoreInstaller_InstallBindings(self);

    if (spawnRotationEventsCount > 0) {
        Transform* flyingGameHUDTransform = flyingGameHUD->get_transform();
        Transform* fpsCounterUITransform = fpsCounterUI->get_transform();

        fpsCounterUITransform->set_localPosition(fpsCounterLocalPositionFlying);
        fpsCounterUITransform->set_parent(flyingGameHUDTransform);
    } else {
        Transform* fpsCounterUITransform = fpsCounterUI->get_transform();

        fpsCounterUITransform->set_localPosition(fpsCounterLocalPosition);
        
        UnityEngine::Object::Instantiate(fpsCounterUI);
    }
}

extern "C" void setup(ModInfo& info) {
    info.id = "FpsQounter";
    info.version = VERSION;

    modInfo = info;
    
    getLogger().info("Completed FpsQounter setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing FpsQounter hooks...");
    
    INSTALL_HOOK_OFFSETLESS(getLogger(), FPSCounterUIController_Awake, il2cpp_utils::FindMethodUnsafe("", "FPSCounterUIController", "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), GameplayCoreInstaller_InstallBindings, il2cpp_utils::FindMethodUnsafe("", "GameplayCoreInstaller", "InstallBindings", 0));

    getLogger().info("Installed FpsQounter hooks!");
}