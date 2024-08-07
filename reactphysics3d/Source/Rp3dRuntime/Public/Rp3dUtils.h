#pragma once 

#include "CoreMinimal.h"
#include <string>
#include "reactphysics3d/mathematics/Vector3.h"
#include "reactphysics3d/mathematics/Transform.h"
#include "reactphysics3d/mathematics/Quaternion.h"


#ifdef RP3D_USE_FIXED

#define SCALE_TO_RP3D(VAL) reactphysics3d::decimal::from_raw(VAL.raw_value() >> Shift)
#define SCALE_TO_UE(VAL) float(reactphysics3d::decimal::from_raw(VAL.raw_value() << Shift))

#else

#define SCALE_TO_RP3D(VAL) VAL * Scale
#define SCALE_TO_UE(VAL) VAL * Inv_Scale

#endif

class FRp3dConverter
{
	static constexpr int Shift = 7;
	static constexpr int Inv_Scale = 1 << Shift;
	static constexpr reactphysics3d::decimal Scale = 1.0 / Inv_Scale;

public:
	static std::string ConvertToRP3D(const FString& Str);

	inline static reactphysics3d::Vector3 ConvertToRP3D(const FVector& Vector)
	{
		return {
			SCALE_TO_RP3D(reactphysics3d::decimal(Vector.X)),
			SCALE_TO_RP3D(reactphysics3d::decimal(Vector.Y)),
			SCALE_TO_RP3D(reactphysics3d::decimal(Vector.Z)) };
	}

	inline static reactphysics3d::Vector3 ConvertToRP3D_NoScale(const FVector& Vector)
	{
		return {
			(reactphysics3d::decimal(Vector.X)),
			(reactphysics3d::decimal(Vector.Y)),
			(reactphysics3d::decimal(Vector.Z)) };
	}

	inline static FVector ConvertToUE(const reactphysics3d::Vector3& Vector)
	{
		return {
			SCALE_TO_UE(Vector.x) ,
			SCALE_TO_UE(Vector.y) ,
			SCALE_TO_UE(Vector.z)
		};
	}

	inline static reactphysics3d::Transform ConvertToRP3D(const FTransform& Trans)
	{
		return { ConvertToRP3D(Trans.GetLocation()),ConvertToRP3D(FQuat(Trans.GetRotation())) };

	}

	inline static FTransform ConvertToUE(const reactphysics3d::Transform& Trans)
	{
		return { ConvertToUE(Trans.getOrientation()),ConvertToUE(Trans.getPosition()) };
	}

	inline static reactphysics3d::Quaternion ConvertToRP3D(const FQuat& Quat)
	{
		return { reactphysics3d::decimal(Quat.X), reactphysics3d::decimal(Quat.Y), reactphysics3d::decimal(Quat.Z),reactphysics3d::decimal(Quat.W) };
	}

	inline static FQuat ConvertToUE(const reactphysics3d::Quaternion& Quat)
	{
		return {
			float(Quat.x),
			float(Quat.y),
			float(Quat.z),
			float(Quat.w),
		};
	}

	inline static reactphysics3d::decimal ConvertToRP3D(float Val)
	{
		return SCALE_TO_RP3D(reactphysics3d::decimal(Val));
	}

	inline static float ConvertToUE(reactphysics3d::decimal Val)
	{
		return SCALE_TO_UE(Val);
	}


};



#define UE_TO_RP3D(x) FRp3dConverter::ConvertToRP3D(x)
#define RP3D_TO_UE(x) FRp3dConverter::ConvertToUE(x)




class FRp3dUtils
{
public:
	template<class T, class F>
	static inline TSharedPtr<T> MakeShared(T* Pointer, F&& Deleter)
	{
		return TSharedPtr<T>(Pointer, MoveTemp(Deleter));
	}
};

#define RP3D_MAKE_SHARED(Obj, ...) FRp3dUtils::MakeShared(URp3dSystem::Get().GetRp3dPhysicsCommon()->create##Obj(__VA_ARGS__), [PhysicsCommon = URp3dSystem::Get().GetRp3dPhysicsCommon()](auto Val){if (Val) PhysicsCommon->destroy##Obj(Val);});