#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include <ctype.h>

using namespace physx;
physx::PxPhysics* gPhyscis = NULL;
PxCooking* gCooking = NULL;
physx::PxScene* gScene = NULL;
physx::PxMaterial* gMaterial = NULL;
float rand(float loVal, float hiVal)
{
	return loVal + (float(rand()) / float(RAND_MAX)) * (hiVal - loVal);
}

template<PxConvexMeshCookingType::Enum convexMeshCookingType, bool directInsertion, PxU32 gaussMapLimit>
void createRandomConvex(PxU32 numVerts, const PxVec3* verts)
{
	PxCookingParams params = gCooking->getParams();

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.convexMeshCookingType = convexMeshCookingType;

	// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
	// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
	params.gaussMapLimit = gaussMapLimit;
	gCooking->setParams(params);

	// Setup the convex mesh descriptor
	PxConvexMeshDesc desc;

	// We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
	desc.points.data = verts;
	desc.points.count = numVerts;
	desc.points.stride = sizeof(PxVec3);
	desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxU32 meshSize = 0;
	PxConvexMesh* convex = NULL;

	if (directInsertion)
	{
		// Directly insert mesh into PhysX
		convex = gCooking->createConvexMesh(desc, gPhyscis->getPhysicsInsertionCallback());
		PX_ASSERT(convex);
	}
	else
	{
		PxDefaultMemoryOutputStream outStream;
		bool res = gCooking->cookConvexMesh(desc, outStream);
		PX_UNUSED(res);
		PX_ASSERT(res);
		meshSize = outStream.getSize();

		// Create the mesh from a stream.
		PxDefaultMemoryInputData inStream(outStream.getData(), outStream.getSize());
		convex = gPhyscis->createConvexMesh(inStream);
		PX_ASSERT(convex);
	}

	physx::PxShape* shape = gPhyscis->createShape(physx::PxConvexMeshGeometry(convex), *gMaterial);
	const physx::PxTransform t(physx::PxVec3(0));

	float halfExtent = 500.f;

	physx::PxTransform localTm(physx::PxVec3(
		physx::PxReal(0),
		physx::PxReal(0.f)
		, 0.f
	));
	physx::PxRigidDynamic* body = gPhyscis->createRigidDynamic(t.transform(localTm));
	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);

	convex->release();
}

void createConvexMeshes()
{
	const PxU32 numVerts = 3;
	PxVec3* vertices = new PxVec3[numVerts];

	vertices[0] = PxVec3(0.f, 50.f, 0.f);
	vertices[1] = PxVec3(50.f, 0.f, 0.f);
	vertices[2] = PxVec3(-50.f, 0.f, 0.f);

	// Create convex mesh using the quickhull algorithm with different settings
	//PxConvexMeshCookingType 2인자 true일 때와, false 일 때와 차이 명확히 할 것
	// The default convex mesh creation serializing to a stream, useful for offline cooking.
	createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 16>(numVerts, vertices);

	//// The default convex mesh creation without the additional gauss map data.
	//createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 256>(numVerts, vertices);

	//// Convex mesh creation inserting the mesh directly into PhysX. 
	//// Useful for runtime cooking.
	//createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 16>(numVerts, vertices);

	//// Convex mesh creation inserting the mesh directly into PhysX, without gauss map data.
	//// Useful for runtime cooking.
	//createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 256>(numVerts, vertices);

	delete[] vertices;
}


int main()
{
	// declare variables
	physx::PxDefaultAllocator mDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback mDefaultErrorCallback;
	physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
	physx::PxTolerancesScale mToleranceSacle;
	physx::PxFoundation* mFoundation = NULL;
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
	gPhyscis = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceSacle, true, mPvd);
	physx::PxSceneDesc sceneDesc(gPhyscis->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//create physics material
	gScene = gPhyscis->createScene(sceneDesc);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(mToleranceSacle));
	if (!gCooking)
	{
		return 0;
	}

	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//정지 마찰계수, 운동 마찰계수, 보존되는 에너지량 ?
	gMaterial = gPhyscis->createMaterial(10.f, 10.f, 0.5f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhyscis, physx::PxPlane(0, 1, 0, 1), *gMaterial);
	gScene->addActor(*groundPlane);

	createConvexMeshes();


	while (1)
	{
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
	}

}