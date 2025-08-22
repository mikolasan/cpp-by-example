#include <bx/uint32_t.h>
#include <bx/timer.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/filepath.h>
#include <bx/string.h>

struct Camera
{
	Camera() :
		m_accum_orientation(bx::InitNone)
	{
		reset();
	}

	void reset()
	{
		m_target.curr = { 0.0f, 0.0f, 0.0f };
		m_target.dest = { 0.0f, 0.0f, 0.0f };

		m_pos.curr = { 2.0f, 3.0f, 4.0f };
		m_pos.dest = { 5.0f, 9.0f, -12.0f };

		m_orbit[0] = 0.0f;
		m_orbit[1] = 0.0f;

		m_accum_orientation = bx::Quaternion(bx::InitIdentity);
	}

	void printStats()
	{
		printf("Camera Stats:\n");
        printf("  Target: curr = (%.3f, %.3f, %.3f), dest = (%.3f, %.3f, %.3f)\n",
            m_target.curr.x, m_target.curr.y, m_target.curr.z,
            m_target.dest.x, m_target.dest.y, m_target.dest.z);

        printf("  Position: curr = (%.3f, %.3f, %.3f), dest = (%.3f, %.3f, %.3f)\n",
            m_pos.curr.x, m_pos.curr.y, m_pos.curr.z,
            m_pos.dest.x, m_pos.dest.y, m_pos.dest.z);

        printf("  Orbit: yaw = %.6f, pitch = %.6f\n", m_orbit[0], m_orbit[1]);
	}

	void mtxLookAt(float* _outViewMtx)
	{
		bx::mtxLookAt(_outViewMtx, m_pos.curr, m_target.curr);
	}

	void orbit(float _dx, float _dy)
	{
		m_orbit[0] += _dx;
		m_orbit[1] += _dy;
	}

	void dolly(float _dz)
	{
		const float cnear = 1.0f;
		const float cfar  = 100.0f;

		const bx::Vec3 toTarget     = bx::sub(m_target.dest, m_pos.dest);
		const float toTargetLen     = bx::length(toTarget);
		const float invToTargetLen  = 1.0f / (toTargetLen + bx::kFloatSmallest);
		const bx::Vec3 toTargetNorm = bx::mul(toTarget, invToTargetLen);

		float delta  = toTargetLen * _dz;
		float newLen = toTargetLen + delta;
		if ( (cnear  < newLen || _dz < 0.0f)
		&&   (newLen < cfar   || _dz > 0.0f) )
		{
			m_pos.dest = bx::mad(toTargetNorm, delta, m_pos.dest);
		}
	}

	void consumeOrbit(float _amount)
	{
		float consume[2];
		consume[0] = m_orbit[0] * _amount;
		consume[1] = m_orbit[1] * _amount;
		m_orbit[0] -= consume[0];
		m_orbit[1] -= consume[1];

		const bx::Vec3 toPos     = bx::sub(m_pos.curr, m_target.curr);
		const float toPosLen     = bx::length(toPos);
		const float invToPosLen  = 1.0f / (toPosLen + bx::kFloatSmallest);
		const bx::Vec3 toPosNorm = bx::mul(toPos, invToPosLen);

		float ll[2];
		bx::toLatLong(&ll[0], &ll[1], toPosNorm);
		ll[0] += consume[0];
		ll[1] -= consume[1];
		ll[1]  = bx::clamp(ll[1], 0.02f, 0.98f);

		const bx::Vec3 tmp  = bx::fromLatLong(ll[0], ll[1]);
		const bx::Vec3 diff = bx::mul(bx::sub(tmp, toPosNorm), toPosLen);

		m_pos.curr = bx::add(m_pos.curr, diff);
		m_pos.dest = bx::add(m_pos.dest, diff);
	}

	// void consumeOrbit(float _amount)
	// {


		// float consume[2];
		// consume[0] = m_orbit[0] * _amount;
		// consume[1] = m_orbit[1] * _amount;
		// m_orbit[0] -= consume[0];
		// m_orbit[1] -= consume[1];

		// const bx::Vec3 toPos     = bx::sub(m_pos.curr, m_target.curr);
    // const float toPosLen     = bx::length(toPos);
    // const float invToPosLen  = 1.0f / (toPosLen + bx::kFloatSmallest);
    // const bx::Vec3 toPosNorm = bx::mul(toPos, invToPosLen);

    // float azimuth   = bx::atan2(toPosNorm.x, toPosNorm.z); // [-π, π]
    // float elevation = bx::asin(toPosNorm.y);               // [-π/2, π/2]

    // // Apply orbit
    // elevation += consume[1];

    // // Detect pole crossing and flip azimuth
    // if (elevation > bx::kPiHalf) {
    //     elevation =  bx::kPi - elevation;
    //     azimuth  += bx::kPi;
    //     consume[0] = -consume[0]; // flip azimuth control
    // } else if (elevation < -bx::kPiHalf) {
    //     elevation = -bx::kPi - elevation;
    //     azimuth  += bx::kPi;
    //     consume[0] = -consume[0]; // flip azimuth control
    // }

