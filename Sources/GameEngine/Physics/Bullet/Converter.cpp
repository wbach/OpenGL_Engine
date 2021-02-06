#include "Converter.h"

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

Quaternion Convert(const btQuaternion& q)
{
    //return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
    return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

btTransform Convert(const common::Transform& transform)
{
    btTransform result;
    result.setOrigin(Convert(transform.GetPosition()));
    result.setRotation(Convert(transform.GetRotation().value_));
    return result;
}

btTransform Convert(const common::Transform& transform, const vec3& positionOffset)
{
    btTransform result;
    result.setOrigin(Convert(transform.GetPosition() + positionOffset));
    result.setRotation(Convert(transform.GetRotation().value_));
    return result;
}

common::Transform Convert(const btTransform &transform)
{
    common::Transform result;
    result.SetPosition(Convert(transform.getOrigin()));
    result.SetRotation(Convert(transform.getRotation()));
    return result;
}

/*
glm::vec3 bulletToGlm(const btVector3& v) { return glm::vec3(v.getX(), v.getY(), v.getZ()); }
btVector3 glmToBullet(const glm::vec3& v) { return btVector3(v.x, v.y, v.z); }
glm::quat bulletToGlm(const btQuaternion& q) { return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ()); }
btQuaternion glmToBullet(const glm::quat& q) { return btQuaternion(q.x, q.y, q.z, q.w); }
btMatrix3x3 glmToBullet(const glm::mat3& m) { return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1], m[0][2], m[1][2], m[2][2]); }

// btTransform does not contain a full 4x4 matrix, so this transform is lossy.
// Affine transformations are OK but perspective transformations are not.
btTransform glmToBullet(const glm::mat4& m)
{
    glm::mat3 m3(m);
    return btTransform(glmToBullet(m3), glmToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
}
glm::mat4 bulletToGlm(const btTransform& t)
{
    glm::mat4 m(glm::mat4::_null);
    const btMatrix3x3& basis = t.getBasis();
    // rotation
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            m[c][r] = basis[r][c];
        }
    }
    // traslation
    btVector3 origin = t.getOrigin();
    m[3][0] = origin.getX();
    m[3][1] = origin.getY();
    m[3][2] = origin.getZ();
    // unit scale
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
    return m;
}
*/
}  // namespace Physics
}  // namespace GameEngine
