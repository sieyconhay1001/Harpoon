#pragma once

enum class FrameStage;
class GameEvent;
class CViewSetup;

namespace Visuals
{
    void FogControl() noexcept;
    void playerModel(FrameStage stage) noexcept;
    void colorWorld() noexcept;
    void modifySmoke(FrameStage stage) noexcept;
    void thirdperson(CViewSetup* setup = nullptr, bool inOverride = false) noexcept;
    void removeVisualRecoil(FrameStage stage) noexcept;
    void removeBlur(FrameStage stage) noexcept;
    void updateBrightness() noexcept;
    void removeGrass(FrameStage stage) noexcept;
    void remove3dSky() noexcept;
    void removeShadows() noexcept;
    void applyZoom(FrameStage stage) noexcept;
    void applyScreenEffects() noexcept;
    void hitEffect(GameEvent* event = nullptr) noexcept;
    void hitMarker(GameEvent* event = nullptr) noexcept;
    void disablePostProcessing(FrameStage stage) noexcept;
    void reduceFlashEffect() noexcept;
    bool removeHands(const char* modelName) noexcept;
    bool removeSleeves(const char* modelName) noexcept;
    bool removeWeapons(const char* modelName) noexcept;
    void skybox(FrameStage stage) noexcept;
    void NightModeExtended() noexcept;                  
    void bulletBeams(GameEvent* event) noexcept;
    void grenadeBeams(GameEvent* event) noexcept;
    //void transparentWorld() noexcept;
    void BounceRing(GameEvent* event) noexcept;
    void WalkbotRing() noexcept;
    void NoScopeInZoom(FrameStage stage) noexcept;
    void MoltovColor() noexcept;
    extern int lastSize;
}
