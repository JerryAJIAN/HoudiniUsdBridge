/*
 * Copyright 2019 Side Effects Software Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Produced by:
 *	Side Effects Software Inc.
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *      Canada   M5J 2M2
 *	416-504-9876
 *
 */

#ifndef __HUSD_FindCollections_h__
#define __HUSD_FindCollections_h__

#include "HUSD_API.h"
#include "HUSD_DataHandle.h"
#include "HUSD_FindPrims.h"
#include "HUSD_Utils.h"
#include <UT/UT_StringHolder.h>
#include <UT/UT_UniquePtr.h>
#include <pxr/pxr.h>

PXR_NAMESPACE_OPEN_SCOPE

class XUSD_PathSet;
class XUSD_PathPattern;

PXR_NAMESPACE_CLOSE_SCOPE

class HUSD_API HUSD_FindCollections
{
public:
				 HUSD_FindCollections(HUSD_AutoAnyLock &lock,
					 HUSD_PrimTraversalDemands demands =
					    HUSD_TRAVERSAL_DEFAULT_DEMANDS);
				 // Simple constructor when you just want to
				 // operate on a single collection.
				 HUSD_FindCollections(HUSD_AutoAnyLock &lock,
					 const UT_StringRef &primpath,
					 const UT_StringRef &collectionname,
					 HUSD_PrimTraversalDemands demands =
					    HUSD_TRAVERSAL_DEFAULT_DEMANDS);
				~HUSD_FindCollections();

    const HUSD_FindPrims	&findPrims() const
				 { return myFindPrims; }
    HUSD_FindPrims		&findPrims()
				 { return myFindPrims; }

    const UT_StringHolder	&collectionPattern() const
				 { return myCollectionPattern; }
    void			 setCollectionPattern(
					const UT_StringHolder &pattern);

    const PXR_NS::XUSD_PathSet	&getExpandedPathSet() const;
    void			 getExpandedPaths(UT_StringArray &paths) const;

private:
    class husd_FindCollectionsPrivate;

    UT_UniquePtr<husd_FindCollectionsPrivate>	 myPrivate;
    HUSD_AutoAnyLock				&myAnyLock;
    HUSD_FindPrims				 myFindPrims;
    UT_StringHolder				 myCollectionPattern;
};

#endif

