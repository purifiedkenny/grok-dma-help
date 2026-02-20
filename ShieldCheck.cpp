#include "ShieldCheck.h"

struct ShieldGeometry {
	static constexpr float SHIELD_HEIGHT = 45.0f;

	static constexpr float SHIELD_BOTTOM_OFFSET = -45.0f;
	static constexpr float SHIELD_TOP_OFFSET = 120.f;
	static constexpr float SHIELD_FORWARD_OFFSET = 45.f;

	static constexpr int ARC_SEGMENTS = 12;

	Vec3D origin;
	float baseYaw;
	std::vector<Vec3D> bottomArcPoints;
	std::vector<Vec3D> topArcPoints;

	void CalculateGeometry(const Shield* shield) {
		float SHIELD_ARC_ANGLE = 115.F;
		float SHIELD_RADIUS = 120.0f;
		origin = shield->Origin;
		origin.z += SHIELD_BOTTOM_OFFSET;
		Vec3D rotation = shield->rotation;

		baseYaw = (rotation.y + 180.f) * M_PI / 180.0f;
		float cosYaw = cos(baseYaw);
		float sinYaw = sin(baseYaw);

		if (shield->Origin.distance2dTo(localPlayer->CameraOrigin) >= 80.f)
		{
			origin.x -= SHIELD_FORWARD_OFFSET * cosYaw;
			origin.y -= SHIELD_FORWARD_OFFSET * sinYaw;
		}
		else
		{
			SHIELD_ARC_ANGLE = 180.f;
			origin.x += 50.F * cosYaw;
			origin.y += 50.F * sinYaw;
		}
		bottomArcPoints.clear();
		topArcPoints.clear();

		float halfArcAngle = (SHIELD_ARC_ANGLE / 2.0f) * M_PI / 180.0f;
		float startAngle = baseYaw - halfArcAngle;
		float endAngle = baseYaw + halfArcAngle;
		float angleStep = (endAngle - startAngle) / ARC_SEGMENTS;

		for (int i = 0; i <= ARC_SEGMENTS; i++) {
			float currentAngle = startAngle + angleStep * i;
			float cosA = cos(currentAngle);
			float sinA = sin(currentAngle);

			bottomArcPoints.push_back({
				origin.x + SHIELD_RADIUS * cosA,
				origin.y + SHIELD_RADIUS * sinA,
				origin.z
				});

			topArcPoints.push_back({
				origin.x + SHIELD_RADIUS * cosA,
				origin.y + SHIELD_RADIUS * sinA,
				origin.z + SHIELD_HEIGHT + SHIELD_TOP_OFFSET
				});
		}
	}
};


bool ShieldTool::IsLineThroughSphere(
	const Vec3D& start,
	const Vec3D& end,
	const Vec3D& sphereCenter)
{
	const float CHECK_RADIUS = 240.0f;
	const float INTERSECT_RADIUS = 220.f;

	auto getDistanceXYSqr = [](const Vec3D& point, const Vec3D& center) {
		return (point.x - center.x) * (point.x - center.x)
			+ (point.y - center.y) * (point.y - center.y);
		};

	float startDistSqr = getDistanceXYSqr(start, sphereCenter);
	float endDistSqr = getDistanceXYSqr(end, sphereCenter);
	float checkRadiusSqr = CHECK_RADIUS * CHECK_RADIUS;

	bool checkbool = (startDistSqr <= checkRadiusSqr && endDistSqr <= checkRadiusSqr) ? false : (startDistSqr <= checkRadiusSqr || endDistSqr <= checkRadiusSqr);
	if (checkbool) {
		return true;
	}

	Vec3D dir = {
		end.x - start.x,
		end.y - start.y,
		end.z - start.z
	};

	Vec3D oc = {
		start.x - sphereCenter.x,
		start.y - sphereCenter.y,
		start.z - sphereCenter.z
	};

	float intersectRadiusSqr = INTERSECT_RADIUS * INTERSECT_RADIUS;

	float a = dir.x * dir.x + dir.y * dir.y;
	float b = 2.0f * (oc.x * dir.x + oc.y * dir.y);
	float c = (oc.x * oc.x + oc.y * oc.y) - intersectRadiusSqr;

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		return false;
	}

	float t1 = (-b - sqrt(discriminant)) / (2 * a);
	float t2 = (-b + sqrt(discriminant)) / (2 * a);

	return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

