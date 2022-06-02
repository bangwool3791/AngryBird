#include "Monster.h"
#include <GameEngineCore/GameEngineRenderer.h>

Monster::Monster() 
{
}

Monster::~Monster() 
{
}

void Monster::Start()
{
	__super::Start();
	{
		GameEngineRenderer* RendererTest = CreateComponent<GameEngineRenderer>();
		RendererTest->GetTransform().SetLocalScale({ 50, 50, 0 });
	}
}


void Monster::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}

void Monster::End()
{

}

