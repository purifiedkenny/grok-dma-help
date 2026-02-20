#pragma once
#include "include.h"

namespace ShieldTool
{
	bool IsLineThroughSphere(
		const Vec3D& start,
		const Vec3D& end,
		const Vec3D& sphereCenter);

	bool IsInRampartShieldProtection(
		const Vec3D& cameraPos,
		const Vec2D& localView,
		const Vec3D& targetPos,
		const Vec3D& shieldOrigin,
		const Vec3D& rotation);

	bool IsPointInScreenPolygon(
		const std::vector<Vec2D>& screenPoints,
		const Vec2D& testPoint);

	bool IsBlockedByNewcastleShield(
		Shield* shield,
		const Vec3D& bulletStart,
		const Vec3D& bulletEnd);
	
	bool IsBlockedByLifeLineShield(
		const Vec3D& start,
		const Vec3D& end,
		const Vec3D& shieldCenter);
}