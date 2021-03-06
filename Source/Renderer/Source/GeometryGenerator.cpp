//	VQEngine | DirectX11 Renderer
//	Copyright(C) 2018  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com

#include "GeometryGenerator.h"
#include "Renderer.h"

#include "Utilities/Log.h"

#include <vector>

using std::vector;

// Direct3D Transformation Pipeline: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418867(v=vs.85).aspx

void GeometryGenerator::CalculateTangentsAndBitangents(vector<DefaultVertexBufferData>& vertices, const vector<unsigned> indices)	// Only Tangents
{
	//  Bitangent
	//	
	//	^  (uv1)
	//	|	 V1   ___________________ V2 (uv2)
	//	|		  \                 /
	//	|		   \               /
	//	|		    \             /
	//	|			 \           /
	//	|			  \         /
	//	|  dUV1 | E1   \       /  E2 | dUV2
	//	|			    \     /
	//	|			     \   /
	//	|			      \ /	
	//	|				   V
	//	|				   V0 (uv0)
	//	|				   
	// ----------------------------------------->  Tangent
	const size_t countVerts = vertices.size();
	const size_t countIndices = indices.size();
	assert(countIndices % 3 == 0);

	const vec3 N = vec3::Forward;	//  (0, 0, 1)

	for (size_t i = 0; i < countIndices; i += 3)
	{
		DefaultVertexBufferData& v0 = vertices[indices[i]];
		DefaultVertexBufferData& v1 = vertices[indices[i + 1]];
		DefaultVertexBufferData& v2 = vertices[indices[i + 2]];

		const vec3 E1 = v1.position - v0.position;
		const vec3 E2 = v2.position - v0.position;

		const vec2 dUV1 = vec2(v1.uv - v0.uv);
		const vec2 dUV2 = vec2(v2.uv - v0.uv);
		
		const float f = 1.0f / ( dUV1.x() * dUV2.y() - dUV1.y() * dUV2.x() );
		assert(!std::isinf(f));

		vec3 T( f * (dUV2.y() * E1.x() - dUV1.y() * E2.x()), 
				f * (dUV2.y() * E1.y() - dUV1.y() * E2.y()), 
				f * (dUV2.y() * E1.z() - dUV1.y() * E2.z()));
		T.normalize();
		
		vec3 B(	f * (-dUV2.x() * E1.x() + dUV1.x() * E2.x()), 
				f * (-dUV2.x() * E1.y() + dUV1.x() * E2.y()), 
				f * (-dUV2.x() * E1.z() + dUV1.x() * E2.z()));
		B.normalize();

		v0.tangent = T;
		v1.tangent = T;
		v2.tangent = T;

		// calculated in shader
		//v0.bitangent = B;
		//v1.bitangent = B;
		//v2.bitangent = B;

		if (v0.normal == vec3::Zero)
		{
			v0.normal = static_cast<const vec3>((XMVector3Cross(T, B))).normalized();
		}
		if (v1.normal == vec3::Zero)
		{
			v1.normal = static_cast<const vec3>((XMVector3Cross(T, B))).normalized();
		}
		if (v2.normal == vec3::Zero)
		{
			v2.normal = static_cast<const vec3>((XMVector3Cross(T, B))).normalized();
		}
	}
}

Mesh GeometryGenerator::Triangle(float scale)
{
	const float& size = scale;

	vector<DefaultVertexBufferData> vertices(3);
	vector<unsigned> indices = { 0, 1, 2 };

	// vertices - CW
	vertices[0].position	= vec3(-size, -size, 0.0f);
	vertices[0].normal		= vec3::Back;
	vertices[0].uv			= vec2(0.0f, 1.0f);

	vertices[1].position	= vec3(0, size, 0.0f);
	vertices[1].normal		= vec3::Back;
	vertices[1].uv			= vec2(0.5f, 0.0f);

	vertices[2].position	= vec3(size, -size, 0.0f);
	vertices[2].normal		= vec3::Back;
	vertices[2].uv			= vec2(1.0f, 1.0f);
	
	CalculateTangentsAndBitangents(vertices, indices);
	return Mesh(vertices, indices, "BuiltinTriangle");
}

Mesh GeometryGenerator::Quad(float scale)
{
	const float& size = scale;

	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		2, 3, 0
	//		|	  |		
	//	  0 +-----+ 3	
	const vector<unsigned> indices = 
	{ 
		0, 1, 2,
		2, 3, 0
	};

	vector<DefaultVertexBufferData> vertices(4);

	// vertices - CW
	vertices[0].position	= vec3(-size, -size, 0.0f);
	vertices[0].normal		= vec3(0.0f, 0.0f, -1.0f);
	vertices[0].uv			= vec2(0.0f, 1.0f);

	vertices[1].position	= vec3(-size, +size, 0.0f);
	vertices[1].normal		= vec3(0.0f, 0.0f, -1.0f);
	vertices[1].uv			= vec2(0.0f, 0.0f);

	vertices[2].position	= vec3(+size, +size, 0.0f);
	vertices[2].normal		= vec3(0.0f, 0.0f, -1.0f);
	vertices[2].uv			= vec2(1.0f, 0.0f);

	vertices[3].position	= vec3(+size, -size, 0.0f);
	vertices[3].normal		= vec3(0.0f, 0.0f, -1.0f);
	vertices[3].uv			= vec2(1.0f, 1.0f);
	
	CalculateTangentsAndBitangents(vertices, indices);
	return Mesh(vertices, indices, "BuiltinQuad");
}

