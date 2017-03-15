#pragma once

#include "EditorSystems/BaseEditorSystem.h"
#include "EditorSystems/EditorSystemsManager.h"

#include <Base/Introspection.h>
#include <Math/Vector.h>

class HUDSystem : public DAVA::InspBase, public BaseEditorSystem
{
public:
    HUDSystem(EditorSystemsManager* parent);
    ~HUDSystem() override;

private:
    enum eSearchOrder
    {
        SEARCH_FORWARD,
        SEARCH_BACKWARD
    };
    struct HUD;

    bool CanProcessInput(DAVA::UIEvent* currentInput) const override;
    void ProcessInput(DAVA::UIEvent* currentInput) override;
    EditorSystemsManager::eDragState RequireNewState(DAVA::UIEvent* currentInput) override;
    void OnDragStateChanged(EditorSystemsManager::eDragState currentState, EditorSystemsManager::eDragState previousState) override;
    void OnDisplayStateChanged(EditorSystemsManager::eDisplayState currentState, EditorSystemsManager::eDisplayState previousState) override;

    void OnSelectionChanged(const SelectedNodes& selection);
    void OnHighlightNode(const ControlNode* node);

    void OnMagnetLinesChanged(const DAVA::Vector<MagnetLineInfo>& magnetLines);
    void ClearMagnetLines();

    void ProcessCursor(const DAVA::Vector2& pos, eSearchOrder searchOrder = SEARCH_FORWARD);
    HUDAreaInfo GetControlArea(const DAVA::Vector2& pos, eSearchOrder searchOrder) const;
    void SetNewArea(const HUDAreaInfo& HUDAreaInfo);

    void UpdateAreasVisibility();
    void OnPackageChanged(PackageNode* package);

    void SetHUDEnabled(bool enabled);

    HUDAreaInfo activeAreaInfo;

    DAVA::RefPtr<DAVA::UIControl> hudControl;

    DAVA::Vector2 pressedPoint; //corner of selection rect
    DAVA::Vector2 hoveredPoint;

    DAVA::Map<ControlNode*, std::unique_ptr<HUD>> hudMap;
    DAVA::RefPtr<DAVA::UIControl> selectionRectControl;
    DAVA::Vector<DAVA::RefPtr<DAVA::UIControl>> magnetControls;
    DAVA::Vector<DAVA::RefPtr<DAVA::UIControl>> magnetTargetControls;
    SortedControlNodeSet sortedControlList;
    DAVA::RefPtr<DAVA::UIControl> hoveredNodeControl;

    bool showPivot;
    bool showRotate;

public:
    INTROSPECTION(HUDSystem,
                  MEMBER(showPivot, "Control Transformations/Can transform pivot", DAVA::I_SAVE | DAVA::I_VIEW | DAVA::I_EDIT | DAVA::I_PREFERENCE)
                  MEMBER(showRotate, "Control Transformations/Can rotate control", DAVA::I_SAVE | DAVA::I_VIEW | DAVA::I_EDIT | DAVA::I_PREFERENCE)
                  )
};