    // azimuth += consume[0];
    // // azimuth = bx::wrap(azimuth, bx::kPi2);

    // float cosEl = bx::cos(elevation);
    // bx::Vec3 dir = {
    //     bx::sin(azimuth) * cosEl,
    //     bx::sin(elevation),
    //     bx::cos(azimuth) * cosEl
    // };

    // bx::Vec3 newPos = bx::add(m_target.curr, bx::mul(dir, toPosLen));
    // m_pos.curr = newPos;
    // m_pos.dest = newPos;

		//////////////////////////////////////////////////////////////////
		// quaternion version
		// float yawDelta   = m_orbit[0] * _amount;
    // float pitchDelta = m_orbit[1] * _amount;

    // m_orbit[0] -= yawDelta;
    // m_orbit[1] -= pitchDelta;

    // const bx::Vec3 toPos     = bx::sub(m_pos.curr, m_target.curr);
    // const float dist         = bx::length(toPos);

    // if (dist < 0.0001f)
    //     return;

    // bx::Vec3 toPosNorm = bx::normalize(toPos);

    // // Build incremental rotation: yaw around Y, pitch around X (camera local right)
    // bx::Quaternion qYaw   = bx::rotateY(yawDelta);
    // bx::Vec3 right  = bx::normalize(bx::cross({0.0f, 1.0f, 0.0f}, toPosNorm));
    // bx::Quaternion qPitch = bx::fromAxisAngle(right, pitchDelta);

    // // Combined rotation
    // bx::Quaternion qDelta = bx::mul(qPitch, qYaw);

    // // Rotate direction vector
    // bx::Vec3 rotated = bx::mul(toPosNorm, qDelta);
    // rotated = bx::mul(rotated, dist);

    // // Update camera position
    // m_pos.curr = bx::add(m_target.curr, rotated);
    // m_pos.dest = m_pos.curr;

		///////////////////////////////////////////////////////////////////

		// float yawDelta   = m_orbit[0] * _amount;
    // float pitchDelta = m_orbit[1] * _amount;

    // m_orbit[0] -= yawDelta;
    // m_orbit[1] -= pitchDelta;

    // // Yaw around world Y
    // bx::Quaternion qYaw = bx::rotateY(yawDelta);

    // // Pitch around camera's local right (derived from current orientation)
    // bx::Vec3 forward = bx::mul({0.0f, 0.0f, 1.0f}, m_accum_orientation);
    // bx::Vec3 right   = bx::normalize(bx::cross({0.0f, 1.0f, 0.0f}, forward));
    // bx::Quaternion qPitch  = bx::fromAxisAngle(right, pitchDelta);

    // // Accumulate orientation
    // m_accum_orientation = bx::normalize(bx::mul(qPitch, bx::mul(qYaw, m_accum_orientation)));

    // // Apply to initial orbit vector
    // float radius = bx::length(bx::sub(m_pos.curr, m_target.curr));
    // bx::Vec3 defaultDir = {0.0f, 0.0f, radius}; // start orbiting along -Z
    // bx::Vec3 newDir     = bx::mul(defaultDir, m_accum_orientation);
    // m_pos.curr = bx::add(m_target.curr, newDir);
    // m_pos.dest = m_pos.curr;
	
	// }

	void update(float _dt)
	{
		const float amount = bx::min(_dt / 0.12f, 1.0f);

		consumeOrbit(amount);

		m_target.curr = bx::lerp(m_target.curr, m_target.dest, amount);
		m_pos.curr    = bx::lerp(m_pos.curr,    m_pos.dest,    amount);
	}

	void envViewMtx(float* _mtx)
	{
		const bx::Vec3 toTarget     = bx::sub(m_target.curr, m_pos.curr);
		const float toTargetLen     = bx::length(toTarget);
		const float invToTargetLen  = 1.0f / (toTargetLen + bx::kFloatSmallest);
		const bx::Vec3 toTargetNorm = bx::mul(toTarget, invToTargetLen);

		const bx::Vec3 right = bx::normalize(bx::cross({ 0.0f, 1.0f, 0.0f }, toTargetNorm) );
		const bx::Vec3 up    = bx::normalize(bx::cross(toTargetNorm, right) );

		_mtx[ 0] = right.x;
		_mtx[ 1] = right.y;
		_mtx[ 2] = right.z;
		_mtx[ 3] = 0.0f;
		_mtx[ 4] = up.x;
		_mtx[ 5] = up.y;
		_mtx[ 6] = up.z;
		_mtx[ 7] = 0.0f;
		_mtx[ 8] = toTargetNorm.x;
		_mtx[ 9] = toTargetNorm.y;
		_mtx[10] = toTargetNorm.z;
		_mtx[11] = 0.0f;
		_mtx[12] = 0.0f;
		_mtx[13] = 0.0f;
		_mtx[14] = 0.0f;
		_mtx[15] = 1.0f;
	}

	struct Interp3f
	{
		bx::Vec3 curr = bx::InitNone;
		bx::Vec3 dest = bx::InitNone;
	};

	Interp3f m_target;
	Interp3f m_pos;
	float m_orbit[2];
	bx::Quaternion m_accum_orientation;
};