Mesh GeometryGenerator::FullScreenQuad()
{
	const float& size = 1.0f;

	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		2, 3, 0
	//		|	  |		
	//	  0 +-----+ 3	
	const vector<unsigned> indices =
	{
		0, 1, 2,
		2, 3, 0
	};

	vector<FullScreenVertexBufferData> vertices(4);

	// vertices - CW
	vertices[0].position = vec3(-size, -size, 0.0f);
	vertices[0].uv = vec2(0.0f, 1.0f);

	vertices[1].position = vec3(-size, +size, 0.0f);
	vertices[1].uv = vec2(0.0f, 0.0f);

	vertices[2].position = vec3(+size, +size, 0.0f);
	vertices[2].uv = vec2(1.0f, 0.0f);

	vertices[3].position = vec3(+size, -size, 0.0f);
	vertices[3].uv = vec2(1.0f, 1.0f);

	return Mesh(vertices, indices, "BuiltinQuad");
}

Mesh GeometryGenerator::Cube()
{
	vector<DefaultVertexBufferData> vertices(24);
	const vector<unsigned> indices = 
	{
		0, 1, 2, 0, 2, 3,		// Top
		4, 5, 6, 4, 6, 7,		// back
		8, 9, 10, 8, 10, 11,	// Right
		12, 13, 14, 12, 14, 15, // Back
		16, 17, 18, 16, 18, 19, // Left
		20, 22, 21, 20, 23, 22, // Bottom
	};

//------------------------------------------------------------------
//		ASCII Cube art from: http://www.lonniebest.com/ASCII/Art/?ID=2
// 
//			   0 _________________________ 1		0, 1, 2, 0, 2, 3,		// Top
//		        / _____________________  /|			4, 5, 6, 4, 6, 7,		// Front
//		       / / ___________________/ / |			8, 9, 10, 8, 10, 11,	// Right
//		      / / /| |               / /  |			12, 13, 14, 12, 14, 15, // Left
//		     / / / | |              / / . |			16, 17, 18, 16, 18, 19, // Back
//		    / / /| | |             / / /| |			20, 22, 21, 20, 23, 22, // Bottom
//		   / / / | | |            / / / | |			
//		  / / /  | | |           / / /| | |		   +Y
//		 / /_/__________________/ / / | | |			|  +Z
//	4,3 /________________________/5/  | | |			|  /
//		| ______________________8|2|  | | |			| /
//		| | |    | | |_________| | |__| | |			|/______+X
//		| | |    | |___________| | |____| |			
//		| | |   / / ___________| | |_  / /		
//		| | |  / / /           | | |/ / /		
//		| | | / / /            | | | / /		
//		| | |/ / /             | | |/ /			
//		| | | / /              | | ' /			
//		| | |/_/_______________| |  /			
//		| |____________________| | /			
//		|________________________|/6			
//		7
	vertices[0].uv = vec2(+0.0f, +0.0f);
	vertices[1].uv = vec2(+1.0f, +0.0f);
	vertices[2].uv = vec2(+1.0f, +1.0f);

	vertices[3].uv = vec2(+0.0f, +1.0f);
	vertices[4].uv = vec2(+0.0f, +0.0f);
	vertices[5].uv = vec2(+1.0f, +0.0f);

	vertices[6].uv = vec2(+1.0f, +1.0f);
	vertices[7].uv = vec2(+0.0f, +1.0f);
	vertices[8].uv = vec2(+0.0f, +0.0f);

	vertices[9].uv = vec2(+1.0f, +0.0f);
	vertices[10].uv = vec2(+1.0f, +1.0f);
	vertices[11].uv = vec2(+0.0f, +1.0f);

	vertices[12].uv = vec2(+0.0f, +0.0f);
	vertices[13].uv = vec2(+1.0f, +0.0f);
	vertices[14].uv = vec2(+1.0f, +1.0f);
	vertices[15].uv = vec2(+0.0f, +1.0f);
	vertices[16].uv = vec2(+0.0f, +0.0f);
	vertices[17].uv = vec2(+1.0f, +0.0f);
	vertices[18].uv = vec2(+1.0f, +1.0f);
	vertices[19].uv = vec2(+0.0f, +1.0f);

	vertices[20].uv = vec2(+1.0f, +0.0f);
	vertices[21].uv = vec2(+0.0f, +0.0f);
	vertices[22].uv = vec2(+0.0f, +1.0f);
	vertices[23].uv = vec2(+1.0f, +1.0f);

	// vertices - CW 
	// TOP
	vertices[0].position	= vec3(-1.0f, +1.0f, +1.0f);
	vertices[0].normal		= vec3(+0.0f, +1.0f, +0.0f);
	vertices[0].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[1].position	= vec3(+1.0f, +1.0f, +1.0f);
	vertices[1].normal		= vec3(+0.0f, +1.0f, +0.0f);
	vertices[1].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[2].position	= vec3(+1.0f, +1.0f, -1.0f);
	vertices[2].normal		= vec3(+0.0f, +1.0f, +0.0f);
	vertices[2].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[3].position	= vec3(-1.0f, +1.0f, -1.0f);
	vertices[3].normal		= vec3(+0.0f, +1.0f, +0.0f);
	vertices[3].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	// FRONT
	vertices[4].position	= vec3(-1.0f, +1.0f, -1.0f);
	vertices[4].normal		= vec3(+0.0f, +0.0f, -1.0f);
	vertices[4].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[5].position	= vec3(+1.0f, +1.0f, -1.0f);
	vertices[5].normal		= vec3(+0.0f, +0.0f, -1.0f);
	vertices[5].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[6].position	= vec3(+1.0f, -1.0f, -1.0f);
	vertices[6].normal		= vec3(+0.0f, +0.0f, -1.0f);
	vertices[6].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[7].position	= vec3(-1.0f, -1.0f, -1.0f);
	vertices[7].normal		= vec3(+0.0f, +0.0f, -1.0f);
	vertices[7].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	// RIGHT
	vertices[8].position	= vec3(+1.0f, +1.0f, -1.0f);
	vertices[8].normal		= vec3(+1.0f, +0.0f, +0.0f);
	vertices[8].tangent	= vec3(+0.0f, +0.0f, +1.0f);

	vertices[9].position	= vec3(+1.0f, +1.0f, +1.0f);
	vertices[9].normal		= vec3(+1.0f, +0.0f, +0.0f);
	vertices[9].tangent	= vec3(+0.0f, +0.0f, +1.0f);

	vertices[10].position	= vec3(+1.0f, -1.0f, +1.0f);
	vertices[10].normal	= vec3(+1.0f, +0.0f, +0.0f);
	vertices[10].tangent	= vec3(+0.0f, +0.0f, +1.0f);

	vertices[11].position	= vec3(+1.0f, -1.0f, -1.0f);
	vertices[11].normal	= vec3(+1.0f, +0.0f, +0.0f);
	vertices[11].tangent	= vec3(+0.0f, +0.0f, +1.0f);
	
	// BACK
	vertices[12].position	= vec3(+1.0f, +1.0f, +1.0f);
	vertices[12].normal	= vec3(+0.0f, +0.0f, +1.0f);
	vertices[12].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[13].position	= vec3(-1.0f, +1.0f, +1.0f);
	vertices[13].normal	= vec3(+0.0f, +0.0f, +1.0f);
	vertices[13].tangent	= vec3(+1.0f, +0.0f, +0.0f);
	
	vertices[14].position	= vec3(-1.0f, -1.0f, +1.0f);
	vertices[14].normal	= vec3(+0.0f, +0.0f, +1.0f);
	vertices[14].tangent	= vec3(+1.0f, +0.0f, +0.0f);
	
	vertices[15].position	= vec3(+1.0f, -1.0f, +1.0f);
	vertices[15].normal	= vec3(+0.0f, +0.0f, +1.0f);
	vertices[15].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	// LEFT
	vertices[16].position	= vec3(-1.0f, +1.0f, +1.0f);
	vertices[16].normal	= vec3(-1.0f, +0.0f, +0.0f);
	vertices[16].tangent	= vec3(+0.0f, +0.0f, -1.0f);

	vertices[17].position	= vec3(-1.0f, +1.0f, -1.0f);
	vertices[17].normal	= vec3(-1.0f, +0.0f, +0.0f);
	vertices[17].tangent	= vec3(+0.0f, +0.0f, -1.0f);
	
	vertices[18].position	= vec3(-1.0f, -1.0f, -1.0f);
	vertices[18].normal	= vec3(-1.0f, +0.0f, +0.0f);
	vertices[18].tangent	= vec3(+0.0f, +0.0f, -1.0f);

	vertices[19].position	= vec3(-1.0f, -1.0f, +1.0f);
	vertices[19].normal	= vec3(-1.0f, +0.0f, +0.0f);
	vertices[19].tangent	= vec3(+0.0f, +0.0f, -1.0f);


	// BOTTOM
	vertices[20].position	= vec3(+1.0f, -1.0f, -1.0f);
	vertices[20].normal	= vec3(+0.0f, -1.0f, +0.0f);
	vertices[20].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[21].position	= vec3(-1.0f, -1.0f, -1.0f);
	vertices[21].normal	= vec3(+0.0f, -1.0f, +0.0f);
	vertices[21].tangent	= vec3(+1.0f, +0.0f, +0.0f);
	
	vertices[22].position	= vec3(-1.0f, -1.0f, +1.0f);
	vertices[22].normal	= vec3(+0.0f, -1.0f, +0.0f);
	vertices[22].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	vertices[23].position	= vec3(+1.0f, -1.0f, +1.0f);
	vertices[23].normal	= vec3(+0.0f, -1.0f, +0.0f);
	vertices[23].tangent	= vec3(+1.0f, +0.0f, +0.0f);

	//--------------------------------------------------------------

	// make edges unit length
	//for (int i = 0; i < 24; ++i)
	//{
	//	vertices[i].position.x() *= 0.5f;
	//	vertices[i].position.y() *= 0.5f;
	//	vertices[i].position.z() *= 0.5f;
	//}

	CalculateTangentsAndBitangents(vertices, indices);
	return Mesh(vertices, indices, "BuiltinCube");
}


