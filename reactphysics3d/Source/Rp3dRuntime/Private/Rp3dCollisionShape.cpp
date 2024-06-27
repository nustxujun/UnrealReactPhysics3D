#include "Rp3dCollisionShape.h"
#include "Rp3dUtils.h"
#include "Rp3dSystem.h"

#include "reactphysics3d/engine/PhysicsCommon.h"
#include "reactphysics3d/collision/TriangleVertexArray.h"
#include "reactphysics3d/utils/Message.h"

#ifdef RP3D_USE_FIXED
#define VERTEX_TYPE_NAME VERTEX_FIXED_TYPE
#else
#define VERTEX_TYPE_NAME VERTEX_FLOAT_TYPE
#endif

void URp3dCollisionShape::BeginDestroy()
{
    Super::BeginDestroy();
    Shape.Reset();
}


URp3dCollisionShape* URp3dCollisionShape::CreateBoxShape(const FVector& BoxExtents)
{
    auto Shape = RP3D_MAKE_SHARED(BoxShape, UE_TO_RP3D(BoxExtents));
    auto CollisionShape = NewObject<URp3dCollisionShape>(&URp3dSystem::Get());
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

URp3dCollisionShape* URp3dCollisionShape::CreateSphereShape(float Radius)
{
    auto Shape = RP3D_MAKE_SHARED(SphereShape, UE_TO_RP3D(Radius));
    auto CollisionShape = NewObject<URp3dCollisionShape>(&URp3dSystem::Get());
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

URp3dCollisionShape* URp3dCollisionShape::CreateCapsuleShape(float Radius, float HalfHeight)
{
	auto Shape = RP3D_MAKE_SHARED(CapsuleShape, UE_TO_RP3D(Radius), UE_TO_RP3D(HalfHeight - Radius) * 2);
    auto CollisionShape = NewObject<URp3dCollisionShape>(&URp3dSystem::Get());
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

URp3dCollisionShape* URp3dCollisionShape::CreateConvexShape(const FKConvexElem& Convex, const FVector& Scale)
{
    return URp3dConvexShape::Create(Convex,Scale);
}

URp3dCollisionShape* URp3dCollisionShape::CreateConcaveShape(UStaticMesh* Mesh, const FVector& Scale)
{
    return URp3dConcaveShape::Create(Mesh, Scale);
}

URp3dConvexShape* URp3dConvexShape::Create(const FKConvexElem& Convex, const FVector& Scale)
{
    using namespace reactphysics3d;
    auto Shape = NewObject<URp3dConvexShape>(&URp3dSystem::Get());

    TArray<Vector3> VerticesRaw;
    VerticesRaw.Reserve(Convex.VertexData.Num());
    for (auto& Ver : Convex.VertexData)
    {
        VerticesRaw.Add(UE_TO_RP3D(Ver));
    }

    if (Convex.IndexData.Num() == 0)
    {
        VertexArray Vertices(VerticesRaw.GetData(), sizeof(Vector3), VerticesRaw.Num(), VertexArray::DataType::VERTEX_TYPE_NAME);
        std::vector<Message> Msgs;
        auto ConvexMesh = RP3D_MAKE_SHARED(ConvexMesh, Vertices, Msgs);
        if (!ConvexMesh)
        {
            FString Content;
            for (auto& m : Msgs)
            {
                Content += m.text.c_str();
            }
            UE_LOG(LogPhysics, Error, TEXT("%s"), *Content);
            return nullptr;
        }
        
        Shape->Mesh = ConvexMesh;
		Shape->Shape = RP3D_MAKE_SHARED(ConvexMeshShape, ConvexMesh.Get(), FRp3dConverter::ConvertToRP3D_NoScale(Scale));
    }
    else
    {
        const auto NumFaces = Convex.IndexData.Num() / 3;

        TArray<PolygonVertexArray::PolygonFace> PolyFaces;
        PolyFaces.Reserve(NumFaces);
        for (int i = 0; i < NumFaces; ++i)
        {
            PolygonVertexArray::PolygonFace Face;
            Face.nbVertices = 3;
            Face.indexBase = i * 3;
            PolyFaces.Push(MoveTemp(Face));
        }

        PolygonVertexArray Vertices(
            VerticesRaw.Num(), VerticesRaw.GetData(), sizeof(Vector3),
            Convex.IndexData.GetData(),
            4, NumFaces, PolyFaces.GetData(),
            PolygonVertexArray::VertexDataType::VERTEX_TYPE_NAME,
            PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

        std::vector<Message> Msgs;

        auto ConvexMesh = RP3D_MAKE_SHARED(ConvexMesh, Vertices, Msgs);
        if (!ConvexMesh)
        {
            FString Content;
            for (auto& m : Msgs)
            {
                Content += m.text.c_str();
            }
            UE_LOG(LogPhysics, Error, TEXT("%s"), *Content);
            return nullptr;
        }

        Shape->Mesh = ConvexMesh;
		Shape->Shape = RP3D_MAKE_SHARED(ConvexMeshShape, ConvexMesh.Get(), FRp3dConverter::ConvertToRP3D_NoScale(Scale));

    }

    return Shape;
}

void URp3dConvexShape::BeginDestroy()
{
    Super::BeginDestroy();
    Mesh.Reset();
}

URp3dConcaveShape* URp3dConcaveShape::Create(UStaticMesh* Mesh, const FVector& Scale)
{
    using namespace reactphysics3d;
    auto Shape = NewObject<URp3dConcaveShape>(&URp3dSystem::Get());

    auto& MeshData = Mesh->RenderData->LODResources[0];
    auto& VertexBuffer = MeshData.VertexBuffers.PositionVertexBuffer;
    auto& IndexBuffer = MeshData.IndexBuffer;
    
    check(IndexBuffer.AccessStream16());
    
    const auto NumFaces = MeshData.GetNumTriangles();

    TArray<Vector3> VerticesRaw;
    VerticesRaw.Reserve(VertexBuffer.GetNumVertices() );
    for (uint32 i = 0; i < VertexBuffer.GetNumVertices(); ++i)
    {
        auto& Vertex = VertexBuffer.VertexPosition(i);
        VerticesRaw.Add(UE_TO_RP3D(Vertex));
    }

    TriangleVertexArray VertexArray(VertexBuffer.GetNumVertices(), VerticesRaw.GetData(), sizeof(Vector3),
        NumFaces,
        IndexBuffer.Is32Bit() ? (const void*)IndexBuffer.AccessStream32() : (const void*)IndexBuffer.AccessStream16(),
        (IndexBuffer.Is32Bit() ? 4 : 2) * 3,
        TriangleVertexArray::VertexDataType::VERTEX_TYPE_NAME,
        IndexBuffer.Is32Bit() ? TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE : TriangleVertexArray::IndexDataType::INDEX_SHORT_TYPE);
        std::vector<Message> Msgs;

    auto TriangleMesh = RP3D_MAKE_SHARED(TriangleMesh, VertexArray, Msgs);

    if (!TriangleMesh)
    {
        UE_LOG(LogPhysics, Error, TEXT("failed to generate convex mesh with %s :"), *Mesh->GetPathName());
        FString Content;
        for (auto& m : Msgs)
        {
            Content += m.text.c_str();
        }
        UE_LOG(LogPhysics, Error, TEXT("%s"), *Content);
        return nullptr;
    }

    Shape->TriangleMesh = TriangleMesh;
	Shape->Shape = RP3D_MAKE_SHARED(ConcaveMeshShape, TriangleMesh.Get(), FRp3dConverter::ConvertToRP3D_NoScale(Scale));

    return Shape;
}

void URp3dConcaveShape::BeginDestroy()
{
    Super::BeginDestroy();
    TriangleMesh.Reset();
}
