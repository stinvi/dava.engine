#include "Base/BaseMath.h"
#include "Base/BaseObject.h"
#include "ShootSystem.h"
#include "Components/ShootComponent.h"

#include <Reflection/ReflectionRegistrator.h>
#include "Scene3D/Scene.h"
#include "Scene3D/Components/ComponentHelpers.h"
#include "Scene3D/Components/TransformComponent.h"
#include "Utils/Random.h"

#include <NetworkCore/Snapshot.h>
#include <NetworkCore/Scene3D/Components/NetworkDebugDrawComponent.h>
#include <NetworkCore/Scene3D/Components/NetworkReplicationComponent.h>
#include <NetworkCore/Scene3D/Components/NetworkTransformComponent.h>
#include <NetworkCore/Scene3D/Components/NetworkMovementComponent.h>
#include <NetworkCore/Scene3D/Components/SingleComponents/NetworkTimeSingleComponent.h>
#include <NetworkCore/Scene3D/Components/SingleComponents/NetworkGameModeSingleComponent.h>

#include "Components/GameStunningComponent.h"
#include "Components/DamageComponent.h"

#include <Physics/PhysicsSystem.h>
#include <Physics/CollisionSingleComponent.h>
#include <Physics/Core/DynamicBodyComponent.h>
#include <Physics/Core/BoxShapeComponent.h>


#include "NetworkCore/NetworkCoreUtils.h"

using namespace DAVA;

DAVA_VIRTUAL_REFLECTION_IMPL(ShootSystem)
{
    ReflectionRegistrator<ShootSystem>::Begin()[M::Tags("shoot")]
    .ConstructorByPointer<Scene*>()
    .Method("ProcessFixed", &ShootSystem::ProcessFixed)[M::SystemProcess(SP::Group::GAMEPLAY, SP::Type::FIXED, 8.0f)]
    .End();
}

namespace ShootSystemDetail
{
template <typename T>
static bool CompareTransform(const T& lhs, const T& rhs, uint32 size, float32 epsilon, uint32 frameId)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (!FLOAT_EQUAL_EPS(lhs.data[i], rhs.data[i], epsilon))
        {
            Logger::Debug("Transforms aren't equal (compared by shoot system), diff: %f, index: %d, frame: %d", std::abs(lhs.data[i] - rhs.data[i]), i, frameId);

            return false;
        }
    }
    return true;
}
}

ShootSystem::ShootSystem(Scene* scene)
    : DAVA::BaseSimulationSystem(scene, ComponentUtils::MakeMask<ShootComponent>())
    , entityGroup(scene->AquireEntityGroup<ShootComponent>())
    , pendingEntities(scene->AquireEntityGroupOnAdd(entityGroup, this))
{
}

ShootSystem::~ShootSystem()
{
}

void ShootSystem::NextState(Entity* bullet, ShootComponent* shootComponent, DAVA::float32 timeElapsed)
{
    const CollisionSingleComponent* collisionSingleComponent = GetScene()->GetSingleComponentForRead<CollisionSingleComponent>(this);

    switch (shootComponent->GetPhase())
    {
    case ShootPhase::BURN:
    {
        if (shootComponent->GetDistance() > 8.0)
        {
            shootComponent->SetPhase(ShootPhase::FLY);
            BoxShapeComponent* boxShape = bullet->GetComponent<BoxShapeComponent>();
            if (boxShape)
            {
                boxShape->SetTypeMask(2);
                boxShape->SetTypeMaskToCollideWith(1);
            }
        }
        break;
    }

    case ShootPhase::FLY:
    {
        if (shootComponent->GetDistance() > ShootComponent::MAX_DISTANCE)
        {
            shootComponent->SetPhase(ShootPhase::DESTROY);
        }
        else if (!collisionSingleComponent->GetCollisionsWithEntity(bullet).empty())
        {
            shootComponent->SetPhase(ShootPhase::DESTROY);
        }
        break;
    }

    case ShootPhase::DESTROY:
        return;
    }

    switch (shootComponent->GetPhase())
    {
    case ShootPhase::BURN:
    case ShootPhase::FLY:
    {
        TransformComponent* transformComp = bullet->GetComponent<TransformComponent>();
        Vector3 moveVector(0.f, ShootComponent::MOVE_SPEED * timeElapsed, 0.f);
        transformComp->SetLocalTransform(Transform(
        Matrix4::MakeTranslation(moveVector) * transformComp->GetLocalMatrix()));
        shootComponent->SetDistance(shootComponent->GetDistance() + 1);
        return;
    }

    case ShootPhase::DESTROY:
        return;
    }
}

void ShootSystem::ProcessFixed(float32 timeElapsed)
{
    Vector<Entity*> destroyedBullets;
    for (Entity* bullet : entityGroup->GetEntities())
    {
        if (IsClient(this) && !IsClientOwner(bullet))
        {
            continue;
        }

        ShootComponent* shootComponent = bullet->GetComponent<ShootComponent>();
        NextState(bullet, shootComponent, timeElapsed);

        if (shootComponent->GetPhase() == ShootPhase::DESTROY)
        {
            destroyedBullets.push_back(bullet);
        }
    }

    for (Entity* destroyedBullet : destroyedBullets)
    {
        GetScene()->RemoveNode(destroyedBullet);
        // SERVER_COMPLETE
        //SafeRelease(destroyedBullet);
    }
}
