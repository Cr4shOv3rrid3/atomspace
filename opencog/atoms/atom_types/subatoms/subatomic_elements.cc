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

//we go with Quarks first! and to give it more Stlye its best to relate this zoo of element words into animal styles for Kung-Fu too!
element.push_back("Up"); //Crane (Hequan)
element.push_back("Down"); //Tiger (Huxingquan)
element.push_back("Charm"); //Leopard (Baoquan)
element.push_back("Strange"); //Snake (Shequan)
element.push_back("Top"); //Ape (Houquan)
element.push_back("Bottom"); //Dragon (Longquan)
//then comes the Leptones! this as well relates to the animal scheme above but with different animals
element.push_back("Electron"); //cockerell
element.push_back("E-Neutrino"); //hen or if no hen is there: corn to pick fights with others
element.push_back("Myon"); //Mantis (decided for it cause its tiny like one and also beginns with a M).
element.push_back("M-Neutrino"); //its prey (mostly other insects but if its too much work you could also place a Horse here. with a bit of Horsey work you can change an "M" into an "H" i assume and start to Laugh (HaHaHa). *bends character uouff*)
element.push_back("Tauon"); //Bull (mainly cause its nearest to Taurus the astrological sign also having a Bull and guess what it also begins with a "T")
element.push_back("T-Neutrino"); //Crocodile (similar force like a Bull but in another way but still similar lazyness, i mean a Neutrino is related to a Neutron. what is neutral. like: doing nothing at all! or in terms of IT = *computer goes idle mode')
//then comes the Vector Bosons! (kind of the Bosses in the Quantum Realm!)
element.push_back("Gluon"); //Boss 1 (Master Snake = can "Glue" onto your body without you getting rid of it, that's very sneaky!)
element.push_back("Photon"); //Boss 2 (Master Leopard = highest speed you only get by firing those fists like a Photon Torpedo in Sci- Fi Movies!)
element.push_back("Z-Boson"); //Boss 3 (Master Crane = Z direction is always the best way up and so is the Ability to Fly up into to the Sky)
element.push_back("W-Boson"); //Boss 4 (Master Tiger = he doesnt care if he attacks you in zigzag ways like a W he just does!)
//Last but not least the Scalar Boson! (Master Boss!)
element.push_back("Higgs"); //Nope that wasnt a Hiccup! Really! but we could make a Gorilla the best Overall Boss (nearest to a human Skeletton anyways and most realistic therefore, i mean try to transform a human body into any of the other, would look a bit weird wont it?)
//one animal so to say is still missig. but you dont need to Master a Dragon. cause guess what: IT DOESN'T EXIST DUMMY, IT'S A MADE UP CREATURE BY FOLKLORE and TALES that probably came out as a mixture idea of a snake a cockerell and a fish at least neareast to it!
//another idea is combining all the other animals into this one. i mean we could get the wings from the crane realised. 
//but we also kind of running into the same old problem now. how am are we supposed to make a human body a Dragon shape? we canot fly! at least in real body sense. 
//so i guess the dragon still stands for mostly internal and thought aspects hence: strategic relevance like putting him last here. what makes much more sense anyways to hide this knowledge deep down here in the Dragons lair!
//But if you still prefer to go with a Dragon to make it your Ultimate Mastered Boss thingy anyways. i won't hold you back and will redirect you to Shenlong next *redirecting*->. Now make your wish becoming one! (pretending you have all the required DragonBalls you need here gathered!)
//but also know as Son Goku is still kind of the main star to that show as well. we are kind of back at this Big maybe Golden Gorilla ;)
//End of Story = End of Life !!!(what could make sense for people against the zoo power of the Quantum Realm. yeah Dragonpower as its finest i guess, cause gues what, they destroy things and are not nice commonly to people!!)
//Welcome 2 Quantum Realm Reality!!!
/** @}*/
} // namespace opencog

#endif // _OPENCOG_SUBTYPES_H