bool ShieldTool::IsInRampartShieldProtection(
	const Vec3D& cameraPos,
	const Vec2D& localView,
	const Vec3D& targetPos,
	const Vec3D& shieldOrigin,
	const Vec3D& rotation)
{
	const float SHIELD_HEIGHT = 45.0f;
	const float SHIELD_WIDTH = 120.0f;
	const float MIN_DISTANCE = 30.0f;  // 最小距离检查
	const float MAX_DISTANCE = esp->enemies.distance;  // 最大距离检查

	// 1. 距离检查
	float distanceToShield = shieldOrigin.distance2dTo(cameraPos);
	if (distanceToShield > MAX_DISTANCE) return false;

	// 2. 计算护盾朝向向量
	float yaw = (rotation.y) * M_PI / 180.0f;
	Vec3D shieldForward = {
		cos(yaw),
		sin(yaw),
		0
	};

	Vec3D shieldRight = {
		-shieldForward.y,
		shieldForward.x,
		0
	};

	// 3. 计算相对于护盾的位置向量
	Vec3D toCameraXY = {
		cameraPos.x - shieldOrigin.x,
		cameraPos.y - shieldOrigin.y,
		0
	};

	Vec3D toTargetXY = {
		targetPos.x - shieldOrigin.x,
		targetPos.y - shieldOrigin.y,
		0
	};

	// 4. 计算相对于护盾面的位置
	float cameraDot = toCameraXY.x * shieldForward.x + toCameraXY.y * shieldForward.y;
	float targetDot = toTargetXY.x * shieldForward.x + toTargetXY.y * shieldForward.y;

	// 如果相机和目标在护盾同一侧，则不阻挡
	if ((cameraDot > 0 && targetDot > 0) || (cameraDot < 0 && targetDot < 0)) {
		return false;
	}

	// 5. 高度检查
	float shieldTopZ = shieldOrigin.z + SHIELD_HEIGHT;
	if (targetPos.z < shieldOrigin.z || targetPos.z > shieldTopZ) {
		return false;
	}

	// 6. 计算护盾平面上的投影点
	Vec3D faceCenter = shieldOrigin;
	Vec3D corners[4];

	// 计算护盾四个角点
	corners[0] = {
		faceCenter.x - shieldRight.x * (SHIELD_WIDTH / 2),
		faceCenter.y - shieldRight.y * (SHIELD_WIDTH / 2),
		faceCenter.z
	};

	corners[1] = {
		faceCenter.x + shieldRight.x * (SHIELD_WIDTH / 2),
		faceCenter.y + shieldRight.y * (SHIELD_WIDTH / 2),
		faceCenter.z
	};

	corners[2] = {
		corners[1].x,
		corners[1].y,
		faceCenter.z + SHIELD_HEIGHT
	};

	corners[3] = {
		corners[0].x,
		corners[0].y,
		faceCenter.z + SHIELD_HEIGHT
	};

	// 7. 转换到屏幕坐标
	Vec2D screenCorners[4];
	bool allCornersVisible = true;
	for (int i = 0; i < 4; i++) {
		if (!gameView->WorldToScreenNoClip(corners[i], screenCorners[i])) {
			allCornersVisible = false;
			break;
		}
	}

	if (!allCornersVisible) return false;

	Vec2D targetScreen;
	if (!gameView->WorldToScreen(targetPos, targetScreen)) {
		return false;
	}

	// 8. 点在多边形内检测
	int intersections = 0;
	for (int i = 0; i < 4; i++) {
		const Vec2D& current = screenCorners[i];
		const Vec2D& next = screenCorners[(i + 1) % 4];

		if (((current.y > targetScreen.y) != (next.y > targetScreen.y)) &&
			(targetScreen.x < (next.x - current.x) * (targetScreen.y - current.y) /
				(next.y - current.y) + current.x)) {
			intersections++;
		}
	}

	return (intersections % 2 == 1);
}

bool ShieldTool::IsPointInScreenPolygon(const std::vector<Vec2D>& screenPoints, const Vec2D& testPoint) {
	int intersections = 0;
	size_t numPoints = screenPoints.size();

	for (size_t i = 0; i < numPoints; i++) {
		const Vec2D& current = screenPoints[i];
		const Vec2D& next = screenPoints[(i + 1) % numPoints];

		if (((current.y > testPoint.y) != (next.y > testPoint.y)) &&
			(testPoint.x < (next.x - current.x) * (testPoint.y - current.y) /
				(next.y - current.y) + current.x)) {
			intersections++;
		}
	}

	return (intersections % 2) == 1;
}

