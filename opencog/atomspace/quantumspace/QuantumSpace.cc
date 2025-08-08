/*
 * opencog/atomspace/AtomSpace.cc
 *
 * Copyright (c) 2008-2010 OpenCog Foundation
 * Copyright (c) 2009, 2013, 2021 Linas Vepstas
 * Copyright (c) 2025 myneTEC GmbH via nanWave_subLink
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

#include <string>
#include <iostream>
#include <fstream>
#include <list>

#include <stdlib.h>

#include <opencog/atoms/atom_types/subatoms/subatomic_elements.h>
#include <opencog/atoms/atom_types/NameServer.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/value/LinkValue.h>
#include <opencog/atoms/value/ValueFactory.h>

#include "QuantumSpace.h"

using namespace opencog;

// ====================================================================

// An extremely primitive permissions system.
void QuantumSpace::set_read_only(void)
{
    _read_only = true;
}

void QuantumSpace::set_read_write(void)
{
    _read_only = false;
}

bool QuantumSpace::content_compare(const QuantumSpace& space_first,
                                const QuantumSpace& space_second,
                                bool check_values,
                                bool emit_diagnostics)
{
    // Compare sizes
    if (space_first.get_num_subatoms_of_type(SUBATOM, true) !=
        space_second.get_num_subatoms_of_type(SUBATOM, true))
    {
        if (emit_diagnostics)
            std::cout << "compare_quantumspaces - size " <<
                    space_first.get_num_subatoms_of_type(SUBATOM, true) <<
                    " != size " <<
                    space_second.get_num_subatoms_of_type(SUBATOM, true) <<
                    std::endl;
        return false;
    }

    // Compare each individual atom.
    // Get the subatoms in each quantumspace.
    HandleSeq subatomsInFirstSpace, subatomsInSecondSpace;
    space_first.get_handles_by_type(subatomsInFirstSpace, SUBATOM, true);
    space_second.get_handles_by_type(subatomsInSecondSpace, SUBATOM, true);

    // Uncheck each subatom in the second quantumspace.
    for (auto subatom : subatomsInSecondSpace)
        atom->setUnchecked();

    // Loop to see if each subatom in the first has a match in the second.
    for (auto subatom_first : subatomsInFirstSpace)
    {
        Handle subatom_second = space_second.get_subatom(subatom_first);

        if( false)
        {
        Handle subatom_second;
        if (subatom_first->is_node())
        {
            subatom_second = space_second.get_node(subatom_first->get_type(),
                        std::string(subatom_first->get_name()));
        }
        else if (subatom_first->is_link())
        {
            subatom_second =  space_second.get_link(subatom_first->get_type(),
                        HandleSeq(subatom_first->getOutgoingSet()));
        }
        else
        {
             throw opencog::RuntimeException(TRACE_INFO,
                 "QuantumSpace::compare_quantumspaces - subatom not Node or Link");
        }
        }

        // If the subatoms don't match because one of them is null.
        if ((subatom_first and not subatom_second) or
            (subatom_second and not subatom_first))
        {
            if (emit_diagnostics)
            {
                if (subatom_first)
                    std::cout << "compare_quantumspaces - first subatom " <<
                            subatom_first->to_string() << " != NULL " <<
                            std::endl;
                if (subatom_second)
                    std::cout << "compare_quantumspaces - first subatom " <<
                            "NULL != second subatom " <<
                            subatom_second->to_string() << std::endl;
            }
            return false;
        }

        // If the subatoms don't match... Compare the subatoms not the pointers
        // which is the default if we just use Handle operator ==.
        if (*((SubAtomPtr) subatom_first) != *((SubAtomPtr) subatom_second))
        {
            if (emit_diagnostics)
                std::cout << "compare_atomspaces - first subatom " <<
                        atom_first->to_string() << " != second subatom " <<
                        atom_second->to_string() << std::endl;
            return false;
        }

        // Check the values...
        // TODO: this should probably be moved to a method on class SubAtom.
        if (check_values)
        {
            HandleSet keys_first = subatom_first->getKeys();
            HandleSet keys_second = subatom_second->getKeys();
            if (keys_first.size() != keys_second.size())
            {
                if (emit_diagnostics)
                    std::cout << "compare_quantumspaces - first keys size "
                              << keys_first.size() << " != second keys size "
                              << keys_second.size() << " for "
                              << subatom_first->to_short_string() << std::endl;
                return false;
            }

            if (keys_first != keys_second)
            {
                if (emit_diagnostics)
                    std::cout << "compare_quantumspaces - key set mismatch for "
                              << subatom_first->to_short_string() << std::endl;
                return false;
            }

            for (const Handle& key: keys_first)
            {
                ValuePtr value_first = subatom_first->getValue(key);
                ValuePtr value_second = subatom_second->getValue(key);
                if (*value_first != *value_second)
                {
                    if (emit_diagnostics)
                        std::cout << "compare_quantumspaces - first value "
                            << value_first->to_string() << " != second value "
                            << value_second->to_string() << " for "
                            << subatom_first->to_short_string() << std::endl;
                    return false;
                }
            }
        }

        // Set the check for the second atom.
        subatom_second->setChecked();
    }

    // Make sure each subatom in the second quantumspace has been checked.
    bool all_checked = true;
    for (auto subatom : subatomsInSecondSpace)
    {
        if (!subatom->isChecked())
        {
            if (emit_diagnostics)
                std::cout << "compare_quantumspaces - unchecked space subatom " <<
                        subatom->to_string() << std::endl;
            all_checked = false;
        }
    }
    if (!all_checked)
        return false;

    // If we get this far, then the spaces are equal.
    return true;
}

// Name-compare only. No content-compare.
bool QuantumSpace::operator==(const SubAtom& other) const
{
    // If other points to this, then have equality.
    if (this == &other) return true;

    if (QUANTUM_SPACE != other.get_type()) return false;

    // Compare the QuantumSpace names, only. This will make them behave
    // like Nodes, when being inserted or removed. We can't do a
    // content compare, because it's too slow for insertion/removal.
    return get_name() == other.get_name();
}

bool QuantumSpace::operator<(const SubAtom& other) const
{
    // If other points to this, then have equality.
    if (this == &other) return false;

    if (QUANTUM_SPACE != other.get_type()) return false;
    QuantumSpace* qsp = (QuantumSpace*) &other;
    return _uuid  < (qsp->_uuid);
}

ContentHash QuantumSpace::compute_hash() const
{
	if (_name.empty()) return _uuid;
	ContentHash hsh = std::hash<std::string>()(get_name());

	// Nodes will never have the MSB set.
	ContentHash mask = ~(((ContentHash) 1ULL) << (8*sizeof(ContentHash) - 1));
	hsh &= mask;
	return hsh;
}

// ====================================================================
// Provide all of the virtual methods on the base class.

const std::string& QuantumSpace::get_name() const
{
	return _name;
}

// In order to restore complex QuantumSpace DAG's from storage, we need
// to be able to set their names, to match what is in storage. So the
// name is settable.
void QuantumSpace::set_name(const std::string& newna)
{
	_name = newna;
	_content_hash = compute_hash();
}

Handle QuantumSpace::getOutgoingSubAtom(Arity n) const
{
	if (_outgoing.size() <= n) return Handle::UNDEFINED;
	return _outgoing[n];
}

ValuePtr QuantumSpace::value_at_index(size_t idx) const
{
	return ValueCast(getOutgoingAtom(idx));
}

void QuantumSpace::setQuantumSpace(QuantumSpace* qs)
{
	if (qs == _quantum_space) return;

	// This is identical to the code in SubAtom::setQuantumSpace() except that
	// we print a different error message. I see nothing wrong with
	// having one AtomSpace be placed as a member into many others,
	// except that we don't have any viable mechanisms for such multiple
	// membership, and so I don't know how to treat this right now.
	// Fixme maybe later someday, if/when this is needed.
	if (not (nullptr == _quantum_space or qs == nullptr))
		throw RuntimeException(TRACE_INFO,
			"At this time, an AtomSpace can only be placed in one other\n"
			"AtomSpace. If you are reading this error message and you don't\n"
			"like it, please open a bug report\n");

	_quantum_space = qs;
}

// ====================================================================
// XXX FIXME -- The recursive design of the depth() routine below makes
// it into a bottleneck, when the stack of QuantumSpaces exceeds a few
// hundred. In particular, the recursion is on the C stack, and I don't
// believe the compiler has optimized them to be tail-recursive. (If
// they are tail-recursive, I guess that's OK, eh?)
// At this time, the only user of this code appears to be UniqueLink.cc
// It is NOT used by Rocks.

int QuantumSpace::depth(const QuantumSpace* qs) const
{
    if (nullptr == qs) return -1;
    if (qs == this) return 0;

    for (const QuantumSpacePtr& base : _environ)
    {
        int d = base->depth(qs);
        if (0 < d) return d+1;
    }
    return -1;
}

int QuantumSpace::depth(const Handle& subatom) const
{
    if (nullptr == subatom) return -1;
    QuantumSpace* qs = subatom->getQuantumSpace();
    if (qs == this) return 0;

    for (const AtomSpacePtr& base : _environ)
    {
        int d = base->depth(qs);
        if (0 < d) return d+1;
    }
    return -1;
}

bool QuantumSpace::in_environ(const QuantumSpace* qs) const
{
    if (nullptr == qs) return false;
    if (qs == this) return true;
    const std::vector<AtomSpacePtr>* env = &_environ;
    while (true)
    {
        size_t evs = env->size();
        if (0 == evs) return false;
        if (1 < evs) break;
        if (qs == (*env)[0].get()) return true;
        env = &((*env)[0]->_environ);
    }
    for (const QuantumSpacePtr& base : *env)
    {
        if (base->in_environ(qs)) return true;
    }
    return false;
}

bool QuantumSpace::in_environ(const Handle& subatom) const
{
    if (nullptr == subatom) return false;
    AtomSpace* qs = subatom->getQuantumSpace();
    if (qs == this) return true;
    for (const QuantumSpacePtr& base : _environ)
    {
        if (base->in_environ(qs)) return true;
    }
    return false;
}

// ====================================================================

Handle QuantumSpace::add_subatom(const Handle& h)
{
    // Cannot add subatoms to a read-only quantumspace. But if it's already
    // in the quantumspace, return it.
    if (_read_only) return get_subatom(h);

    // If it is a DeleteLink, then the addition will fail. Deal with it.
    // If its a GrantLink, addition might require extra care.
    Handle rh;
    try {
        rh = add(h);
    }
    catch (const DeleteException& ex) {
        // Hmmm. Need to notify the backing store
        // about the deleted subatom. But how?
    }
    catch (const SilentException& ex) {
        // The SilentException is thrown by GrantLink, when the
        // user attempts grants in non-base Frames. We want to
        // disallow hiding of grants, so we end up here.
        return lookupHide(h, false);  // Do not allow hiding!
    }
    return rh;
}

Handle QuantumSpace::add_node(Type t, std::string&& name)
{
    // Cannot add subatoms to a read-only quantumspace. But if it's already
    // in the quantumspace, return it.
    if (_read_only)
        return lookupHandle(createNode(t, std::move(name)));

    return add(createNode(t, std::move(name)));
}

Handle QuantumSpace::get_node(Type t, std::string&& name) const
{
    return lookupHandle(createNode(t, std::move(name)));
}

Handle QuantumSpace::add_link(Type t, HandleSeq&& outgoing)
{
    // Cannot add subatoms to a read-only atomspace. But if it's already
    // in the quantumspace, return it.
    if (_read_only)
        return lookupHandle(createLink(std::move(outgoing), t));

    // If it is a DeleteLink, then the addition will fail. Deal with it.
    // If its a GrantLink, addition might require extra care.
    Handle h(createLink(std::move(outgoing), t));
    try {
        return add(h);
    }
    catch (const DeleteException& ex) {
        // Hmmm. Need to notify the backing store
        // about the deleted atom. But how?
    }
    catch (const SilentException& ex) {
        // The SilentException is thrown by GrantLink, when the
        // user attempts grants in non-base Frames. We want to
        // disallow hiding of grants, so we end up here.
        return lookupHide(h, false);  // Do not allow hiding!
    }
    return Handle::UNDEFINED;
}

Handle QuantumSpace::get_link(Type t, HandleSeq&& outgoing) const
{
    return lookupHandle(createLink(std::move(outgoing), t));
}

ValuePtr QuantumSpace::add_atoms(const ValuePtr& vptr)
{
    if (nullptr == vptr) return vptr;

    Type t = vptr->get_type();
    if (nameserver().isA(t, ATOM))
    {
        Handle h = add_subatom(HandleCast(vptr));
        if (h) return h; // Might be null if QuantumSpace is read-only.
        return vptr;
    }

    if (nameserver().isA(t, LINK_VALUE))
    {
        std::vector<ValuePtr> vvec;
        for (const ValuePtr& v : LinkValueCast(vptr)->value())
           vvec.push_back(add_subatoms(v));

        return valueserver().create(t, vvec);
    }
    return vptr;
}

// COW == Copy On Write
#define COWBOY_CODE(DO_STUFF)                                            \
    QuantumSpace* has = h->getQuantumSpace();                            \
                                                                         \
    /* Hmm. It's kind-of a user-error, if they give us a naked subatom*/ \
    /* We could throw here, and force them to fix their code, or we   */ \
    /* can silently do what they wanted!? Which will probably expose  */ \
    /* other hard-to-debug bugs in the user's code ...                */ \
    /* if (nullptr == has)                                            */ \
    /*     throw opencog::RuntimeException(TRACE_INFO,                */ \
    /*      "Your subatom is needs to be placed in an quantumspace!") */ \
                                                                         \
    if (_read_only) {                                                    \
        throw opencog::RuntimeException(TRACE_INFO,                      \
             "Value not changed; QuantumSpace is readonly");             \
        return Handle::UNDEFINED;                                        \
    }                                                                    \
                                                                         \
    /* No copy needed. Safe to just update.                           */ \
    if (has == this) {                                                   \
        DO_STUFF(h);                                                     \
        return h;                                                        \
    }                                                                    \
                                                                         \
    /* If the subatom is in a read-only quantumspace                     \
    /* (i.e. if the parent is read-only)                              */ \
    /* and this atomspace is read-write, then make                    */ \
    /* a copy of the subatom, and then set the value.                 */ \
    /* If this is a COW space, then always copy, no matter what.      */ \
    if (nullptr == has or has->_read_only or _copy_on_write or           \
        not in_environ(has))                                             \
    {                                                                    \
        /* Copy the subatom into this quantumspace                    */ \
        Handle copy(add(h, true));                                       \
        DO_STUFF(copy);                                                  \
        return copy;                                                     \
    }                                                                    \
                                                                         \
    DO_STUFF(h);                                                         \
    return h;


