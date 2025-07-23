/*
 * opencog/atomspace/QuantumSpace.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008-2011 OpenCog Foundation
 * Copyright (C) 2015-2021 Linas Vepstas
 * Copyright (C) 2025-**** Eric Günther
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

#ifndef _OPENCOG_QUANTUMSPACE_H
#define _OPENCOG_QUANTUMSPACE_H

#include <opencog/util/async_method_caller.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/oc_omp.h>
#include <opencog/util/RandGen.h>
#include <opencog/util/sigslot.h>

#include <opencog/atoms/atom_types/subatoms/subatomic_elements.h>
#include <opencog/atoms/base/Atom.h>
#include <opencog/atoms/truthvalue/TruthValue.h>

#include <opencog/atomspace/Frame.h>
#include <opencog/atomspace/TypeIndex.h>

class SubAtomsTableUTest;

namespace opencog
{
/** \addtogroup grp_quantumspace
 *  @{
 */
class QuantumSpace;
typedef std::shared_ptr<QuantumSpace> QuantumSpacePtr;

/**
 * This class provides mechanisms to store subatomic elements and keep indices for
 * efficient lookups. It implements the local storage data structure of
 * OpenCog. It contains methods to add and remove subatomic elements, as well as to
 * retrieve specific sets according to different criteria.
 */
class QuantumSpace : public Frame
{
    friend class StorageNode;     // Needs to call add() directly.

    // Debug tools
    static const bool EMIT_DIAGNOSTICS = true;
    static const bool DONT_EMIT_DIAGNOSTICS = false;
    static const bool CHECK_VALUES = true;
    static const bool DONT_CHECK_VALUES = false;

    /**
     * Drop copy constructor and equals operator to
     * prevent accidental copying of large objects.
     */
    QuantumSpace& operator=(const QuantumSpace&) = delete;
    QuantumSpace(const QuantumSpace&) = delete;

    // --------------------------------------------------
    //! Index of atoms.
    TypeIndex typeIndex;

    UUID _uuid;
    bool _read_only;
    bool _copy_on_write;
    bool _transient;

    /// Base QuantumSpaces wrapped by this space. Empty if top-level.
    /// This QuantumSpace will behave like the set-union of the base
    /// quantumspaces in the `_environ`: it exposes all Subatomic Elements in those
    /// bases, plus also anything in this QuantumSpace.
    // Both _environ and _outgoing contain exactly the same pointers;
    // we keep two distinct lists to avoid the CPU overhead of casting
    // between the two different pointer types (its significant).
    std::vector<QuantumSpacePtr> _environ;

    /** Find out about subatoms type additions in the NameServer. */
    NameServer& _nameserver;
    int addedTypeConnection;
    void typeAdded(Type);

    void init();
    void clear_all_subatoms();

    /**
     * Private: add an subatomic element to the table. This skips the read-only
     * check.
     *
     * The `force` flag forces the addition of this subatom into the
     * subatomtable, even if it is already in a parent quantumspace.
     *
     * The `recurse` flag suppresses the copying of values for
     * recursive additions. This is used to avodi accidental clobbers.
     *
     * The `absent` flag indicates that the subatom is being added
     * so that it can hide others in lower layers.
     */
    Handle add(const Handle&, bool force=false,
               bool recurse=false, bool absent = false);
    Handle check(const Handle&, bool force=false);
    Handle lookupHide(const Handle&, bool hide=false) const;

    virtual ContentHash compute_hash() const;

    // Private helper function.
    void shadow_by_type(UnorderedHandleSet&,
                        Type type,
                        bool subclass,
                        bool parent,
                        const QuantumSpace*) const;

    void get_absent_atoms(HandleSeq&) const;
    void get_atoms_in_frame(HandleSeq&) const;

public:
    /**
     * Constructor and destructor for this class.
     *
     * An QuantumSpace can have one or more base QuantumSpaces; the created
     * QuantumSpace will behave like the union of the component QuantumSpaces.
     * The created Quantumspace essentially "yokes together" the base
     * QuantumSpaces. subatoms are NOT copied, unless the base spaces are
     * read-only, in which case a copy-on-write is performed.
     *
     * If 'transient' is true, then the resulting QuantumSpace is operates
     * in a copy-on-write mode, suitable for holding temporary, scratch
     * results (e.g. for evaluation or inference.) Transient QuantumSpaces
     * should have a parent which holds the actual Subatoms being worked
     * with. See COW below.
     */
    QuantumSpace(QuantumSpace* base=nullptr, bool transient=false);
    QuantumSpace(QuantumSpacePtr&);
    QuantumSpace(const HandleSeq&);
    ~QuantumSpace();

