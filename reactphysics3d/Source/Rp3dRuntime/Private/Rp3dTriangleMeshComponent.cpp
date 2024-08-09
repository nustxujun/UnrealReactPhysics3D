#include "Rp3dTriangleMeshComponent.h"
#include "Rp3dSystem.h"
#include "Rp3dUtils.h"
#include "StaticMeshResources.h"

#if ENGINE_MAJOR_VERSION >= 5 || ENGINE_MINOR_VERSION >= 27
#define GET_BODYSETUP() GetBodySetup()
#include "PhysicsEngine/BodySetup.h"
#else
#define GET_BODYSETUP() BodySetup
#endif


#if WITH_EDITOR
void URp3dTriangleMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ClearShapes();
	UpdateCollisionShape();


}
#endif

void URp3dTriangleMeshComponent::UpdateCollisionShape()
{
	if (!IsInGameThread() || !Mesh)
		return;

	using namespace reactphysics3d;

	ClearShapes();

	if (bIsConvexMesh)
	{
		for (auto& Box : Mesh->GET_BODYSETUP()->AggGeom.BoxElems)
		{
			auto Trans = Box.GetTransform();
			AddCollisionShape(Rp3dCollisionShape::CreateBoxShape(FVector(Box.X, Box.Y, Box.Z) / 2), Trans);
		}

		for (auto& Sphere : Mesh->GET_BODYSETUP()->AggGeom.SphereElems)
		{
			auto Trans = Sphere.GetTransform();
			AddCollisionShape( Rp3dCollisionShape::CreateSphereShape(Sphere.Radius), Trans );
		}

		for (auto& Capsule : Mesh->GET_BODYSETUP()->AggGeom.SphylElems)
		{
			auto Trans = Capsule.GetTransform();
			AddCollisionShape(Rp3dCollisionShape::CreateCapsuleShape(Capsule.Radius, Capsule.Length), FTransform(FRotator(0,0,90)) * Trans );
		}

		for (auto& Convex : Mesh->GET_BODYSETUP()->AggGeom.ConvexElems)
		{
			AddCollisionShape(Rp3dCollisionShape::CreateConvexShape(
				Convex,
				GetComponentTransform().GetScale3D()),
				Convex.GetTransform());
		}

	}
	else
	{
#if WITH_EDITOR
		FTriMeshCollisionData MeshData;
		Mesh->GetPhysicsTriMeshData(&MeshData, true);
		Indices.Reset();
		Indices.Reserve(MeshData.Indices.Num() * 3);
		for (auto& Face : MeshData.Indices)
		{
			Indices.Add(Face.v0);
			Indices.Add(Face.v1);
			Indices.Add(Face.v2);
		}

#if ENGINE_MAJOR_VERSION >= 5
		Vertices.Reset();
		for (auto& Ver : MeshData.Vertices)
		{
			Vertices.Add(FVector(Ver));
		}
#else
		Vertices = MoveTemp(MeshData.Vertices);
#endif
#endif
		check(Vertices.Num() != 0 && Indices.Num() != 0);
		AddCollisionShape(Rp3dCollisionShape::CreateConcaveShape(Vertices, Indices,GetComponentTransform().GetScale3D()), FTransform::Identity);
	}


}


