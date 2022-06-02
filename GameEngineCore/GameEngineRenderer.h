#pragma once
#include "GameEngineTransformComponent.h"
#include "GameEngineCollision.h"

// Ό³Έν :
class GameEngineRenderer 
	: public GameEngineTransformComponent
	
{
	friend class GameEngineLevel;

public:
	// constrcuter destructer
	GameEngineRenderer();
	~GameEngineRenderer();

	// delete Function
	GameEngineRenderer(const GameEngineRenderer& _Other) = delete;
	GameEngineRenderer(GameEngineRenderer&& _Other) noexcept = delete;
	GameEngineRenderer& operator=(const GameEngineRenderer& _Other) = delete;
	GameEngineRenderer& operator=(GameEngineRenderer&& _Other) noexcept = delete;

protected:
	virtual void Start();
	virtual void Update(float _DeltaTime) {}
	virtual void End() {}

public :
	void Set_RigidBody(PxRigidDynamic* _pRigidBody)
	{
		m_RigidBody = _pRigidBody;
	}
private :
	PxRigidDynamic* m_RigidBody;
private:
	void Render(float _DeltaTime);


};

