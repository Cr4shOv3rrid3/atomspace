/*
 * opencog/atoms/atom_types/subatomic_elements/subatomic_elements.cc
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

#include subatomic_elements.h

namespace opencog
{
/** \addtogroup grp_quantumspace
 *  @{
 */

opencog::subType element;

//just 3 different types and combination of all of em can build the main essential basis towards a Hydrogen Atom!
element.push_back("Proton"); //positive electric charge, +1 e (elementary charge) (1886 Eugen Goldstein)
element.push_back("Neutron"); //no electric charge, mass slightly greater than a proton (yep 1 line to precise) (1932 James Chadwick)
element.push_back("Electron"); //see appropriate subatomic_elements.cc line where it is mentioned as well!
//and the graphics designer understands the Atom in general Understanding as the following!
element.push_back("Pixel"); //word creation stands for: Picture element! (1888/1911/1927/1972: German Bildpunkt/US Patents/Wirleless World magazine/"picture cell" IBM)
//we further need the resonating Brainwave state anchor when it comes to Volts and Ampere electrical speed vs. width/size of current resonance
element.push_back("2xMicroVolt"); //minimum stable speed indicator multiplication 4 the Brainwaves travel velocity.
element.push_back("10xPicoAmpere"); //we wont get the necessary stable running resonance without that multiplication factor! No Grant Card[ON/OFF] Access Possible without!
/** @}*/
} // namespace opencog

#endif // _OPENCOG_SUBTYPES_H
