#include "GameEngineActor.h"
#include "GameEngineComponent.h"
#include "GameEngineTransformComponent.h"
#include "GameEngineCollision.h"

GameEngineActor::GameEngineActor() 
	:ParentLevel(nullptr)
{
}

GameEngineActor::~GameEngineActor() 
{
	for (GameEngineComponent* Com : AllComList)
	{
		delete Com;
	}

	for (GameEngineTransformComponent* Com : AllTransComList)
	{
		delete Com;
	}
}

void GameEngineActor::Start() 
{
	PxVec3 pxArray[] = { PxVec3(-500.f, 500.f, 0.f), PxVec3(500.f, 500.f, 0.f), PxVec3(500.f, -500.f, 0.f), PxVec3(-500.f, -500.f, 0.f) };
	Rigidbody = GameEngineCollision::GetInst()->CreateConvexMeshes(4, pxArray);

	if (!Rigidbody)
	{
		return;
	}
}

void GameEngineActor::Update(float _DeltaTime) 
{
	float4 Pos4 = GetTransform().GetLocalPosition();
	PxVec3 Pos3 = PxVec3{ Pos4.x, Pos4.y, Pos4.z };
	PxTransform transform = PxTransform(Pos3);
	GetRigidbody()->setGlobalPose(transform);
}

void GameEngineActor::End() 
{
}

void GameEngineActor::ComponentUpdate(float _ScaleDeltaTime, float _DeltaTime)
{
	for (GameEngineComponent* Com : AllComList)
	{
		Com->AddAccTime(_DeltaTime);
		Com->Update(_ScaleDeltaTime);
	}
}

void GameEngineActor::SettingTransformComponent(GameEngineTransformComponent* TransCom)
{
	GetTransform().PushChild(&TransCom->GetTransform());
}

void GameEngineActor::ComponentCalculateTransform()
{
	for (GameEngineTransformComponent* Com : AllTransComList)
	{
		Com->GetTransform().CalculateWorld();
	}
}