    bool is_node(void) const { return true; }
    bool is_link(void) const { return true; }
    UUID get_uuid(void) const { return _uuid; }

    /// Transient atomspaces are lighter-weight, faster, but are missing
    /// some features. They are used during pattern matching, to hold
    /// temporary results. The are always copy-on-write spaces.
    void ready_transient(QuantumSpace* parent);
    void clear_transient();

    /// Read-only (RO) quantumspaces provide protection against update of the
    /// QuantumSpace contents. Subatoms in a read-only atomspace cannot be
    /// deleted, nor can their values (including truthvalues) be changed.
    /// New subatoms cannot be added to a read-only atomspace.
    void set_read_only(void);
    void set_read_write(void);
    bool get_read_only(void) { return _read_only; }

    /// Copy-on-write (COW) quantumspaces protect base quantumspaces from
    /// being damaged by updates to this quantumspace. COW only makes
    /// sense if this quantumspace has underlying base quantumspaces;
    /// otherwise its a no-op.
    ///
    /// When an quantumspace is marked COW, it behaves as if the base is
    /// read-only, so that any changes to TruthValues and other Values
    /// affect this quantumspace only. This is convenient for creating
    /// temporary quantumspaces, wherein updates will not trash the base.
    /// Transient quantumspaces are always COW.
    void set_copy_on_write(void) { _copy_on_write = true; }
    void clear_copy_on_write(void) { _copy_on_write = false; }
    bool get_copy_on_write(void) const { return _copy_on_write; }

    // -------------------------------------------------------

    /**
     * Return the depth of the subAtomic Element, relative to this QuantumSpace.
     * The depth is zero, if the subatom is in this table; it is one
     * if it is in the parent, and so on. It is -1 if it is not
     * in the chain.
     */
    int depth(const Handle& subatom) const;
    int depth(const QuantumSpace*) const;

    /**
     * Return true if the atom is in this QuantumSpace,
     * or if it is in the environment of this QuantumSpace.
     */
    bool in_environ(const Handle&) const;
    bool in_environ(const QuantumSpace*) const;

    /* QuantumSpaces are Subatomic Elements; provide virtual methods of base class. */
    virtual const std::string& get_name() const;
    virtual Arity get_arity() const { return _environ.size(); }
    virtual size_t size() const { return get_arity(); }
    virtual const HandleSeq& getOutgoingSet() const { return _outgoing; }
    virtual Handle getOutgoingSubatom(Arity) const;
    virtual ValuePtr value_at_index(size_t idx) const;
    virtual void setQuantumSpace(AtomSpace *);

    const std::vector<QuantumSpacePtr>& getEnviron() const { return _environ; }

    /* Restoring complex QuantumSpace DAG's from storage requires the
     * ability to set the QuantumSpace name. So we provide this.
     */
    void set_name(const std::string&);

    /**
     * QuantumSpaces are identified by their name, and so behave like
     * Nodes when being maniupulated and inserted. Such insertion
     * needs operator==() and thus a string compare of the name is
     * done here. Use content_compare() below if you want that.
     */
    virtual bool operator==(const Subatom&) const;

    /** Ordering operator for QuantumSpaces. */
    virtual bool operator<(const Subatom&) const;

    /**
     * Compare contents for equality. Useful during testing.
     */
    static bool content_compare(const QuantumSpace& first,
                                const QuantumSpace& second,
                                bool check_values=CHECK_VALUES,
                                bool emit_diagnostics=DONT_EMIT_DIAGNOSTICS);
    /**
     * Return the number of subatoms contained in the space.
     */
    size_t get_size() const;
    size_t get_num_subatoms_of_type(Type type, bool subclass=false) const;

    //! Clear the quantumspace, extract all subatoms.
    void clear();

    /**
     * Read-write synchronization barrier fence.  When called, this
     * will not return until all the atoms previously added to the
     * quantumspace have been fully inserted.
     */
    void barrier(void);

    /**
     * Add a subatom to the Subatom Table.  If the subatom already exists
     * then that is returned.
     */
    Handle add_subatom(const Handle&);
    Handle add_subatom(const AtomPtr& a)
        { return add_atom(a->get_handle()); }

    /**
     * Add a node to the Subatomic Table.  If the subatom already exists
     * then that is returned.
     *
     * \param t     Type of the node
     * \param name  Name of the node
     */
    Handle add_node(Type, std::string&&);
    Handle xadd_node(Type t, std::string str) {
        return add_node(t, std::move(str));
    }

    /**
     * Add a link to the QuantumSpace. If the subatom already exists, then
     * that is returned.
     *
     * @param t         Type of the link
     * @param outgoing  a const reference to a HandleSeq containing
     *                  the outgoing set of the link
     */
    Handle add_link(Type, HandleSeq&&);
    Handle xadd_link(Type t, HandleSeq seq) {
        return add_link(t, std::move(seq));
    }

