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



Rp3dCollisionShape::Ptr Rp3dCollisionShape::CreateBoxShape(const FVector& BoxExtents)
{
    auto Shape = RP3D_MAKE_SHARED(BoxShape, UE_TO_RP3D(BoxExtents));
    auto CollisionShape = MakeShared<Rp3dCollisionShape>();
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

Rp3dCollisionShape::Ptr Rp3dCollisionShape::CreateSphereShape(float Radius)
{
    auto Shape = RP3D_MAKE_SHARED(SphereShape, UE_TO_RP3D(Radius));
    auto CollisionShape = MakeShared<Rp3dCollisionShape>();
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

Rp3dCollisionShape::Ptr Rp3dCollisionShape::CreateCapsuleShape(float Radius, float HalfHeight)
{
	auto Shape = RP3D_MAKE_SHARED(CapsuleShape, UE_TO_RP3D(Radius), UE_TO_RP3D(HalfHeight - Radius) * 2);
    auto CollisionShape = MakeShared<Rp3dCollisionShape>();
    CollisionShape->Shape = Shape;
    return CollisionShape;
}

Rp3dCollisionShape::Ptr Rp3dCollisionShape::CreateConvexShape(const FKConvexElem& Convex, const FVector& Scale)
{
    return Rp3dConvexShape::Create(Convex,Scale);
}

Rp3dCollisionShape::Ptr Rp3dCollisionShape::CreateConcaveShape(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale)
{
    return Rp3dConcaveShape::Create(Vertices, Indices, Scale);
}

TSharedPtr<Rp3dConvexShape> Rp3dConvexShape::Create(const FKConvexElem& Convex, const FVector& Scale)
{
    using namespace reactphysics3d;
    auto Shape = MakeShared< Rp3dConvexShape>();

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


TSharedPtr<Rp3dConcaveShape> Rp3dConcaveShape::Create(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale)
{
    using namespace reactphysics3d;
    auto Shape = MakeShared<Rp3dConcaveShape>();

    const auto NumFaces = Indices.Num() / 3 ;

    TArray<Vector3> VerticesRaw;
    VerticesRaw.Reserve(Vertices.Num() );
    for (int32 i = 0; i < Vertices.Num(); ++i)
    {
        auto& Vertex = Vertices[i];
        VerticesRaw.Add(UE_TO_RP3D(Vertex));
    }

    TriangleVertexArray VertexArray(Vertices.Num(), VerticesRaw.GetData(), sizeof(Vector3),
        NumFaces,
        Indices.GetData(),
        4 * 3,
        TriangleVertexArray::VertexDataType::VERTEX_TYPE_NAME,
        TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
        std::vector<Message> Msgs;

    auto TriangleMesh = RP3D_MAKE_SHARED(TriangleMesh, VertexArray, Msgs);

    if (!TriangleMesh)
    {
        UE_LOG(LogPhysics, Error, TEXT("failed to generate concave mesh "));
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