bool ShieldTool::IsBlockedByNewcastleShield(Shield* shield, const Vec3D& bulletStart, const Vec3D& bulletEnd) {
	ShieldGeometry shieldGeo;
	shieldGeo.CalculateGeometry(shield);

	std::vector<Vec2D> screenPoints;

	for (const auto& point : shieldGeo.bottomArcPoints) {
		Vec2D screenPoint;
		gameView->WorldToScreenNoClip(point, screenPoint);
		screenPoints.push_back(screenPoint);
	}

	for (auto it = shieldGeo.topArcPoints.rbegin(); it != shieldGeo.topArcPoints.rend(); ++it) {
		Vec2D screenPoint;
		gameView->WorldToScreenNoClip(*it, screenPoint);
		screenPoints.push_back(screenPoint);
	}

	Vec2D endScreen;
	if (!gameView->WorldToScreen(bulletEnd, endScreen)) {
		return false;
	}


	bool isInside = IsPointInScreenPolygon(screenPoints, endScreen);

	return isInside;
}

bool ShieldTool::IsBlockedByLifeLineShield(
	const Vec3D& start,
	const Vec3D& end,
	const Vec3D& shieldCenter)
{
	static constexpr float SHIELD_HEIGHT = 180.0f;
	static constexpr float SHIELD_RADIUS = 250.0f;
	static constexpr int NUM_SEGMENTS = 32;


	static std::vector<Vec2D> circlePoints;
	if (circlePoints.capacity() < NUM_SEGMENTS + 1) {
		circlePoints.reserve(NUM_SEGMENTS + 1);
	}
	circlePoints.clear();

	Vec3D topCenter = shieldCenter;
	topCenter.z += SHIELD_HEIGHT;

	Vec2D endScreen;
	if (!gameView->WorldToScreen(end, endScreen)) {
		return false;
	}


	for (int i = 0; i <= NUM_SEGMENTS; i++) {
		float theta = (2 * M_PI * i) / NUM_SEGMENTS;
		Vec3D worldPoint = {
			topCenter.x + SHIELD_RADIUS * cosf(theta),
			topCenter.y + SHIELD_RADIUS * sinf(theta),
			topCenter.z
		};

		Vec2D screenPoint;
		if (gameView->WorldToScreenNoClip(worldPoint, screenPoint)) {
			circlePoints.push_back(screenPoint);
		}
	}

	if (circlePoints.size() < 3) {
		return false;
	}

	bool inside = false;
	for (size_t i = 0, j = circlePoints.size() - 1; i < circlePoints.size(); j = i++) {
		if (((circlePoints[i].y > endScreen.y) != (circlePoints[j].y > endScreen.y)) &&
			(endScreen.x < (circlePoints[j].x - circlePoints[i].x) *
				(endScreen.y - circlePoints[i].y) / (circlePoints[j].y - circlePoints[i].y) +
				circlePoints[i].x)) {
			inside = !inside;
		}
	}

	Vec3D dir = {
	   end.x - start.x,
	   end.y - start.y,
	   end.z - start.z
	};

	Vec3D oc = {
		start.x - shieldCenter.x,
		start.y - shieldCenter.y,
		start.z - shieldCenter.z
	};


	float intersectRadiusSqr = SHIELD_RADIUS * SHIELD_RADIUS;


	float a = dir.x * dir.x + dir.y * dir.y;
	float b = 2.0f * (oc.x * dir.x + oc.y * dir.y);
	float c = (oc.x * oc.x + oc.y * oc.y) - intersectRadiusSqr;

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		// If no sphere intersection, proceed with cylinder check
	}
	else {

		float t1 = (-b - sqrt(discriminant)) / (2 * a);
		float t2 = (-b + sqrt(discriminant)) / (2 * a);


		if ((t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1)) {
			return true;
		}
	}


	auto isInsideCylinder = [&](const Vec3D& point) -> bool {
		float dx = point.x - shieldCenter.x;
		float dy = point.y - shieldCenter.y;
		float distSqr = dx * dx + dy * dy;
		float height = point.z - shieldCenter.z;

		return distSqr <= (SHIELD_RADIUS * SHIELD_RADIUS) &&
			height >= 0 && height <= SHIELD_HEIGHT;
		};

	bool startInside = isInsideCylinder(start);
	bool endInside = isInsideCylinder(end);

	if (startInside && endInside) {
		return false;
	}

	if (inside && endInside) {
		return false;
	}

	if (startInside || endInside) {
		return true;
	}

	return false;
}