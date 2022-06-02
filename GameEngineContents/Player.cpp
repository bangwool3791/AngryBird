#include "Player.h"
#include <GameEngineCore/GameEngineRenderer.h>
#include <GameEngineCore/GameEngineCollision.h>

Player::Player() 
{
}

Player::~Player() 
{
}

void Player::Start()
{
	__super::Start();

	{
		GameEngineRenderer* RendererTest = CreateComponent<GameEngineRenderer>();
		RendererTest->GetTransform().SetLocalScale({ 100, 100, 0 });
	}
}


void Player::Update(float _DeltaTime) 
{
	__super::Update(_DeltaTime);
}

void Player::End() 
{

}

