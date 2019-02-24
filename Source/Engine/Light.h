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

#pragma once

#include "Transform.h"
#include "DataStructures.h"
#include "Settings.h"

#include "Renderer/RenderingEnums.h"
#include "Renderer/Texture.h"

#include "Utilities/Color.h"

#include <DirectXMath.h>


// Only used for point lights when querying LightSpaceMatrix, ViewMatrix and ViewFrustumPlanes.
//
#define DEFAULT_POINT_LIGHT_LOOK_DIRECTION Texture::CubemapUtility::ECubeMapLookDirections::CUBEMAP_LOOK_FRONT


// Design consideration here:
//
// INHERITANCE
// - if we were to use inheritance for different types of lights, then we can utilize pure virtual functions
//   to enforce class-specific behavior. However, now, we cannot store a vector<Light> due to pure virtuality.
//   most likely solution is the store pointers to derived types, which now requires a memory manager for lights
//   if we want to iterate over lights in a linear-memory-access fashion.
//
// C-STYLE 
// - instead, we can collect the light-specific data under a union and enforce light-specific behavior
//   through the ELightType enum. Currently favoring this approach over inheritance to avoid maintaining the memory
//   of the derived types and simply making use of a vector to hold all light data.
//

struct Light
{
	enum ELightType : int
	{
		POINT = 0,
		SPOT,
		DIRECTIONAL,

		CYLINDER,
		LINE,
		RECTANGLE,
		DISK,


		LIGHT_TYPE_COUNT
	};

	//--------------------------------------------------------------------------------------------------------------
	// INTERFACE
	//--------------------------------------------------------------------------------------------------------------
	Light() // DEFAULTS
		: mColor(LinearColor::white)
		, mbEnabled(true)
		, mType(LIGHT_TYPE_COUNT)
		, mBrightness(300.0f)
		, mbCastingShadows(false)
		, mDepthBias(0.0f)
		, mNearPlaneDistance(0.0f)
		, mFarPlaneDistance(0.0f)
		, mRange(100.0f)
		, mTransform()
		, mMeshID(EGeometry::SPHERE)
	{}
	Light
	(
		LinearColor color
		, ELightType type
		, float brightness
		, bool bCastShadows
		, float depthBias
		, float nearPlaneDistance
		, float range
		, Transform transform
		, EGeometry mesh
	)
		: mColor(color)
		, mType(type)
		, mBrightness(brightness)
		, mbCastingShadows(bCastShadows)
		, mDepthBias(depthBias)
		, mNearPlaneDistance(nearPlaneDistance)
		, mRange(range)
		, mTransform(transform)
		, mMeshID(mesh)
	{}


	// returns the projection matrix for the light space transformation. 
	//
	XMMATRIX GetProjectionMatrix() const;

	// returns the view matrix for Directional/Spot lights. 
	// Use 'Texture::CubemapUtility::ECubeMapLookDirections' to get view matrices for cubemap faces for PointLight.
	//
	XMMATRIX GetViewMatrix(Texture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const;

	// returns the frustum plane data for the light.
	// Use 'Texture::CubemapUtility::ECubeMapLookDirections' to get frustum planes for each direction for PointLight.
	//
	inline FrustumPlaneset GetViewFrustumPlanes(Texture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const
	{ 
		return FrustumPlaneset::ExtractFromMatrix(GetViewMatrix(lookDir) * GetProjectionMatrix());
	}

	// Returns the View*Projection matrix that describes the light-space transformation of a world space position.
	// Use 'Texture::CubemapUtility::ECubeMapLookDirections' to get the light space matrix for each direction for PointLight.
	//
	inline XMMATRIX GetLightSpaceMatrix(Texture::CubemapUtility::ECubeMapLookDirections lookDir = DEFAULT_POINT_LIGHT_LOOK_DIRECTION) const 
	{ 
		return GetViewMatrix(lookDir) * GetProjectionMatrix(); 
	}


	void GetGPUData(PointLightGPU& refData) const;
	void GetGPUData(SpotLightGPU& refData) const;
	void GetGPUData(DirectionalLightGPU& refData) const;
	void GetGPUData(CylinderLightGPU& refData) const;
	void GetGPUData(RectangleLightGPU& refData) const;

	// TODO: remove this arbitrary function for directional lights
	Settings::ShadowMap GetSettings() const; // ?



	//--------------------------------------------------------------------------------------------------------------
	// DATA
	//--------------------------------------------------------------------------------------------------------------
	ELightType mType;
	LinearColor	mColor;
	float mBrightness;

	bool mbCastingShadows;
	float mDepthBias;
	float mNearPlaneDistance;
	union 
	{
		float mFarPlaneDistance;
		float mRange;
	};

	Transform mTransform;
	EGeometry mMeshID;
	bool mbEnabled;


	union // LIGHT-SPECIFIC DATA
	{
		// DIRECTIONAL LIGHT ----------------------------------------
		//  
		//   |  |  |  |  |
		//   |  |  |  |  |
		//   v  v  v  v  v
		//    
		struct  
		{
			float mViewportX;
			float mViewportY;
			float mDistanceFromOrigin;
		};
		


		// POINT LIGHT -----------------------------------------------
		//
		//   \|/ 
		//  --*--
		//   /|\
		//
		struct // Point
		{
			float mAttenuationConstant ; // Currently Unused: attenuation = 1/distance^2 in the shaders
			float mAttenuationLinear   ; // Currently Unused: attenuation = 1/distance^2 in the shaders
			float mAttenuationQuadratic; // Currently Unused: attenuation = 1/distance^2 in the shaders
		};



		// SPOT LIGHT --------------------------------------------------
		//     
		//       *
		//     /   \
		//    /_____\
		//   ' ' ' ' ' 
		//
		struct // Spot
		{
			float mSpotOuterConeAngleDegrees;
			float mSpotInnerConeAngleDegrees;
		};



		// Eric Heitz LTC Slides: https://drive.google.com/file/d/0BzvWIdpUpRx_Z2pZWWFtam5xTFE/view
		// CYLINDER LIGHT  ------------------------------------------------
		//
		//
		//
		//
		//
		//
		struct // Cylinder
		{
			float mRadius;
			float mHeight;
		};

		// LINEAR/LINE LIGHT  ------------------------------------------------
		//
		//
		//
		//
		//
		//
		struct // Linear
		{
			float mLength;
		};

		// REACTANGLE LIGHT  ------------------------------------------------
		//
		//
		//
		//
		//
		//
		struct // Rectangle
		{
			float mWidth;
			float mHeight;
		};

		// DISK LIGHT  ------------------------------------------------
		//
		//
		//
		//
		//
		//
		struct // Disk
		{
			float mRadius;
		};


	};
};

