#include "BulletPhysics.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "Utils.h"
#include <unordered_map>
#include <algorithm>

namespace GameEngine
{
	namespace Physics
	{
		vec3 Convert(const btVector3& v)
		{
			return vec3(v.getX(), v.getY(), v.getZ());
		}

		btVector3 Convert(const vec3& v)
		{
			return btVector3(v.x, v.y, v.z);
		}

		btQuaternion Convert(const Quaternion& quat)
		{
			return btQuaternion(quat.x, quat.y, quat.z, quat.w);
		}

		btTransform Convert(common::Transform& transform)
		{
			btTransform result;
			result.setOrigin(Convert(transform.GetPosition()));
			Quaternion rotaton(transform.GetRotation());
			result.setRotation(Convert(rotaton));
			return result;
		}

		struct Shape
		{
			std::unique_ptr<btCollisionShape> shape_;
			btVector3 positionOffset_;
		};

		struct BulletPhysics::Pimpl
		{
			Pimpl()
			{
				auto trimesh = new btTriangleMesh();
				btVector3 worldAabbMin(-1000, -1000, -1000);
				btVector3 worldAabbMax(1000, 1000, 1000);
				const int maxProxies = 32766;

				//m_broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);
				collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
				btDispacher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
				btBroadPhase = std::make_unique<btDbvtBroadphase>();
				//btBroadPhase = std::make_unique<btAxisSweep3>(worldAabbMin, worldAabbMax, maxProxies);
				btSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
				btDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(), collisionConfiguration.get());
				btDynamicWorld->setGravity(btVector3(0, -10, 0));
			}
			std::unique_ptr<btDynamicsWorld> btDynamicWorld;
			std::unique_ptr<btBroadphaseInterface> btBroadPhase;
			std::unique_ptr<btConstraintSolver> btSolver;
			std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
			std::unique_ptr<btDispatcher> btDispacher;
			std::unordered_map<uint32, std::unique_ptr<btRigidBody>> rigidBodies;
			std::unordered_map<uint32, common::Transform*> transforms;
			std::unordered_map<uint32, Shape> shapes_;
		};
		BulletPhysics::BulletPhysics()
			: simulationStep_(1.f / 60.f)
			, simualtePhysics_(true)
			, id_(1)
		{
			impl_.reset(new Pimpl());
		}
		void BulletPhysics::SetSimulationStep(float step)
		{
			simulationStep_ = step;
		}
		void BulletPhysics::EnableSimulation()
		{
			simualtePhysics_ = true;
		}
		void BulletPhysics::Simulate()
		{
			if (!simualtePhysics_)
			{
				return;
			}

			impl_->btDynamicWorld->stepSimulation(simulationStep_);

			for (auto& pair : impl_->rigidBodies)
			{
				auto& transform = impl_->transforms[pair.first];
				auto& rigidbody = pair.second;
				vec3 rot;
				rigidbody->getWorldTransform().getRotation().getEulerZYX(rot.z, rot.y, rot.x);

				rot = vec3(Utils::ToDegrees(rot.x), Utils::ToDegrees(rot.y), Utils::ToDegrees(rot.z));
				transform->SetRotation(rot);

				transform->SetPosition(Convert(rigidbody->getWorldTransform().getOrigin()));
				transform->TakeSnapShoot();
			}

		}
		void BulletPhysics::DisableSimulation()
		{
			simualtePhysics_ = false;
		}
		uint32 BulletPhysics::CreateBoxColider(const vec3 & positionOffset, const vec3& size)
		{
			impl_->shapes_[id_].shape_.reset(new btBoxShape(Convert(size)));
			impl_->shapes_[id_].positionOffset_ = Convert(positionOffset);
			return id_++;
		}
		uint32 BulletPhysics::CreateSphereColider(const vec3 & positionOffset, float radius)
		{
			impl_->shapes_[id_].shape_.reset(new btSphereShape(radius));
			impl_->shapes_[id_].positionOffset_ = Convert(positionOffset);
			return id_++;
		}
		uint32 BulletPhysics::CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, std::vector<float>& data, float hightFactor)
		{
			//impl_->shapes_[id_].shape_.reset(new btHeightfieldTerrainShape(size.x, size.y, &data[0],100, 1.f, true, false ));
			auto minElementIter = std::min_element(data.begin(), data.end());
			auto maxElementIter = std::max_element(data.begin(), data.end());

			auto minElement = minElementIter != data.end() ? *minElementIter : 0.f;
			auto maxElement = maxElementIter != data.end() ? *maxElementIter : 0.f;

			impl_->shapes_[id_].shape_.reset(new btHeightfieldTerrainShape(size.x, size.y, &data[0], 1.f, minElement, maxElement, 1.f, PHY_FLOAT, false));
			impl_->shapes_[id_].positionOffset_ = Convert(positionOffset);

			//>(terrain->GetSize().x, terrain->GetSize().y, &tdata[0], 1.f, -100, 100.f, 1, PHY_FLOAT, false);
			return id_++;
		}
		uint32 BulletPhysics::CreateMeshCollider(const vec3 & positionOffset, const std::vector<float>& data, const std::vector<uint16> indicies)
		{
			auto& shape = impl_->shapes_[id_];

			auto trimesh = new btTriangleMesh();

			for (int i = 0; i < indicies.size(); i+=9)
			{
				btVector3 v0(data[i], data[i+1], data[i+2]);
				btVector3 v1(data[i+3], data[i + 4], data[i + 5]);
				btVector3 v2(data[i+6], data[i + 7], data[i + 8]);

				trimesh->addTriangle(v0, v1, v2);
			}
			btConvexShape* tmpshape = new btConvexTriangleMeshShape(trimesh);

			shape.shape_= std::unique_ptr<btCollisionShape>(tmpshape);
			shape.positionOffset_ = Convert(positionOffset);

			btShapeHull* hull = new btShapeHull(tmpshape);
			btScalar margin = tmpshape->getMargin();
			hull->buildHull(margin);
			tmpshape->setUserPointer(hull);

			return id_++;
		}
		uint32  BulletPhysics::CreateRigidbody(uint32 shapeId, common::Transform& transform, float mass, bool isStatic)
		{
			if (impl_->shapes_.count(shapeId) == 0)
			{
				return 0;
			}

			btCollisionShape* shape = impl_->shapes_.at(shapeId).shape_.get();

			btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);

			if (!isStatic)
				shape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* myMotionState = new btDefaultMotionState(Convert(transform));

			btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

			impl_->rigidBodies[id_] = std::make_unique<btRigidBody>(cInfo);
			impl_->transforms[id_] = &transform;

			auto& body = impl_->rigidBodies[id_];
			body->setUserIndex(-1);
			impl_->btDynamicWorld->addRigidBody(body.get());
			return id_++;
		}
		void BulletPhysics::SetVelocityRigidbody(uint32 rigidBodyId,  const vec3& velocity)
		{
			if (!impl_->rigidBodies.count(rigidBodyId))
			{
				return;
			}

			impl_->rigidBodies.at(rigidBodyId)->setLinearVelocity(Convert(velocity));
		}
		void BulletPhysics::RemoveRigidBody(uint32 id)
		{
			if (!impl_->rigidBodies.count(id))
			{
				return;
			}

			auto& rigidbody = impl_->rigidBodies.at(id);
			impl_->btDynamicWorld->removeRigidBody(rigidbody.get());
			impl_->rigidBodies.erase(id);
		}
	}

} // GameEngine