Mesh GeometryGenerator::Sphere(float radius, unsigned ringCount, unsigned sliceCount, int numLODLevels /*= 1*/)
{
	// Vertex & Index buffer per LOD level
	MeshLODData< DefaultVertexBufferData> meshData(numLODLevels, "BuiltinSphere");

	// shorthands
	std::vector < vector<DefaultVertexBufferData>>& LODVertices = meshData.LODVertices;
	std::vector < std::vector<unsigned>>& LODIndices = meshData.LODIndices;

	// parameters for each LOD level
	std::vector<unsigned> LODRingCounts(numLODLevels);
	std::vector<unsigned> LODSliceCounts(numLODLevels);

	const unsigned MIN_RING_COUNT = 12;
	const unsigned MIN_SLICE_COUNT = 12;

	// using a simple lerp between min levels and given parameters 
	// so that:
	// - LOD level 0 represents the mesh defined with the function
	//   parameters @radius, @ringCount and @sliceCount
	// - the last LOD level is represented by MIN_RING_COUNT
	//   and MIN_SLICE_COUNT
	// 
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const float t = static_cast<float>(LOD) / (numLODLevels - 1);
		LODRingCounts[LOD]  = MathUtil::lerp(MIN_RING_COUNT , ringCount , 1.0f - t);
		LODSliceCounts[LOD] = MathUtil::lerp(MIN_SLICE_COUNT, sliceCount, 1.0f - t);
	}


	// SPHERE
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		vector<DefaultVertexBufferData>& Vertices = LODVertices[LOD];
		std::vector<unsigned>& Indices = LODIndices[LOD];

		// Compute vertices for each stack ring starting at the bottom and moving up.
		float dPhi = XM_PI / (LODRingCounts[LOD] - 1);
		for (float phi = -XM_PIDIV2; phi <= XM_PIDIV2 + 0.00001f; phi += dPhi)
		{
			float y = radius * sinf(phi);	// horizontal slice center height
			float r = radius * cosf(phi);	// horizontal slice radius

			// vertices of ring
			float dTheta = 2.0f*XM_PI / LODSliceCounts[LOD];
			for (unsigned j = 0; j <= LODSliceCounts[LOD]; ++j)	// for each pice(slice) in horizontal slice
			{
				DefaultVertexBufferData vertex;
				float theta = j * dTheta;
				float x = r * cosf(theta);
				float z = r * sinf(theta);
				vertex.position = vec3(x, y, z);
				{
					float u = (float)j / LODSliceCounts[LOD];
					float v = (y + radius) / (2 * radius);
					//fmod(2 * v, 1.0f);
					vertex.uv = vec2(u, v);
				}
				// Cylinder can be parameterized as follows, where we
				// introduce v parameter that goes in the same direction
				// as the v tex-coord so that the bitangent goes in the
				// same direction as the v tex-coord.
				// Let r0 be the bottom radius and let r1 be the
				// top radius.
				// y(v) = h - hv for v in [0,1].
				// r(v) = r1 + (r0-r1)v
				//
				// x(t, v) = r(v)*cos(t)
				// y(t, v) = h - hv
				// z(t, v) = r(v)*sin(t)
				//
				// dx/dt = -r(v)*sin(t)
				// dy/dt = 0
				// dz/dt = +r(v)*cos(t)
				//
				// dx/dv = (r0-r1)*cos(t)
				// dy/dv = -h
				// dz/dv = (r0-r1)*sin(t)

				// TangentU us unit length.
				vertex.tangent = vec3(-z, 0.0f, x).normalized();
				//float dr = bottomRadius - topRadius;
				//vec3 bitangent(dr*x, -, dr*z);
				//XMVECTOR T = XMLoadFloat3(&vertex.tangent);
				//XMVECTOR B = XMLoadFloat3(&bitangent);
				//XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				//XMStoreFloat3(&vertex.normal, N);
				XMVECTOR N = XMVectorSet(0, 1, 0, 1);
				XMVECTOR ROT = XMQuaternionRotationRollPitchYaw(0.0f, -XM_PI - theta, XM_PIDIV2 - phi);
				N = XMVector3Rotate(N, ROT);

				vertex.normal = N;
				Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		unsigned ringVertexCount = LODSliceCounts[LOD] + 1;
		// Compute indices for each stack.
		for (unsigned i = 0; i < LODRingCounts[LOD]; ++i)
		{
			for (unsigned j = 0; j < LODSliceCounts[LOD]; ++j)
			{
				Indices.push_back(i*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j + 1);
				Indices.push_back(i*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j + 1);
				Indices.push_back(i*ringVertexCount + j + 1);
			}
		}
	}
	//------------------------------------------------

	return Mesh(meshData);
}

