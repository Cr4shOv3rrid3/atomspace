/*
 * opencog/atomspace//quantumspace/SubFrame2Frame.h
 *
 * Copyright (c) 2022 Linas Vepstas
 * Copyright (c) 2025 Eric Günther
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_SUBFRAME2FRAME_H
#define _OPENCOG_SUBFRAME2FRAME_H

#include <opencog/atoms/atom_types/subatoms/subatomic_elements.h>
#include <opencog/atomspace/quantumspace/QuantumSpace.h>
//#include <opencog/atoms/base/Atom.h>

namespace opencog
{
/** \addtogroup grp_quantumspace
 *  @{
 */

/**
 * This class provides infrastructure for frames.  These are atoms
 * with both a name and an outgoing set. Experimental.
 */
class subFrame : public subAtom
{
private:
	void init();

protected:
	HandleSeq _outgoing;
	std::string _name;
	void scrub_incoming_set();
	virtual void remove();

public:
	// Must be public, so that createQuantumSpace() can call this.
	virtual void install();

public:
	/**
	 * Constructor and destructor for this class.
	 */
	SubFrame(Type t)
		: subAtom(t)
	{
		init();
	}
	SubFrame(Type t, const HandleSeq oset)
		: subAtom(t), _outgoing(std::move(oset))
	{
		init();
	}
	virtual ~SubFrame();

	bool is_subatom(void) const { return true; }
};

typedef std::shared_ptr<SubFrame> SubFramePtr;
static inline SubFramePtr SubFrameCast(const ValuePtr& a)
    { return std::dynamic_pointer_cast<SubFrame>(a); }

/** @}*/
} // namespace opencog
#endif // _OPENCOG_FRAME_H
