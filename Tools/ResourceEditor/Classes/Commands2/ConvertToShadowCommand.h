#ifndef __CONVERT_TO_SHADOW_COMMAND_H__
#define __CONVERT_TO_SHADOW_COMMAND_H__

#include "QtTools/Commands/CommandWithoutExecute.h"
#include "DAVAEngine.h"

class ConvertToShadowCommand : public CommandWithoutExecute
{
public:
    ConvertToShadowCommand(DAVA::Entity* entity, DAVA::RenderBatch* batch);
    ~ConvertToShadowCommand();

    void Undo() override;
    void Redo() override;
    DAVA::Entity* GetEntity() const;

    static bool CanConvertBatchToShadow(DAVA::RenderBatch* renderBatch);

    DAVA::Entity* entity;
    DAVA::RenderObject* renderObject;
    DAVA::RenderBatch* oldBatch;
    DAVA::RenderBatch* newBatch;
};


#endif // #ifndef __CONVERT_TO_SHADOW_COMMAND_H__