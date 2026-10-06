/*
 *
 *	Copyright 2005-2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Auto_Descriptor_h_
	#define Auto_Descriptor_h_

#include "piactions.h"
#include "Errors.h"

#define THROW_IF_ERR(error)										\
	{ OSStatus _err(error); if (0 != _err) throw _err; }

// Wrapper for the Photoshop PIActionDescriptor
class	Auto_Descriptor
{
public:
	// Constructors:
				Auto_Descriptor(const PSActionDescriptorProcs* p_descriptor_procs);
				Auto_Descriptor(const PSDescriptorRegistryProcs* p_registry_procs,
								const PSActionDescriptorProcs* p_descriptor_procs,
								const char* p_key);
				~Auto_Descriptor()						{ m_descriptor_procs->Free(m_descriptor); }

	// Native type accessor:
	operator	PIActionDescriptor()					{ return m_descriptor; }

private:
	const PSActionDescriptorProcs*	m_descriptor_procs;
	PIActionDescriptor				m_descriptor;
};

// Make a new PIActionDescriptor
inline Auto_Descriptor::Auto_Descriptor(const PSActionDescriptorProcs* p_descriptor_procs)	:
						m_descriptor_procs(p_descriptor_procs)
{
	THROW_IF_ERR(p_descriptor_procs->Make(&m_descriptor));
}

// Retrieve the stored PIActionDescriptor with the given key
inline Auto_Descriptor::Auto_Descriptor(const PSDescriptorRegistryProcs* p_registry_procs,
						const PSActionDescriptorProcs* p_descriptor_procs, const char* p_key)	:
						m_descriptor_procs(p_descriptor_procs)
{
	THROW_IF_ERR(p_registry_procs->Get(p_key, &m_descriptor));
}

#endif
