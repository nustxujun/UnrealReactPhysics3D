#include "Rp3dTriangleMeshComponent.h"
#include "Rp3dSystem.h"
#include "Rp3dUtils.h"
#include "StaticMeshResources.h"




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
	if (!Mesh)
		return;

	using namespace reactphysics3d;

	auto& PhysicsCommon = URp3dSystem::Get().GetRp3dPhysicsCommon();

	ClearShapes();

	if (bIsConvexMesh)
	{
		for (auto& Box : Mesh->BodySetup->AggGeom.BoxElems)
		{
			auto Trans = Box.GetTransform();
			AddCollisionShape(URp3dCollisionShape::CreateBoxShape(FVector(Box.X, Box.Y, Box.Z) / 2), Trans);
		}

		for (auto& Sphere : Mesh->BodySetup->AggGeom.SphereElems)
		{
			auto Trans = Sphere.GetTransform();
			AddCollisionShape( URp3dCollisionShape::CreateSphereShape(Sphere.Radius), Trans );
		}

		for (auto& Capsule : Mesh->BodySetup->AggGeom.SphylElems)
		{
			auto Trans = Capsule.GetTransform();
			AddCollisionShape(URp3dCollisionShape::CreateCapsuleShape(Capsule.Radius, Capsule.Length), FTransform(FRotator(0,0,90)) * Trans );
		}

		for (auto& Convex : Mesh->BodySetup->AggGeom.ConvexElems)
		{
			AddCollisionShape(URp3dCollisionShape::CreateConvexShape(
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
		Vertices = MoveTemp(MeshData.Vertices);
#endif
		check(Vertices.Num() != 0 && Indices.Num() != 0);
		AddCollisionShape(URp3dCollisionShape::CreateConcaveShape(Vertices, Indices,GetComponentTransform().GetScale3D()), FTransform::Identity);
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
				auto& Geoms = InComponent->Mesh->BodySetup->AggGeom;

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

				// uint32 BaseIndex = 0;
				// for (auto& Convex: InComponent->PhysicsConvexMeshs)
				// {
				// 	auto& Mesh = *Convex;
				// 	Vertices.Reserve(Mesh.getNbVertices());
				// 	for (uint32 i = 0; i < Mesh.getNbVertices(); ++i)
				// 	{
				// 		Vertices.Push(RP3D_TO_UE(Mesh.getVertex(i)));
				// 	}

				// 	struct Line
				// 	{
				// 		uint32 a;
				// 		uint32 b;
				// 	};

				// 	auto GetKey = [](auto a, auto b) {
				// 		auto Min = FMath::Min(a, b);
				// 		auto Max = FMath::Max(a, b);
				// 		return (uint64(a) << 32) | uint64(b);
				// 	};

				// 	TSortedMap<uint64, Line > Lines;
				// 	Lines.Reserve(Mesh.getNbFaces() * 3);
				// 	auto& Info = Mesh.getHalfEdgeStructure();
				// 	for (uint32 i = 0; i < Info.getNbFaces(); i++)
				// 	{
				// 		auto& Face = Info.getFace(i);
				// 		auto i1 = Face.faceVertices[0] + BaseIndex;
				// 		auto i2 = Face.faceVertices[1] + BaseIndex;
				// 		auto i3 = Face.faceVertices[2] + BaseIndex;


				// 		Lines.Add(GetKey(i1, i2), { i1, i2 });
				// 		Lines.Add(GetKey(i1, i3), { i1, i3 });
				// 		Lines.Add(GetKey(i2, i3), { i2, i3 });
				// 	}

				// 	for (auto& L : Lines)
				// 	{
				// 		Indices.Add(L.Value.a);
				// 		Indices.Add(L.Value.b);
				// 	}

				// 	BaseIndex = Vertices.Num();
				// }
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

				Vertices = MeshData.Vertices;
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