FPrimitiveSceneProxy* URp3dTriangleMeshComponent::CreateSceneProxy()
{
	class FTriangleMeshSceneProxy : public FPrimitiveSceneProxy
	{
		TArray<FVector> Vertices;
		TArray<uint32> Indices;

		TArray<FVector> BoxExtents;
		TArray<FTransform> BoxTrans;
		TArray<float> SphereRadius;
		TArray<FVector> SphereOffset;

		TArray<FVector2D> Capsules;
		TArray<FTransform> CapsuleTrans;

		FColor Color = FColor::Green;
	public:

		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FTriangleMeshSceneProxy(const URp3dTriangleMeshComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
		{

			if (InComponent->bIsConvexMesh )
			{
				if (!InComponent->Mesh)
					return;
				auto& Geoms = InComponent->Mesh->GET_BODYSETUP()->AggGeom;

				for (auto& Box : Geoms.BoxElems)
				{
					BoxExtents.Add(FVector(Box.X, Box.Y, Box.Z) * 0.5f);
					BoxTrans.Add(Box.GetTransform());
				}

				for (auto& Sphere : Geoms.SphereElems)
				{
					SphereRadius.Add(Sphere.Radius);
					SphereOffset.Add(Sphere.Center);
				}

				for (auto& Cap : Geoms.SphylElems)
				{
					Capsules.Add({Cap.Radius, Cap.Length });
					CapsuleTrans.Add(Cap.GetTransform());
				}

			}
			else
			{
				if (!InComponent->Mesh)
					return;
				FTriMeshCollisionData MeshData;
				InComponent->Mesh->GetPhysicsTriMeshData(&MeshData, true);

				struct Line
				{
					int32 a;
					int32 b;
				};

				auto GetKey = [](auto a, auto b) {
					auto Min = FMath::Min(a, b);
					auto Max = FMath::Max(a, b);
					return (uint64(a) << 32) | uint64(b);
				};

				TSortedMap<uint64, Line > Lines;
				Lines.Reserve(MeshData.Indices.Num() * 3);
				for (int32 i = 0; i < MeshData.Indices.Num(); ++i)
				{
					auto& Face = MeshData.Indices[i];
					Lines.Add(GetKey(Face.v0, Face.v1), { Face.v0, Face.v1 });
					Lines.Add(GetKey(Face.v0, Face.v2), { Face.v0, Face.v2 });
					Lines.Add(GetKey(Face.v2, Face.v1), { Face.v2, Face.v1 });

				}

				for (auto& L : Lines)
				{
					Indices.Add(L.Value.a);
					Indices.Add(L.Value.b);
				}

#if ENGINE_MAJOR_VERSION >= 5
				Vertices.Reset();
				for (auto& Ver : MeshData.Vertices)
				{
					Vertices.Add(FVector(Ver));
				}
#else
				Vertices = MoveTemp(MeshData.Vertices);
#endif
			}
		}


		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			const FMatrix& LocalToWorld = GetLocalToWorld();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					const FLinearColor DrawColor = GetViewSelectionColor(Color, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					auto Beg = Indices.GetData();
					auto End = Beg + Indices.Num();
					for (; Beg != End; )
					{
						auto P1 = Vertices[*(Beg++)];
						auto P2 = Vertices[*(Beg++)];
						P1 = LocalToWorld.TransformPosition(P1);
						P2 = LocalToWorld.TransformPosition(P2);

						PDI->DrawLine(P1, P2, DrawColor, SDPG_World, 2.0f);
					}


					for (int i = 0; i < BoxExtents.Num(); ++i)
					{
						FMatrix Trans =  BoxTrans[i].ToMatrixNoScale() * LocalToWorld;
						DrawOrientedWireBox(PDI, Trans.GetOrigin(), Trans.GetScaledAxis(EAxis::X), Trans.GetScaledAxis(EAxis::Y), Trans.GetScaledAxis(EAxis::Z), BoxExtents[i], DrawColor, SDPG_World, 2.0f);
					}

					for (int i = 0; i < SphereRadius.Num(); ++i)
					{

						const int32 SphereSides = FMath::Clamp<int32>(SphereRadius[i] / 4.f, 16, 64);

						DrawWireSphere(PDI, LocalToWorld.GetOrigin() + SphereOffset[i], DrawColor, SphereRadius[i], SphereSides,SDPG_World,2.0f);
					}

					for (int i = 0; i < Capsules.Num(); ++i)
					{
						FMatrix Trans = CapsuleTrans[i].ToMatrixNoScale() * LocalToWorld;
						const int32 CapsuleSides = FMath::Clamp<int32>(Capsules[i].Y / 4.f, 16, 64);

						DrawWireCapsule(PDI, Trans.GetOrigin(), Trans.GetScaledAxis(EAxis::X), Trans.GetScaledAxis(EAxis::Y), Trans.GetScaledAxis(EAxis::Z), DrawColor, Capsules[i].X, Capsules[i].Y, CapsuleSides, SDPG_World, 2.0f);

					}


				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }
	};
	UpdateCollisionShape();
	return new FTriangleMeshSceneProxy(this);
}

const static FGuid Rp3dTriangleMeshGUID(0x21d23c94, 0xf4a747f3, 0xB4FD92DB,0xBD4CF9F2);
const static int Rp3dTriangleMeshLatestVersion = 1;
static FCustomVersionRegistration RegisterRp3dTriangleMeshVersion(Rp3dTriangleMeshGUID, Rp3dTriangleMeshLatestVersion, TEXT("Rp3dTriangleMesh"));

void URp3dTriangleMeshComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (bIsConvexMesh || !Mesh)
		return;

	Ar.UsingCustomVersion(Rp3dTriangleMeshGUID);

	if (Ar.IsLoading() && Ar.CustomVer(Rp3dTriangleMeshGUID) < Rp3dTriangleMeshLatestVersion)
	{
		UE_LOG(LogPhysics, Warning, TEXT("Rp3dTriangle Mesh is updated."));
		return;
	}

	Ar << Vertices;
	Ar << Indices;
}
