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

#ifndef __HUSD_ShaderTranslator_h__
#define __HUSD_ShaderTranslator_h__

#include "HUSD_API.h"

#include <VOP/VOP_Types.h>
#include <UT/UT_StringArray.h>
#include <UT/UT_Pair.h>

class HUSD_AutoWriteLock;
class HUSD_TimeCode;
class OP_Node;


/// Creates a USD shader primitives from Houdini's nodes.
class HUSD_API HUSD_ShaderTranslator
{
public:
    /// Standard virtual destructor for this abstract base class.
    virtual	~HUSD_ShaderTranslator() = default;


    /// Returns true if the translator can encode a shader that reports
    /// a given render mask (ie, is a shader for a given render target).
    virtual bool matchesRenderMask( const UT_StringRef &render_mask ) = 0;


    /// Defines a USD shader primitive that is part of the USD material.
    /// Ie, the translator will connect the shader to the material output.
    ///
    /// @p usd_material_path - path to the material primitive in which 
    ///		the shader primitive should be created.
    /// @p time_code - time code at which to evaluate any properties
    /// @p shader_node - the Houdini node that represents a shader and that 
    ///		needs to be translated into a USD shader primitive
    /// @p shader_type - some VOPs contains several shaders (eg material
    ///		builders). So this parameters specifies the type of the shader 
    ///		to pick and translate.
    /// @p output_name - the output name of the VOP node that represents
    ///		the shader to pick and translate. It can be an empty string,
    ///		if the VOP node does not have shader outputs.
    virtual void createMaterialShader( HUSD_AutoWriteLock &lock,
			const UT_StringRef &usd_material_path,
			const HUSD_TimeCode &time_code,
			OP_Node &shader_node, 
			VOP_Type shader_type,
			const UT_StringRef &output_name) = 0;

    /// Defines a USD shader primitive that is part of a shader network chain.
    /// Ie, the translator will create a shader primitive output, that the 
    /// caller can use to connect as an input to another shader.
    /// 
    /// @p usd_material_path - path to the material primitive in which 
    ///		the shader primitive should be created.
    /// @p usd_parent_path - path to the primitive inside which 
    ///		the shader primitive should be created directly.
    /// @p time_code - time code at which to evaluate any properties
    /// @p shader_node - the Houdini node that represents a shader and that 
    ///		needs to be translated into a USD shader primitive
    /// @p output_name - the output name of the VOP node that needs to be
    ///		translated into USD shader output. This is the output
    ///		the caller is interested in having representation in USD.
    ///
    /// @return The path to the USD shader output attribute corresponding
    ///		to the @p output_name connector on the @p shader_node.
    virtual UT_StringHolder createShader( HUSD_AutoWriteLock &lock,
			const UT_StringRef &usd_material_path,
			const UT_StringRef &usd_parent_path,
			const HUSD_TimeCode &time_code,
			OP_Node &shader_node, 
			const UT_StringRef &output_name) = 0;


    /// Re-translates the shader parameters given the shader VOP node (and its 
    /// new parameter values).
    /// @p usd_shader_path - the path to the USD shader primitive whose
    ///		input attributes need updating due to node parm value change.
    /// @p time_code - time code at which to evaluate any properties
    /// @p shader_node - Houdini node that represents a shader that 
    ///		needs to be re-translated into the given USD shader primitive.
    /// @p parameter_names - the list  of parameters that have changed.
    ///         If the list is empty, then any of the node's parameters may
    ///         have changed. If it's not empty, then only listed parameters
    ///         have changed.
    virtual void updateShaderParameters( HUSD_AutoWriteLock &lock,
			const UT_StringRef &usd_shader_path,
			const HUSD_TimeCode &time_code,
			OP_Node &shader_node,
                        const UT_StringArray &parameter_names) = 0;

    /// Returns the name of the renderer (render context name) that
    /// should be used in the material output name for that USD shader.
    virtual UT_StringHolder getRenderContextName( OP_Node &shader_node, 
			const UT_StringRef &output_name) = 0;


    /// Some translators may want to know their ID in the registry.
    virtual void	setID( int id )	{ myID = id; }
    virtual int		getID() const	{ return myID; }

private:
    /// Translator's ID.
    int			myID = -1;
};

// ============================================================================ 
/// Creates a standard USD Preview Surface shader from Houdini's node.
class HUSD_API HUSD_PreviewShaderGenerator
{
public:
    /// Standard virtual destructor for this abstract base class.
    virtual	~HUSD_PreviewShaderGenerator() = default;