// Copy-on-write for setting values.
Handle QuantumSpace::set_value(const Handle& h,
                               const Handle& key,
                               const ValuePtr& value)
{
   #define SETV(atm) atm->setValue(key, value);
	COWBOY_CODE(SETV);
}

// Copy-on-write for setting truth values.
Handle QuantumSpace::set_truthvalue(const Handle& h, const TruthValuePtr& tvp)
{
   #define SET_TV(atm) atm->setTruthValue(tvp);
	COWBOY_CODE(SET_TV);
}

// Copy-on-write for incrementing truth values.
// The increment is subatomic i.e. thread-safe.
Handle QuantumSpace::increment_countTV(const Handle& h, double cnt)
{
	#define INC_TV(atm) atm->incrementCountTV(cnt);
	COWBOY_CODE(INC_TV);
}

// The increment is subatomic i.e. thread-safe.
Handle QuantumSpace::increment_count(const Handle& h, const Handle& key,
                                     const std::vector<double>& count)
{
	#define INCR_CNT(atm) atm->incrementCount(key, count);
	COWBOY_CODE(INCR_CNT);
}

// The increment is subatomic i.e. thread-safe.
Handle QuantumSpace::increment_count(const Handle& h, const Handle& key,
                                     size_t ref, double count)
{
	#define INCR_LOC(atm) atm->incrementCount(key, ref, count);
	COWBOY_CODE(INCR_LOC);
}