Mesh GeometryGenerator::Grid(float width, float depth, unsigned horizontalTessellation, unsigned verticalTessellation, int numLODLevels /*= 1*/)
{
	MeshLODData< DefaultVertexBufferData> meshData(numLODLevels, "BuiltinGrid");

	// shorthands
	std::vector < vector<DefaultVertexBufferData>>& LODVertices = meshData.LODVertices;
	std::vector < std::vector<unsigned>>& LODIndices = meshData.LODIndices;

	// parameters for each LOD level
	std::vector<unsigned> LODNumHorizontalSlices(numLODLevels);
	std::vector<unsigned> LODNumVerticalSlices(numLODLevels);

	const unsigned MIN_HSLICE_COUNT = 8;
	const unsigned MIN_VSLICE_COUNT = 8;

	// using a simple lerp between min levels and given parameters 
	// so that:
	// - LOD level 0 represents the mesh defined with the function
	//   parameters @radius, @ringCount and @sliceCount
	// - the last LOD level is represented by MIN_RING_COUNT
	//   and MIN_SLICE_COUNT
	// 
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const float t = static_cast<float>(LOD) / (numLODLevels - 1);
		LODNumHorizontalSlices[LOD] = MathUtil::lerp(MIN_HSLICE_COUNT, horizontalTessellation, std::powf(1.0f - t, 2.0f));
		LODNumVerticalSlices[LOD]   = MathUtil::lerp(MIN_VSLICE_COUNT, verticalTessellation  , std::powf(1.0f - t, 2.0f));
	}

	//		Grid of m x n vertices
	//		-----------------------------------------------------------
	//		+	: Vertex
	//		d	: depth
	//		w	: width
	//		dx	: horizontal cell spacing = width / (m-1)
	//		dz	: z-axis	 cell spacing = depth / (n-1)
	// 
	//		  V(0,0)		  V(m-1,0)	^ Z
	//		^	+-------+-------+ ^		|
	//		|	|		|		| |		|
	//		|	|		|		| dz	|
	//		|	|		|		| |		|
	//		d	+-------+-------+ v		+--------> X
	//		|	|		|		|		
	//		|	|		|		|
	//		|	|		|		|
	//		v	+-------+-------+		
	//			<--dx--->		  V(m-1, n-1)
	//			<------ w ------>

	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const unsigned m = LODNumHorizontalSlices[LOD];
		const unsigned n = LODNumVerticalSlices[LOD];

		unsigned numQuads = (m - 1) * (n - 1);
		unsigned faceCount = numQuads * 2; // 2 faces per quad = triangle count
		unsigned vertCount = m * n;
		float dx = width / (n - 1);
		float dz = depth / (m - 1);	// m & n mixed up??

		// offsets for centering the grid : V(0,0) = (-halfWidth, halfDepth)
		float halfDepth = depth / 2;
		float halfWidth = width / 2;

		// texture coord increments
		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		vector<DefaultVertexBufferData>& Vertices = LODVertices[LOD];
		std::vector<unsigned>& Indices = LODIndices[LOD];

		Vertices.resize(vertCount);
		Indices.resize(faceCount * 3);

		// position the Vertices
		for (unsigned i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;
			for (unsigned j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;
				float u = j * du;
				float v = i * dv;
				Vertices[i*n + j].position = vec3(x, 0.0f, z);
				Vertices[i*n + j].normal = vec3(0.0f, 0.0f, 0.0f);
				Vertices[i*n + j].uv = vec2(u, v);
				Vertices[i*n + j].tangent = vec3(1.0f, 0.0f, 0.0f);
			}
		}

		//	generate Indices
		//
		//	  A	+------+ B
		//		|	 / |
		//		|	/  |
		//		|  /   |
		//		| /	   |
		//		|/	   |
		//	  C	+------+ D
		//
		//	A	: V(i  , j  )
		//	B	: V(i  , j+1)
		//	C	: V(i+1, j  )
		//	D	: V(i+1, j+1)
		//
		//	ABC	: (i*n +j    , i*n + j+1, (i+1)*n + j  )
		//	CBD : ((i+1)*n +j, i*n + j+1, (i+1)*n + j+1)

		unsigned k = 0;
		for (unsigned i = 0; i < m - 1; ++i)
		{
			for (unsigned j = 0; j < n - 1; ++j)
			{
				Indices[k] = i * n + j;
				Indices[k + 1] = i * n + j + 1;
				Indices[k + 2] = (i + 1)*n + j;
				Indices[k + 3] = (i + 1)*n + j;
				Indices[k + 4] = i * n + j + 1;
				Indices[k + 5] = (i + 1)*n + j + 1;
				k += 6;
			}
		}

		// apply height function
		for (size_t i = 0; i < Vertices.size(); ++i)
		{
			vec3& pos = Vertices[i].position;
			pos.y() = 0.2f * (pos.z() * sinf(20.0f * pos.x()) + pos.x() * cosf(10.0f * pos.z()));
		}

		CalculateTangentsAndBitangents(Vertices, Indices);
	}

	return Mesh(meshData);
}

