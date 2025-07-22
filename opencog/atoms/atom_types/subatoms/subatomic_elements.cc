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

#include subatomic_elements.h

namespace opencog
{
/** \addtogroup grp_quantumspace
 *  @{
 */

opencog::subType element;

//we go with Quarks first!
element.push_back('Up');
element.push_back('Down');
element.push_back('Charm');
element.push_back('Strange');
element.push_back('Top');
element.push_back('Bottom');
//then comes the Leptones!
element.push_back('Electron');
element.push_back('E-Neutrino');
element.push_back('Myon');
element.push_back('M-Neutrino');
element.push_back('Tauon');
element.push_back('T-Neutrino');
//then comes the Vector Bosons!
element.push_back('Gluon');
element.push_back('Photon');
element.push_back('Z-Boson');
element.push_back('W-Boson');
//Last but not least theee...
element.push_back('Higgs'); //Nope that wasnt a Hiccup! Really!