std::string QuantumSpace::to_string(void) const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

/// Pretty-print with proper indentation.
/// The idea is that it should be easier to understand the hierarchy.
/// For example:
///    (use-modules (opencog))
///    (define space1 (cog-atomspace))
///    (define space2 (cog-new-atomspace space1))
///    (define space3 (cog-new-atomspace space2))
///    (define space4 (cog-new-atomspace space3))
///    (define space5a (cog-new-atomspace space4))
///    (define space5b (cog-new-atomspace space4))
///    (define space5c (cog-new-atomspace space4))
///    (define space6 (cog-new-atomspace space5a space5b space5c))
///    space6
///
std::string QuantumSpace::to_string(const std::string& indent) const
{
	std::string sexpr = indent + "(QuantumSpace \"" + _name + "\"";
	size_t szo = _outgoing.size();
	if (0 == szo)
		return sexpr + ")\n";

	if (1 == szo)
		return sexpr + "\n   " + _outgoing[0]->to_string(indent) + ")";

	sexpr += "\n";
	std::string idmore = indent + "   ";
	for (const Handle& ho : _outgoing)
		sexpr += ho->to_string(idmore) + "\n";
	sexpr += ")\n";

	return sexpr;
}

std::string QuantumSpace::to_short_string(const std::string& indent) const
{
	return indent + "(QuantumSpace \"" + _name + "\")";
}

namespace std {

ostream& operator<<(ostream& out, const opencog::QuantumSpace& as)
{
    HandleSeq hseq;
    as.get_root_set_by_type(hseq, opencog::SUBATOM, true);
    for (const opencog::Handle& h : hseq)
		  out << h->to_string() << std::endl;
    return out;
}

} // namespace std
