#pragma once

#include "Commands2/Base/RECommand.h"
#include "Particles/ParticleLayer.h"
#include "Particles/ParticleForceSimplified.h"

namespace DAVA
{
class ParticleForce;
}

class ParticleSimplifiedForceMoveCommand : public RECommand
{
public:
    ParticleSimplifiedForceMoveCommand(DAVA::ParticleForceSimplified* force, DAVA::ParticleLayer* oldLayer, DAVA::ParticleLayer* newLayer);
    ~ParticleSimplifiedForceMoveCommand();

    void Undo() override;
    void Redo() override;

    DAVA::ParticleForceSimplified* force;
    DAVA::ParticleLayer* oldLayer;
    DAVA::ParticleLayer* newLayer;
};

class ParticleForceMoveCommand : public RECommand
{
public:
    ParticleForceMoveCommand(DAVA::ParticleForce* force, DAVA::ParticleLayer* oldLayer, DAVA::ParticleLayer* newLayer);
    ~ParticleForceMoveCommand();

    void Undo() override;
    void Redo() override;

    DAVA::ParticleForce* force;
    DAVA::ParticleLayer* oldLayer;
    DAVA::ParticleLayer* newLayer;
};
