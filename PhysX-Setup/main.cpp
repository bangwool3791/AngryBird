#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

int main()
{
	// declare variables
	physx::PxDefaultAllocator mDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback mDefaultErrorCallback;
	physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
	physx::PxTolerancesScale mToleranceSacle;
	physx::PxFoundation* mFoundation = NULL;
	physx::PxPhysics* mPhyscis = NULL;
	physx::PxScene* mScene = NULL;
	physx::PxMaterial* mMaterial = NULL;
	physx::PxPvd* mPvd = NULL;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
	if (!mFoundation)throw("PxCreateFoundataion failed!");
	
	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	//Class to define the scale at which simulation runs. Most simulation tolerances are calculated in terms of the values her 
	//brief The approximate size of objects in the simulation.
	//For simulating roughly human - sized in metric units, 1 is a good choice.If simulation is done in centimetres, use 100 instead.This is used to estimate certain length - related tolerances.
	
	mToleranceSacle.length = 100; //typical length of an object
	mToleranceSacle.speed = 981;
	mPhyscis = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceSacle, true, mPvd);
	physx::PxSceneDesc sceneDesc(mPhyscis->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//create physics material
	mScene = mPhyscis->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//Á¤Áö ¸¶Âû°è¼ö, ¿îµ¿ ¸¶Âû°è¼ö, Åº¼º·Â ?
	mMaterial = mPhyscis->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhyscis, physx::PxPlane(0, 1, 0, 1), *mMaterial);
	mScene->addActor(*groundPlane);

	float halfExtent = .5f;
	physx::PxShape* shape = mPhyscis->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, 0.00001f), *mMaterial);
	physx::PxU32 size = 30;
	const physx::PxTransform t(physx::PxVec3(0));

	for (physx::PxU32 i = 0; i < size; i++)
	{
		for (physx::PxU32 j = 0; j < size - i; j++)
		{
			physx::PxTransform localTm(physx::PxVec3(
				physx::PxReal(j * 2) - physx::PxReal(size - i),
				physx::PxReal(i * 2 + 1)
				, 0
			) * halfExtent);
			physx::PxRigidDynamic* body = mPhyscis->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			mScene->addActor(*body);
		}
	}
	shape->release();



	while (1)
	{
		mScene->simulate(1.0f / 60.0f);
		mScene->fetchResults(true);
	}

}