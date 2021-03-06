//
// Copyright 2019 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef GUSD_AGENTUTILS_H
#define GUSD_AGENTUTILS_H

/// \file agentUtils.h
/// \ingroup group_gusd_Agents
/// Utilities for translating agents to/from USD.
///
/// These do not provide complete, automatic conversion to/from USD at this
/// stage. Rather, these utilities may be used to build out a conversion
/// pipeline, such as generating all of the various JSON files needed to
/// build out the components of GU_Agent primitives.
///

#include "api.h"
#include "purpose.h"

#include "pxr/base/vt/array.h"
#include "pxr/usd/usd/timeCode.h"

#include <GU/GU_AgentDefinition.h>
#include <GU/GU_AgentLayer.h>
#include <GU/GU_AgentRig.h>
#include <GU/GU_AgentShapeLib.h>
#include <UT/UT_StringArray.h>

class GT_RefineParms;

PXR_NAMESPACE_OPEN_SCOPE

class UsdSkelBinding;
class UsdSkelSkeleton;
class UsdSkelSkeletonQuery;
class UsdSkelSkinningQuery;
class UsdSkelTopology;


/// Create an agent rig from a \p skelQuery.
GUSD_API GU_AgentRigPtr
GusdCreateAgentRig(const UT_StringHolder &name,
                   const UsdSkelSkeletonQuery& skelQuery,
                   bool createLocomotionJoint = true);


/// Create an agent rig from \p topology and \p jointNames.
/// Each joint name must be unique.
GUSD_API GU_AgentRigPtr
GusdCreateAgentRig(const UT_StringHolder &name,
                   const UsdSkelTopology& topology,
                   const VtTokenArray& jointNames,
                   bool createLocomotionJoint = true);


/// Create a shape library where every skinning target of \p binding is
/// a separate shape.
/// The \p sev defines the error severity when reading in each shape.
/// If the severity is less than UT_ERROR_ABORT, the invalid shape is
/// skipped. Otherwise, creation of the shape lib fails if errors are
/// produced processing any shapes.
GUSD_API GU_AgentShapeLibPtr
GusdCreateAgentShapeLib(const UsdSkelBinding& binding,
                        UsdTimeCode time=UsdTimeCode::EarliestTime(),
                        const char* lod=nullptr,
                        GusdPurposeSet purpose=GusdPurposeSet(
                            GUSD_PURPOSE_DEFAULT|GUSD_PURPOSE_PROXY),
                        UT_ErrorSeverity sev=UT_ERROR_WARNING,
                        const GT_RefineParms* refineParms=nullptr);


/// Read in all skinnable shapes for \p binding, coalescing them into \p gd.
/// The \p sev defines the error severity when reading in each shape.
/// If the severity is less than UT_ERROR_ABORT, the invalid shape is
/// skipped. Otherwise, creation of the coalesced detail fails if errors are
/// produced processing any shapes.
GUSD_API bool
GusdCoalesceAgentShapes(GU_Detail& gd,
                        const UsdSkelBinding& binding,
                        UsdTimeCode time=UsdTimeCode::EarliestTime(),
                        const char* lod=nullptr,
                        GusdPurposeSet purpose=GusdPurposeSet(
                            GUSD_PURPOSE_DEFAULT|GUSD_PURPOSE_PROXY),
                        UT_ErrorSeverity sev=UT_ERROR_WARNING,
                        const GT_RefineParms* refineParms=nullptr);


/// Read in a skinnable prim given by \p skinningQuery into \p gd.
/// The \p jointNames array provides the names of the joints of the bound
/// Skeleton, using the ordering specified on the Skeleton.
/// The \p invBindTransforms array holds the inverse of the Skeleton's
/// bind transforms.
/// Errors encountered while reading the skinnable primitive are reported
/// with a severity of \p sev.
GUSD_API bool
GusdReadSkinnablePrim(GU_Detail& gd,
                      const UsdSkelSkinningQuery& skinningQuery,
                      const VtTokenArray& jointNames,
                      const VtMatrix4dArray& invBindTransforms,
                      UsdTimeCode time=UsdTimeCode::EarliestTime(),
                      const char* lod=nullptr,
                      GusdPurposeSet purpose=GusdPurposeSet(
                          GUSD_PURPOSE_DEFAULT|GUSD_PURPOSE_PROXY),
                      UT_ErrorSeverity sev=UT_ERROR_ABORT,
                      const GT_RefineParms* refineParms=nullptr);


/// Read shapes for each shape in \p binding.
/// The \p sev defines the error severity when reading in each shape.
/// If the severity is less than UT_ERROR_ABORT, invalid shapes are
/// skipped, and an empty detail handle is stored in \p details for
/// the corresponding shape. Otherwise, the process returns false if
/// errors are encountered processing any shapes.
GUSD_API bool
GusdReadSkinnablePrims(const UsdSkelBinding& binding,
                       UT_Array<GU_DetailHandle>& details,
                       UsdTimeCode time=UsdTimeCode::EarliestTime(),
                       const char* lod=nullptr,
                       GusdPurposeSet purpose=GusdPurposeSet(
                            GUSD_PURPOSE_DEFAULT|GUSD_PURPOSE_PROXY),
                       UT_ErrorSeverity sev=UT_ERROR_WARNING,
                       const GT_RefineParms* refineParms=nullptr);

/// Create the boneCapture attribute on the geometry.
/// Requires the skel:jointIndices and skel:jointWeights primvars to have been
/// imported as attributes, unless the geometry is rigidly deformed.
GUSD_API bool
GusdCreateCaptureAttribute(GU_Detail &detail,
                           const UsdSkelSkinningQuery &skinningQuery,
                           const VtTokenArray &jointNames,
                           const VtMatrix4dArray &invBindTransforms);

struct GUSD_API GusdSkinImportParms
{
    UsdTimeCode myTime = UsdTimeCode::EarliestTime();
    const char *myLOD = nullptr;
    GusdPurposeSet myPurpose =
        GusdPurposeSet(GUSD_PURPOSE_DEFAULT | GUSD_PURPOSE_PROXY);
    const GT_RefineParms *myRefineParms = nullptr;
};

using GusdSkinnedPrimCallback =
    std::function<bool(exint i,
                       const GusdSkinImportParms &parms,
                       const VtTokenArray &jointNames,
                       const VtMatrix4dArray &invBindTransforms)>;

/// Invokes the callback for each skinnable prim, possibly in parallel.
/// This can be used for customized importing of shapes.
GUSD_API bool
GusdForEachSkinnedPrim(const UsdSkelBinding &binding,
                       const GusdSkinImportParms &parms,
                       const GusdSkinnedPrimCallback &callback);

/// Returns the skeleton's list of joint names, preferring the 'jointNames'
/// attribute over the 'joints' attribute.
GUSD_API bool
GusdGetJointNames(const UsdSkelSkeleton &skel, VtTokenArray &jointNames);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // GUSD_AGENTUTILS_H
