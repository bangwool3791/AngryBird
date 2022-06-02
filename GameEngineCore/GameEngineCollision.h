#pragma once
#include "GameEngineComponent.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

class GameEngineCollision
{
private:
	static GameEngineCollision* Inst_;
public:
	static GameEngineCollision* GetInst()
	{
		return Inst_;
	}
	static void Destroy()
	{
		if (nullptr != Inst_)
		{
			delete Inst_;
			Inst_ = nullptr;
		}
	}
private:
	// constrcuter destructer
	GameEngineCollision();
	~GameEngineCollision();

	// delete Function
	GameEngineCollision(const GameEngineCollision& _Other) = delete;
	GameEngineCollision(GameEngineCollision&& _Other) noexcept = delete;
	GameEngineCollision& operator=(const GameEngineCollision& _Other) = delete;
	GameEngineCollision& operator=(GameEngineCollision&& _Other) noexcept = delete;

protected:

private:
	PxCooking* m_pCooking;
	PxPhysics* m_pPhyscis;
	PxScene* m_pScene;
	PxMaterial* m_pMaterial;

	PxDefaultAllocator m_DefaultAllocatorCallback;
	PxDefaultErrorCallback m_DefaultErrorCallback;
	PxDefaultCpuDispatcher* m_Dispatcher = NULL;
	PxTolerancesScale m_ToleranceSacle;
	PxFoundation* m_Foundation = NULL;
	PxPvd* m_Pvd = NULL;

	//reserve vector or memeber pointer
	//and release vec or member pointer
public:
	void Start();
	void Update(float _DeltaTime);
	void End();
public:
	PxRigidDynamic* CreateConvexMeshes(const PxU32 _numVerts, const PxVec3* _verts);
	//Create and return PxRigidDynamic
	template<PxConvexMeshCookingType::Enum convexMeshCookingType, bool directInsertion, PxU32 gaussMapLimit>
	PxRigidDynamic* createRandomConvex(PxU32 numVerts, const PxVec3* verts)
	{
			PxCookingParams params = m_pCooking->getParams();

			// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
			params.convexMeshCookingType = convexMeshCookingType;

			// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
			// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
			params.gaussMapLimit = gaussMapLimit;
			m_pCooking->setParams(params);

			static const PxVec3 convexVerts[] = { PxVec3(0,1,0),PxVec3(1,0,0),PxVec3(-1,0,0),PxVec3(0,0,1),
				PxVec3(0,0,-1) };
			// Setup the convex mesh descriptor
			PxConvexMeshDesc desc;

			// We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
			desc.points.data = convexVerts;
			desc.points.count = numVerts;
			desc.points.stride = sizeof(PxVec3);
			desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

			PxU32 meshSize = 0;
			PxConvexMesh* convex = NULL;

			if (directInsertion)
			{
				// Directly insert mesh into PhysX
				convex = m_pCooking->createConvexMesh(desc, m_pPhyscis->getPhysicsInsertionCallback());
				PX_ASSERT(convex);
			}
			else
			{
				PxDefaultMemoryOutputStream outStream;
				bool res = m_pCooking->cookConvexMesh(desc, outStream);
				PX_UNUSED(res);
				PX_ASSERT(res);
				meshSize = outStream.getSize();

				// Create the mesh from a stream.
				PxDefaultMemoryInputData inStream(outStream.getData(), outStream.getSize());
				convex = m_pPhyscis->createConvexMesh(inStream);
				PX_ASSERT(convex);
			}

			PxShape* shape = m_pPhyscis->createShape(PxConvexMeshGeometry(convex), *m_pMaterial);
			const PxTransform t(PxVec3(0.f, 1000.f, 0.f));

			//float halfExtent = 500.f;

			PxTransform localTm(PxVec3(PxReal(0),PxReal(0.f), PxReal(0.f)));

			PxRigidDynamic* body = m_pPhyscis->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			m_pScene->addActor(*body);

			convex->release();
			return body;
	}
};

