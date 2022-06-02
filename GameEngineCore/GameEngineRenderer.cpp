#include "GameEngineRenderer.h"
#include "GameEngineActor.h"
#include "GameEngineLevel.h"
#include <Windows.h>
#include <GameEngineBase/GameEngineWindow.h>

#include "GameEngineVertexBuffer.h"
#include "GameEngineIndexBuffer.h"

GameEngineRenderer::GameEngineRenderer() 
{
}

GameEngineRenderer::~GameEngineRenderer() 
{
}


void GameEngineRenderer::Start() 
{
	GetActor()->GetLevel()->PushRenderer(this);
	Set_RigidBody(GetActor()->GetRigidbody());
}


void GameEngineRenderer::Render(float _DeltaTime)
{
	PxTransform transform = m_RigidBody->getGlobalPose();
	float4 Pos4 = float4{ transform.p.x,transform.p.y, 0.f };
	GetTransform().SetLocalPosition(Pos4);
	// 랜더링
	GameEngineVertexBuffer* Vertex = GameEngineVertexBuffer::Find("Box");
	GameEngineIndexBuffer* Index = GameEngineIndexBuffer::Find("Box");
	
	std::vector<POINT> DrawVertex;
	DrawVertex.resize(Index->Indexs.size());

	std::vector<float4> CopyBuffer;
	CopyBuffer.resize(Index->Indexs.size());


	
	for (size_t i = 0; i < Index->Indexs.size(); i++)
	{
		int TriIndex = Index->Indexs[i];

		// 0 번째 순서의 점이 됩니다.
		// 최초에 원본 매쉬의 점을 복사합니다.
		CopyBuffer[i] = Vertex->Vertexs[TriIndex];

		//CopyBuffer[i] = CopyBuffer[i] * GetTransform().GetWorldWorld();
		CopyBuffer[i] = CopyBuffer[i] * GetTransform().GetWorldWorld();
		DrawVertex[i] = CopyBuffer[i].GetConvertWindowPOINT();
	}

	
	for (size_t i = 0; i < DrawVertex.size(); i += 3)
	{
		Polygon(GameEngineWindow::GetHDC(), &DrawVertex[i], 3);
	}
	

	// Rectangle(GameEngineWindow::GetHDC(), LeftTop.ix(), LeftTop.iy(), RightBot.ix(), RightBot.iy());
}