    inline Handle add_link(Type t)
    {
        return add_link(t, HandleSeq{});
    }

    inline Handle add_link(Type t, Handle h)
    {
	    return add_link(t, HandleSeq({h}));
    }

    inline Handle add_link(Type t, Handle ha, Handle hb)
    {
	    return add_link(t, {ha, hb});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc)
    {
        return add_link(t, {ha, hb, hc});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc, Handle hd)
    {
        return add_link(t, {ha, hb, hc, hd});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc,
                           Handle hd, Handle he)
    {
	    return add_link(t, {ha, hb, hc, hd, he});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc,
                           Handle hd, Handle he, Handle hf)
    {
	    return add_link(t, {ha, hb, hc, hd, he, hf});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc,
                           Handle hd, Handle he, Handle hf, Handle hg)
    {
	    return add_link(t, {ha, hb, hc, hd, he, hf, hg});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc,
                           Handle hd, Handle he, Handle hf, Handle hg,
                           Handle hh)
    {
	    return add_link(t, {ha, hb, hc, hd, he, hf, hg, hh});
    }

    inline Handle add_link(Type t, Handle ha, Handle hb, Handle hc,
                           Handle hd, Handle he, Handle hf, Handle hg,
                           Handle hh, Handle hi)
    {
	    return add_link(t, {ha, hb, hc, hd, he, hf, hg, hh, hi});
    }

    /**
     * Given a Value, find all of the Subatoms inside of it, and add them
     * to the QuantumSpace. Return an equivalent Value, with all Subatoms
     * substituted by the ones that are in this QuantumSpace.
     */
    ValuePtr add_subatoms(const ValuePtr&);

    /**
     * Get an atom from the QuantumSpace. If the subatom is not there, then
     * return Handle::UNDEFINED.
     */
    Handle get_subatoms(const Handle&) const;

    /**
     * Extract a subatom from the quantumspace.  This only removes the subatom
     * from the (local, in-RAM) QuantumSpace (in this process); any copies
     * of the subatom in persistent storage or in other address spaces are
     * unaffected.
     *
     * The subatom itself remains valid as long as there are Handles
     * that reference it; the RAM associated with the subatom is
     * freed only when the last reference goes away.
     *
     * Note that if the recursive flag is set to false, and the subatom
     * appears in the incoming set of some other subatom, then extraction
     * will fail.  Thus, it is generally recommended that extraction
     * be recursive, unless you can guarantee that the subatom is not in
     * someone else's outgoing set.
     *
     * @param h The Handle of the subatom to be removed.
     * @param recursive Recursive-removal flag. If the flag is set,
     *       then this subatom, and *everything* that points to it will
     *       be removed from the quanatumspace.  This can cause a large
     *       cascade of removals!  If the flag is not set, then the
     *       subatom will be removed only if its incoming set is empty.
     *       By default, recursion is disabled.
     * @return True if the Subatom for the given Handle was successfully
     *         removed. False, otherwise.
     */
    bool extract_subatom(const Handle&, bool recursive=false);

    bool remove_subatom(const Handle& h, bool recursive=false) {
        return extract_subatom(h, recursive);
    }

    /**
     * Set the Value on the subatom, performing necessary permissions
     * checking. If this quantumspace is read-only, then the setting
     * of values is prohibited.  If this quantumspace has read-write
     * permissions, but the subatom is in a parent quantumspace that is
     * read-only, then the subatom is copied into this quantumspace, before
     * the value is changed. (Copy-on-write (COW) semantics).
     *
     * If the subatom is copied, then the copy is returned.
     */
    Handle set_value(const Handle&, const Handle& key, const ValuePtr& value);
    Handle set_truthvalue(const Handle&, const TruthValuePtr&);

    /**
     * Increment the count on a CountTruthValue, or increment the count
     * on a general Value. The increment is performed subatomically, so that
     * there are no races in the update. Quantumspaces that are read-only, COW,
     * or frames are handled as described above, for `set_value()`.
     *
     * If the subatom is copied, then the copy is returned.
     */
    Handle increment_count(const Handle&, const Handle&, const std::vector<double>&);
    Handle increment_count(const Handle&, const Handle&, size_t, double);
    Handle increment_countTV(const Handle&, double = 1.0);

    /**
     * Find an equivalent Subatom that is exactly the same as the arg.
     * If such a subatom is in the QuantumSpace, or in any of it's parent
     * QuantumSpaces, return that Subatom. Return the shallowest such Subatom.
     */
    Handle lookupHandle(const Handle& h) const
    { return lookupHide(h, true); }