    /// Returns true if the generator can create a USD Preview Surface shader
    /// for a shader node that reports the given render mask.
    virtual bool matchesRenderMask( const UT_StringRef &render_mask ) = 0;


    /// Creates a USD Preview Surface shader primitive for the USD material.
    ///
    /// @p usd_material_path - path to the material primitive in which 
    ///		the shader primitive should be created.
    /// @p time_code - time code at which to evaluate any properties
    /// @p shader_node - the Houdini node that represents a shader for which
    ///		the USD Preview Shader prim should be created.
    /// @p output_name - the output name of the VOP node that represents
    ///		the shader to pick and translate. It can be an empty string,
    ///		if the VOP node does not have shader outputs.
    virtual void createMaterialPreviewShader( HUSD_AutoWriteLock &lock,
			const UT_StringRef &usd_material_path,
			const HUSD_TimeCode &time_code,
			OP_Node &shader_node, 
			const UT_StringRef &output_name) = 0;

    /// Re-generates the shader parameters given the shader VOP node (and its 
    /// new parameter values).
    /// @p usd_shader_path - the path to the USD preview shader primitive whose
    ///		input attributes need updating due to node parm value change.
    /// @p time_code - time code at which to evaluate any properties
    /// @p shader_node - Houdini node whose parameters changed, thus requiring
    ///		an update to the input attributes of the corresponding
    ///		USD preview shader.
    virtual void updateMaterialPreviewShaderParameters(HUSD_AutoWriteLock &lock,
			const UT_StringRef &usd_shader_path,
			const HUSD_TimeCode &time_code,
			OP_Node &shader_node,
                        const UT_StringArray &parameter_names) = 0;
};

// ============================================================================ 
/// Keeps a list of known translators that define a USD shader prim from 
/// Houdini shader nodes.
class HUSD_API HUSD_ShaderTranslatorRegistry
{
public:
    /// Returns a singelton instance.
    static HUSD_ShaderTranslatorRegistry &get();


    /// Adds the translator to the list of known translators.
    void	registerShaderTranslator( HUSD_ShaderTranslator &translator );

    /// Removes the translator from the list of known translators.
    void	unregisterShaderTranslator( HUSD_ShaderTranslator &translator );

    /// Returns a translator that accepts the given render target mask.
    /// If no translator is found, returns nullptr.
    HUSD_ShaderTranslator * findShaderTranslator( const OP_Node &node ) const;

    /// Returns the internal ID number of a translator that handles the
    /// translation of the given node.
    int		findShaderTranslatorID( const OP_Node &node ) const;


    /// Adds the generator to the list of known generator.
    void	registerPreviewShaderGenerator(HUSD_PreviewShaderGenerator &g);

    /// Removes the generator from the list of known generator.
    void	unregisterPreviewShaderGenerator(HUSD_PreviewShaderGenerator&g);

    /// Returns a generator that accepts the given render target mask.
    /// If no generator is found, returns nullptr.
    HUSD_PreviewShaderGenerator * findPreviewShaderGenerator(
					const OP_Node &node ) const;

    /// Removes all translators and generators from the registry.
    /// Should only be called on shutdown of the process.
    void	clear();

    /// Informs the registry about a new translation of node into a USD prim.
    void	reportShaderTranslation( const OP_Node &node, 
			const UT_StringRef &usd_shader_path );

    /// @{ Adds and removes a node from the translation observers list.
    /// Observers are basicaly interested in creation of any new USD shader
    /// primitive and the original VOP node based on which it was created.
    /// Translators report such creation events with reportShaderTranslation(),
    /// and observer LOPs can use that info to selectively re-translate
    /// single USD prim when a only single VOP changed.
    using TranslationRecord  = UT_Pair<int, UT_StringHolder>;
    using TranslationRecords = UT_Array<TranslationRecord>;
    void		    addTranslationObserver( const OP_Node &node );
    TranslationRecords	    removeTranslationObserver( const OP_Node &node );
    /// @}


private:
    /// List of known shader translators.
    UT_Array<HUSD_ShaderTranslator *>	    myTranslators;

    /// List of known preview shader generators.
    UT_Array<HUSD_PreviewShaderGenerator *> myGenerators;

    /// @{ IDs of translation observer nodes and translations reported for them.
    UT_Array<int>			    myTranslationObservers;
    UT_Array<TranslationRecords>	    myTranslations;
    /// @}
};

#endif
