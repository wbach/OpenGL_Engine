#include "BulletPhysics.h"
#include "btBulletDynamicsCommon.h"
#include "Utils.h"
#include <unordered_map>

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

		struct BulletPhysics::Pimpl
		{
			Pimpl()
			{
				collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
				btDispacher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
				btBroadPhase = std::make_unique<btDbvtBroadphase>();
				btSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
				btDynamicWorld = std::make_unique<btDiscreteDynamicsWorld>(btDispacher.get(), btBroadPhase.get(), btSolver.get(), collisionConfiguration.get());
				btDynamicWorld->setGravity(btVector3(0, -10, 0));

				collisonShape.reset(new btBoxShape(btVector3(.1, .1, .1)));
			}
			std::unique_ptr<btCollisionShape> collisonShape;

			std::unique_ptr<btDynamicsWorld> btDynamicWorld;
			std::unique_ptr<btBroadphaseInterface> btBroadPhase;
			std::unique_ptr<btConstraintSolver> btSolver;
			std::unique_ptr<btCollisionConfiguration> collisionConfiguration;
			std::unique_ptr<btDispatcher> btDispacher;
			std::unordered_map<uint32, std::unique_ptr<btRigidBody>> rigidBodies;
			std::unordered_map<uint32, common::Transform*> transforms;
		};
		BulletPhysics::BulletPhysics()
			: simulationStep_(1.f / 60.f)
			, simualtePhysics_(false)
			, id_(0)
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
		uint32 BulletPhysics::CreateBoxColider(const vec3 & positionOffset, float size)
		{
			return id_++;
		}
		uint32 BulletPhysics::CreateSphereColider(const vec3 & positionOffset, float radius)
		{
			return id_++;
		}
		uint32 BulletPhysics::CreateTerrainColider(const vec3 & positionOffset, std::vector<float>& data)
		{
			return id_++;
		}
		uint32  BulletPhysics::CreateRigidbody(common::Transform& transform, float mass, bool isStatic)
		{
			btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);

			if (!isStatic)
				impl_->collisonShape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* myMotionState = new btDefaultMotionState(Convert(transform));

			btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, impl_->collisonShape.get(), localInertia);

			impl_->rigidBodies[id_] = std::make_unique<btRigidBody>(cInfo);
			impl_->transforms[id_] = &transform;

			auto& body = impl_->rigidBodies[id_];
			body->setUserIndex(-1);
			impl_->btDynamicWorld->addRigidBody(body.get());
			return id_++;
		}
	}

} // GameEngine
