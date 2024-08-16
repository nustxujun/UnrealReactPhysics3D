#include "Rp3dSphereComponent.h"
#include "Rp3dSystem.h"
#include "Rp3dUtils.h"

#include "reactphysics3d/engine/PhysicsCommon.h"


void URp3dSphereComponent::SetSphereRadius(float InSphereRadius, bool bUpdateOverlaps )
{
	if (InSphereRadius == SphereRadius)
		return ;
	SphereRadius = InSphereRadius;
	UpdateBounds();
	MarkRenderStateDirty();

	OnCreateRp3dState({});
}

void URp3dSphereComponent::UpdateCollisionShape()
{
	ClearShapes();
	AddCollisionShape(Rp3dCollisionShape::CreateSphereShape(SphereRadius) , FTransform::Identity );
}




FPrimitiveSceneProxy* URp3dSphereComponent::CreateSceneProxy()
{
	/** Represents a DrawLightRadiusComponent to the scene manager. */
	class FSphereSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		/** Initialization constructor. */
		FSphereSceneProxy(const URp3dSphereComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(true)
			, SphereColor(FColor::Green)
			, SphereRadius(InComponent->SphereRadius)
		{
			bWillEverBeLit = false;
		}

		// FPrimitiveSceneProxy interface.

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SphereSceneProxy_GetDynamicMeshElements);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FMatrix& LocalToWorld = GetLocalToWorld();
					const FLinearColor DrawSphereColor = GetViewSelectionColor(SphereColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					// Taking into account the min and maximum drawing distance
					const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalToWorld.GetOrigin()).SizeSquared();
					if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
					{
						continue;
					}

					float AbsScaleX = LocalToWorld.GetScaledAxis(EAxis::X).Size();
					float AbsScaleY = LocalToWorld.GetScaledAxis(EAxis::Y).Size();
					float AbsScaleZ = LocalToWorld.GetScaledAxis(EAxis::Z).Size();
					float MinAbsScale = FMath::Min3(AbsScaleX, AbsScaleY, AbsScaleZ);

					FVector ScaledX = LocalToWorld.GetUnitAxis(EAxis::X) * MinAbsScale;
					FVector ScaledY = LocalToWorld.GetUnitAxis(EAxis::Y) * MinAbsScale;
					FVector ScaledZ = LocalToWorld.GetUnitAxis(EAxis::Z) * MinAbsScale;

					const int32 SphereSides = FMath::Clamp<int32>(SphereRadius / 4.f, 16, 64);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledY, DrawSphereColor, SphereRadius, SphereSides, SDPG_World, 2.0f);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World, 2.0f);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledY, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World, 2.0f);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleForSelection = !bDrawOnlyIfSelected || IsSelected();
			const bool bVisibleForShowFlags = true; // @TODO

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bVisibleForSelection && bVisibleForShowFlags) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32				bDrawOnlyIfSelected : 1;
		const FColor				SphereColor;
		const float					SphereRadius;
	};

	return new FSphereSceneProxy(this);
}


bool URp3dSphereComponent::IsZeroExtent() const {
	return SphereRadius == 0;
}
FCollisionShape URp3dSphereComponent::GetCollisionShape(float Inflation ) const {
	return {};
}

bool URp3dSphereComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const {
	return Scale3D.GetAbs().AllComponentsEqual() || A.Equals(B);
}

FBoxSphereBounds URp3dSphereComponent::CalcBounds(const FTransform& LocalToWorld) const 
{
	return FBoxSphereBounds(FVector::ZeroVector, FVector(SphereRadius), SphereRadius).TransformBy(LocalToWorld);
}

void URp3dSphereComponent::CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const
{
	CylinderRadius = SphereRadius * GetComponentTransform().GetMaximumAxisScale();
	CylinderHalfHeight = CylinderRadius;
}