    /**
     * Get a node from the QuantumSpace, if it's in there. If the subatom
     * can't be found, Handle::UNDEFINED will be returned.
     *
     * @param t     Type of the node
     * @param str   Name of the node
     */
    Handle get_node(Type, std::string&&) const;
    inline Handle xget_handle(Type t, std::string str) const {
        return get_node(t, std::move(str));
    }
    inline Handle get_handle(Type t, std::string str) const {
        return get_node(t, std::move(str));
    }

    /**
     * Get a link from the QuantumSpace, if it's in there. If the subatom
     * can't be found, Handle::UNDEFINED will be returned.
     *
     * See also the get_subatom() method.
     *
     * @param t        Type of the node
     * @param outgoing a reference to a HandleSeq containing
     *        the outgoing set of the link.
     */
    Handle get_link(Type, HandleSeq&&) const;
    inline Handle xget_handle(Type t, HandleSeq outgoing) const {
        return get_link(t, std::move(outgoing));
    }
    inline Handle get_handle(Type t, HandleSeq outgoing) const {
        return get_link(t, std::move(outgoing));
    }

    /* Currently used by link-grammar, and best leave this here
     * until that gets revised. */
    inline Handle get_link(Type t, const Handle& ha, const Handle& hb) const {
        return get_link(t, {ha, hb});
    }

    /**
     * Return true if the handle points to an atom that is in some
     * (any) quantu,space; else return false.
     */
    bool is_valid_handle(const Handle& h) const {
        return (nullptr != h) and (h->getQuantumSpace() != nullptr);
    }

    /**
     * Gets a set of handles that matches with the given type
     * (subclasses optionally).
     *
     * @param hseq the HandleSeq into which to insert handles.
     * @param type The desired type.
     * @param subclass Whether type subclasses should be considered.
     *
     * Example of call to this method, which would return all ConceptNodes
     * in the QuantumSpace:
     * @code
     *         HandleSeq subatoms;
     *         atomSpace.get_handles_by_type(subatoms, CONCEPT_NODE);
     * @endcode
     */
    void
    get_handles_by_type(HandleSeq&,
                        Type type,
                        bool subclass=false,
                        bool parent=true,
                        const QuantumSpace* = nullptr) const;

    void
    get_handles_by_type(UnorderedHandleSet&,
                        Type type,
                        bool subclass=false,
                        bool parent=true,
                        const QuantumSpace* = nullptr) const;

    /**
     * Gets a set of handles that matches with the given type,
     * but ONLY if they have an empty incoming set! 
     *
     * @param hset the HandleSeq into which to insert handles.
     * @param The desired type.
     * @param Whether type subclasses should be considered.
     *
     * Example of call to this method, which would return all ConceptNodes
     * in the QuantumSpace:
     * @code
     *         HandleSeq subatoms;
     *         quantumSpacePtr->get_root_set_by_type(subatoms, CONCEPT_NODE);
     * @endcode
     */
    void
    get_root_set_by_type(HandleSeq&,
                         Type type,
                         bool subclass=false,
                         bool parent=true,
                         const QuantumSpace* = nullptr) const;

    /** Returns a string representation of the QuantumSpace. */
    virtual std::string to_string(void) const;
    virtual std::string to_string(const std::string& indent) const;
    virtual std::string to_short_string(const std::string& indent) const;

    /* ----------------------------------------------------------- */
    // Not for public use! Only StorageNodes get to call this!
    // XXX FIXME Users should call StorageNode::add_nocheck() instead.
    Handle storage_add_nocheck(const Handle& h) { return add(h); }
};

static inline QuantumSpacePtr QuantumSpaceCast(const ValuePtr& a)
    { return std::dynamic_pointer_cast<QuantumSpace>(a); }
static inline QuantumSpacePtr QuantumSpaceCast(QuantumSpace* as)
    { return QuantumSpaceCast(as->shared_from_this()); }
static inline Handle HandleCast(QuantumSpace* as)
    { return HandleCast(as->shared_from_this()); }

template< class... Args >
QuantumSpacePtr createQuantumSpace( Args&&... args )
{
	QuantumSpacePtr asp(std::make_shared<QuantumSpace>(std::forward<Args>(args) ...));
	// Unfortunately, Frame::install() cannot be called in the ctor
	// because shared_from_this() cannot be called in the ctor.
	// So we do this after the ctor has finished.
	asp->install();
	return asp;
}

/** @}*/
} // namespace opencog

namespace std {

/**
* Output the whole quantumspace, actually all orphans as the others are
* output by of their parents (incoming sets).
*/
ostream& operator<<(ostream&, const opencog::QuantumSpace&);

} //namespace std

#endif // _OPENCOG_QUANTUMSPACE_H