Mesh GeometryGenerator::Cylinder(float height, float topRadius, float bottomRadius, unsigned numSlices, unsigned numStacks, int numLODLevels /*= 1*/)
{
	MeshLODData< DefaultVertexBufferData> meshData(numLODLevels, "BuiltinCylinder");

	// shorthands
	std::vector < vector<DefaultVertexBufferData>>& LODVertices = meshData.LODVertices;
	std::vector < std::vector<unsigned>>& LODIndices = meshData.LODIndices;

	// parameters for each LOD level
	std::vector<unsigned> LODStackCounts(numLODLevels);
	std::vector<unsigned> LODSliceCounts(numLODLevels);

	const unsigned MIN_STACK_COUNT = 4;
	const unsigned MIN_SLICE_COUNT = 8;

	// using a simple lerp between min levels and given parameters 
	// so that:
	// - LOD level 0 represents the mesh defined with the function
	//   parameters @radius, @ringCount and @sliceCount
	// - the last LOD level is represented by MIN_RING_COUNT
	//   and MIN_SLICE_COUNT
	// 
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const float t = static_cast<float>(LOD) / (numLODLevels - 1);
		LODStackCounts[LOD] = MathUtil::lerp(MIN_STACK_COUNT, numStacks, 1.0f - t);
		LODSliceCounts[LOD] = MathUtil::lerp(MIN_SLICE_COUNT, numSlices, 1.0f - t);
	}

	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const unsigned stackCount = LODStackCounts[LOD];
		const unsigned sliceCount = LODSliceCounts[LOD];

		// slice count	: horizontal resolution
		// stack count	: height resolution
		float stackHeight = height / stackCount;
		float radiusStep = (topRadius - bottomRadius) / stackCount;
		unsigned ringCount = stackCount + 1;

		// CYLINDER BODY
		//-----------------------------------------------------------
		vector<DefaultVertexBufferData>& Vertices = LODVertices[LOD];
		std::vector<unsigned>& Indices = LODIndices[LOD];

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (unsigned i = 0; i < ringCount; ++i)
		{
			float y = -0.5f*height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			// vertices of ring
			float dTheta = 2.0f*XM_PI / sliceCount;
			for (unsigned j = 0; j <= sliceCount; ++j)
			{
				DefaultVertexBufferData vertex;
				float c = cosf(j*dTheta);
				float s = sinf(j*dTheta);
				vertex.position = vec3(r*c, y, r*s);
				{
					float u = (float)j / sliceCount;
					float v = 1.0f - (float)i / stackCount;
					vertex.uv = vec2(u, v);
				}
				// Cylinder can be parameterized as follows, where we
				// introduce v parameter that goes in the same direction
				// as the v tex-coord so that the bitangent goes in the
				// same direction as the v tex-coord.
				// Let r0 be the bottom radius and let r1 be the
				// top radius.
				// y(v) = h - hv for v in [0,1].
				// r(v) = r1 + (r0-r1)v
				//
				// x(t, v) = r(v)*cos(t)
				// y(t, v) = h - hv
				// z(t, v) = r(v)*sin(t)
				//
				// dx/dt = -r(v)*sin(t)
				// dy/dt = 0
				// dz/dt = +r(v)*cos(t)
				//
				// dx/dv = (r0-r1)*cos(t)
				// dy/dv = -h
				// dz/dv = (r0-r1)*sin(t)

				// TangentU us unit length.
				vertex.tangent = vec3(-s, 0.0f, c);
				float dr = bottomRadius - topRadius;
				vec3 bitangent(dr*c, -height, dr*s);
				XMVECTOR T = vertex.tangent;
				XMVECTOR B = bitangent;
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				vertex.normal = N;
				Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring since the texture coordinates are different.
		unsigned ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (unsigned i = 0; i < stackCount; ++i)
		{
			for (unsigned j = 0; j < sliceCount; ++j)
			{
				Indices.push_back(i*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j + 1);
				Indices.push_back(i*ringVertexCount + j);
				Indices.push_back((i + 1)*ringVertexCount + j + 1);
				Indices.push_back(i*ringVertexCount + j + 1);
			}
		}

		// CYLINDER TOP
		//-----------------------------------------------------------
		{
			unsigned baseIndex = (unsigned)Vertices.size();
			float y = 0.5f*height;
			float dTheta = 2.0f*XM_PI / sliceCount;

			// Duplicate cap ring vertices because the texture coordinates and normals differ.
			for (unsigned i = 0; i <= sliceCount; ++i)
			{
				float x = topRadius * cosf(i*dTheta);
				float z = topRadius * sinf(i*dTheta);

				// Scale down by the height to try and make top cap texture coord area proportional to base.
				float u = x / height + 0.5f;
				float v = z / height + 0.5f;

				DefaultVertexBufferData Vert;
				Vert.position = vec3(x, y, z);
				Vert.normal = vec3(0.0f, 1.0f, 0.0f);
				Vert.tangent = vec3(1.0f, 0.0f, 0.0f);	// ?
				Vert.uv = vec2(u, v);
				Vertices.push_back(Vert);
			}

			// Cap center vertex.
			DefaultVertexBufferData capCenter;
			capCenter.position = vec3(0.0f, y, 0.0f);
			capCenter.normal = vec3(0.0f, 1.0f, 0.0f);
			capCenter.tangent = vec3(1.0f, 0.0f, 0.0f);
			capCenter.uv = vec2(0.5f, 0.5f);
			Vertices.push_back(capCenter);

			// Index of center vertex.
			unsigned centerIndex = (unsigned)Vertices.size() - 1;
			for (unsigned i = 0; i < sliceCount; ++i)
			{
				Indices.push_back(centerIndex);
				Indices.push_back(baseIndex + i + 1);
				Indices.push_back(baseIndex + i);
			}
		}


		// CYLINDER BOTTOM
		//-----------------------------------------------------------
		{
			unsigned baseIndex = (unsigned)Vertices.size();
			float y = -0.5f*height;
			float dTheta = 2.0f*XM_PI / sliceCount;

			// Duplicate cap ring vertices because the texture coordinates and normals differ.
			for (unsigned i = 0; i <= sliceCount; ++i)
			{
				float x = bottomRadius * cosf(i*dTheta);
				float z = bottomRadius * sinf(i*dTheta);

				// Scale down by the height to try and make top cap texture coord area proportional to base.
				float u = x / height + 0.5f;
				float v = z / height + 0.5f;

				DefaultVertexBufferData Vert;
				Vert.position = vec3(x, y, z);
				Vert.normal = vec3(0.0f, -1.0f, 0.0f);
				Vert.tangent = vec3(-1.0f, 0.0f, 0.0f);	// ?
				Vert.uv = vec2(u, v);
				Vertices.push_back(Vert);
			}
			// Cap center vertex.
			DefaultVertexBufferData capCenter;
			capCenter.position = vec3(0.0f, y, 0.0f);
			capCenter.normal = vec3(0.0f, -1.0f, 0.0f);
			capCenter.tangent = vec3(-1.0f, 0.0f, 0.0f);
			capCenter.uv = vec2(0.5f, 0.5f);
			Vertices.push_back(capCenter);

			// Index of center vertex.
			unsigned centerIndex = (unsigned)Vertices.size() - 1;
			for (unsigned i = 0; i < sliceCount; ++i)
			{
				Indices.push_back(centerIndex);
				Indices.push_back(baseIndex + i);
				Indices.push_back(baseIndex + i + 1);
			}
		}

	}
	//------------------------------------------------

	return Mesh(meshData);
}

