/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
*/

#ifndef __HUSD_Skeleton_h__
#define __HUSD_Skeleton_h__

#include "HUSD_API.h"

#include <GU/GU_AgentRig.h>
#include <SYS/SYS_Types.h>

class GU_AgentClip;
class GU_AgentLayer;
class GU_AgentShapeLib;
class GU_Detail;
class HUSD_AutoReadLock;
class UT_StringHolder;
class UT_StringRef;

/// Imports all skinnable primitives underneath the provided SkelRoot prim.
HUSD_API bool
HUSDimportSkinnedGeometry(GU_Detail &gdp, const HUSD_AutoReadLock &readlock,
                          const UT_StringRef &skelrootpath,
                          const UT_StringHolder &shapeattrib);

enum class HUSD_SkeletonPoseType
{
    Animation,
    BindPose,
    RestPose
};

/// Imports all Skeleton primitives underneath the provided SkelRoot prim.
/// A point is created for each joint, and joints are connected to their
/// parents by polyline primitives.
/// Use HUSDimportSkeletonPose() to set the skeleton's transforms. The pose
/// type is only used in this method to initialize attributes that aren't
/// time-varying.
HUSD_API bool
HUSDimportSkeleton(GU_Detail &gdp, const HUSD_AutoReadLock &readlock,
                   const UT_StringRef &skelrootpath,
                   HUSD_SkeletonPoseType pose_type);

/// Updates the pose for the skeleton geometry created by HUSDimportSkeleton().
HUSD_API bool
HUSDimportSkeletonPose(GU_Detail &gdp, const HUSD_AutoReadLock &readlock,
                       const UT_StringRef &skelrootpath,
                       HUSD_SkeletonPoseType pose_type, fpreal time);

/// Builds an agent rig from the SkelRoot's first Skeleton prim.
HUSD_API GU_AgentRigPtr
HUSDimportAgentRig(const HUSD_AutoReadLock &readlock,
                   const UT_StringRef &skelrootpath,
                   const UT_StringHolder &rig_name);

/// Imports all skinnable primitives underneath the provided SkelRoot prim
/// (which are associated with the skeleton used for HUSDimportRig()), and adds
/// the shape bindings to the provided layer.
HUSD_API bool
HUSDimportAgentShapes(GU_AgentShapeLib &shapelib,
                      GU_AgentLayer &layer,
                      const HUSD_AutoReadLock &readlock,
                      const UT_StringRef &skelrootpath,
                      fpreal layer_bounds_scale);

/// Initialize an agent clip from the animation associated with the skeleton
/// used for HUSDimportAgentRig().
HUSD_API bool
HUSDimportAgentClip(GU_AgentClip &clip,
                    HUSD_AutoReadLock &readlock,
                    const UT_StringRef &skelrootpath);

#endif