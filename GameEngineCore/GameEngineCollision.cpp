#include "GameEngineCollision.h"
#include <ctype.h>

GameEngineCollision* GameEngineCollision::Inst_ = new GameEngineCollision();
GameEngineCollision::GameEngineCollision() 
: m_pCooking(nullptr) 
, m_pPhyscis(nullptr) 
, m_pScene(nullptr) 
, m_pMaterial(nullptr)
{

}

GameEngineCollision::~GameEngineCollision() 
{
}

PxRigidDynamic* GameEngineCollision::CreateConvexMeshes(const PxU32 _numVerts, const PxVec3* _verts)
{
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = _numVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = _verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!m_pCooking->cookConvexMesh(convexDesc, buf, &result))
		return nullptr;
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = m_pPhyscis->createConvexMesh(input);

	PxShape* shape = m_pPhyscis->createShape(physx::PxConvexMeshGeometry(convexMesh), *m_pMaterial);
	const physx::PxTransform t(physx::PxVec3(0.f, 1000.f, +100.f));

	float halfExtent = 500.f;

	PxTransform localTm(physx::PxVec3(PxReal(0.f), PxReal(0.f), PxReal(0.f)));

	PxRigidDynamic* body = m_pPhyscis->createRigidDynamic(t.transform(localTm));
	body->setMass(10);
	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	m_pScene->addActor(*body);

	convexMesh->release();

	return body;
}

void GameEngineCollision::Start()
{
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
	
	if (!m_Foundation)
		throw("PxCreateFoundataion failed!");

	//Debuger Initialize
	m_Pvd = PxCreatePvd(*m_Foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Class to define the scale at which simulation runs. Most simulation tolerances are calculated in terms of the values her 
	//brief The approximate size of objects in the simulation.
	//For simulating roughly human - sized in metric units, 1 is a good choice.If simulation is done in centimetres, use 100 instead.This is used to estimate certain length - related tolerances.

	/*
	brief The approximate size of objects in the simulation.
	For simulating roughly human-sized in metric units, 1 is a good choice. If simulation is done in centimetres, use 100 instead. This is used to estimate certain length-related tolerances.
	*/
	m_ToleranceSacle.length = 100; //typical length of an object

	/*
	brief The typical magnitude of velocities of objects in simulation. This is used to estimate whether a contact should be treated as bouncing or resting based on its impact velocity, and a kinetic energy threshold below which the simulation may put objects to sleep.
	For normal physical environments, a good choice is the approximate speed of an object falling under gravity for one second.
	*/
	m_ToleranceSacle.speed = 100;

	m_pPhyscis = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceSacle, true, m_Pvd);
	PxSceneDesc sceneDesc(m_pPhyscis->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -98.0f, 0.0f);
	m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//create physics material
	m_pScene = m_pPhyscis->createScene(sceneDesc);

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(m_ToleranceSacle));
	if (!m_pCooking)
	{
		return;
	}
	
	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//정지 마찰계수, 운동 마찰계수, 보존되는 에너지량 ?
	m_pMaterial = m_pPhyscis->createMaterial(10.f, 10.f, 0.5f);
	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhyscis, PxPlane(0, 1, 0, 1), *m_pMaterial);
	m_pScene->addActor(*groundPlane);
	//CreateConvexMeshes();
}

void GameEngineCollision::Update(float _DeltaTime)
{
	m_pScene->simulate(_DeltaTime);
	m_pScene->fetchResults(true);
}

void GameEngineCollision::End()
{

}