Mesh GeometryGenerator::Cone(float height, float radius, unsigned numSlices, int numLODLevels /*= 1*/)
{
	MeshLODData< DefaultVertexBufferData> meshData(numLODLevels, "BuiltinCone");

	// shorthands
	std::vector < vector<DefaultVertexBufferData>>& LODVertices = meshData.LODVertices;
	std::vector < std::vector<unsigned>>& LODIndices = meshData.LODIndices;

	// parameters for each LOD level
	std::vector<unsigned> LODSliceCounts(numLODLevels);

	const unsigned MIN_SLICE_COUNT = 10;

	// using a simple lerp between min levels and given parameters 
	// so that:
	// - LOD level 0 represents the mesh defined with the function
	//   parameters @radius, @ringCount and @sliceCount
	// - the last LOD level is represented by MIN_RING_COUNT
	//   and MIN_SLICE_COUNT
	// 
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		const float t = static_cast<float>(LOD) / (numLODLevels - 1);
		LODSliceCounts[LOD] = MathUtil::lerp(MIN_SLICE_COUNT, numSlices, 1.0f - t);
	}

	const bool bAddBackFaceForBase = true;
	for (int LOD = 0; LOD < numLODLevels; ++LOD)
	{
		vector<DefaultVertexBufferData>& Vertices = LODVertices[LOD];
		std::vector<unsigned>& Indices = LODIndices[LOD];

		const unsigned sliceCount = LODSliceCounts[LOD];

		int IndexOfConeBaseCenterVertex = -1;
		// BASE
		//-----------------------------------------------------------
		unsigned baseIndex = (unsigned)Vertices.size();
		float y = 0.0f; // -0.33f*height;
		float dTheta = 2.0f*XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (unsigned i = 0; i <= sliceCount; ++i)
		{
			float x = radius * cosf(i*dTheta);
			float z = radius * sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			DefaultVertexBufferData Vert;
			Vert.position = vec3(x, y, z);

			Vert.normal = vec3(0.0f, 1.0f, 0.0f);
			Vert.tangent = vec3(-1.0f, 0.0f, 0.0f);	// ?
			Vert.uv = vec2(u, v);
			Vertices.push_back(Vert);
		} // ConeRingVertices

		{
			// Cap center vertex.
			DefaultVertexBufferData capCenter;
			capCenter.position = vec3(0.0f, y, 0.0f);
			
			capCenter.normal = vec3(0.0f, 1.0f, 0.0f);
			capCenter.tangent = vec3(-1.0f, 0.0f, 0.0f);
			
			capCenter.uv = vec2(0.5f, 0.5f);
			Vertices.push_back(capCenter);

			// Index of center vertex.
			unsigned centerIndex = (unsigned)Vertices.size() - 1;
			IndexOfConeBaseCenterVertex = static_cast<int>(centerIndex);
			for (unsigned i = 0; i < sliceCount; ++i)
			{
				Indices.push_back(centerIndex);
				Indices.push_back(baseIndex + i + 1);
				Indices.push_back(baseIndex + i);
			}
		} // ConeBaseVertex


		if (bAddBackFaceForBase)
		{
			baseIndex = (unsigned)Vertices.size();
			const float offsetInNormalDirection = 0.0f;//-1.100001f;
			for (unsigned i = 0; i <= sliceCount; ++i)
			{
				const float x = radius * cosf(i*dTheta);
				const float z = radius * sinf(i*dTheta);
				const float u = x / height + 0.5f;
				const float v = z / height + 0.5f;

				DefaultVertexBufferData Vert;
				Vert.position = vec3(x, y + offsetInNormalDirection, z);
				Vert.normal = vec3(0.0f, -1.0f, 0.0f);
				Vert.tangent = vec3(-1.0f, 0.0f, 0.0f);	// ?
				Vert.uv = vec2(u, v);
				Vertices.push_back(Vert);
			}

			DefaultVertexBufferData capCenter;
			capCenter.position = vec3(0.0f, y + offsetInNormalDirection, 0.0f);
			capCenter.normal = vec3(0.0f, -1.0f, 0.0f);
			capCenter.tangent = vec3(-1.0f, 0.0f, 0.0f);
			capCenter.uv = vec2(0.5f, 0.5f);
			Vertices.push_back(capCenter);

			unsigned centerIndex = (unsigned)Vertices.size() - 1;
			for (unsigned i = 0; i < sliceCount; ++i)
			{
				Indices.push_back(centerIndex);
				Indices.push_back(baseIndex + i);
				Indices.push_back(baseIndex + i + 1);
			}
		}



		// CONE
		//-----------------------------------------------------------
		{
			// add the tip vertex
			DefaultVertexBufferData tipVertex;
			tipVertex.position = vec3(0.0f, height, 0.0f);
			tipVertex.normal = vec3(0.0f, 1.0f, 0.0f);
			tipVertex.tangent = vec3(1.0f, 0.0f, 0.0f); // 0 0 0 ?
			tipVertex.uv = vec2(0.5f, 0.5f);            // ?
			Vertices.push_back(tipVertex);

			const unsigned tipVertIndex = (unsigned)Vertices.size() - 1;
			for (unsigned i = 0; i <= sliceCount; ++i)
			{
				Indices.push_back(tipVertIndex);
				Indices.push_back(i + 1);
				Indices.push_back(i);

				// -----------------------------------
				// calculate the tangent and normal vectors
				// and override the placeholder values used earlier
				//
				// Pt : position of tip of cone
				// P0 : position of surface triangle bottom vertex0
				// P1 : position of surface triangle bottom vertex1
				//
				const vec3 Pt = tipVertex.position;
				const vec3 P0 = Vertices[i].position;
				const vec3 P1 = Vertices[i + 1].position;

				//  T  : tangent vector: vector along the cone surface.
				vec3 T = Pt - (P0+P1)*0.5f;
				T.normalize();

				// use the vectors pointing from P0->Pt: V0 and P0->P1: V1
				// as the basis vectors for the triangle surface
				vec3 V0 = Pt - P0;
				vec3 V1 = P1 - P0;
				V0.normalize();
				V1.normalize();

				// cross product of the normalized vectors will give
				// the surface normal vector
				vec3 N = XMVector3Dot(V0, V1);

				Vertices[i + 1].normal = N;
				Vertices[i + 0].normal = N;
				Vertices[i + 0].tangent = T;
				Vertices[i + 0].tangent = T;
			}
		}
	}

	return Mesh(meshData);
}

bool GeometryGenerator::Is2DGeometry(EGeometry meshID)
{
	return meshID == EGeometry::TRIANGLE || meshID == EGeometry::QUAD || meshID == EGeometry::GRID;
}
