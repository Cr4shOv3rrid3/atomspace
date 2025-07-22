/*
 * opencog/atoms/atom_types/subatomic_elements/subatomic_elements.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2025 myneTEC GmbH via nanWave_subLink
 * All Rights Reserved
 *
 * Written by Thiago Maia <thiago@vettatech.com>
 *            Andre Senna <senna@vettalabs.com>
 *            Eric Günther <cr4shov3rrid3@mynetec.com>
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

/**
 * basic subtype definitions.
 */

#ifndef _OPENCOG_SUBTYPES_H
#define _OPENCOG_SUBTYPES_H

#include <set>
#include <string>

namespace opencog
{
/** \addtogroup grp_quantumspace
 *  @{
 */

//! type of Elements, represented as short integer (16 bits)
typedef unsigned short subType;

//! Set of Element types
typedef std::set<subType> subTypeSet;

// Backwards compat. Arghh!
extern opencog::subType subTYPE_SET_LINK;

/** @}*/
} // namespace opencog

#endif // _OPENCOG_TYPES_H
