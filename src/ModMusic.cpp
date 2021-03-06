
/*******************************************************************
 * SLADE - It's a Doom Editor
 * Copyright (C) 2008-2014 Simon Judd
 *
 * Email:       sirjuddington@gmail.com
 * Web:         http://slade.mancubus.net
 * Filename:    ModMusic.cpp
 * Description: ModMusic class, an SFML sound stream class to play
 *              mod music using DUMB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *******************************************************************/


/*******************************************************************
 * INCLUDES
 *******************************************************************/
#include "Main.h"
#include "ModMusic.h"
#include "dumb/dumb.h"


/*******************************************************************
 * VARIABLES
 *******************************************************************/
bool ModMusic::init_done = false;


/*******************************************************************
 * MODMUSIC CLASS FUNCTIONS
 *******************************************************************/

/* ModMusic::ModMusic
 * ModMusic class constructor
 *******************************************************************/
ModMusic::ModMusic()
{
	dumb_module = NULL;
	dumb_player = NULL;
}

/* ModMusic::~ModMusic
 * ModMusic class destructor
 *******************************************************************/
ModMusic::~ModMusic()
{
	close();
}

/* ModMusic::openFromFile
 * Loads a mod file for playback
 *******************************************************************/
bool ModMusic::openFromFile(const std::string& filename)
{
	// Init DUMB if needed
	if (!init_done)
		initDumb();

	// Close current module if any
	close();

	// Load module file
	dumb_module = dumb_load_any(filename.c_str(), 0, 0);
	if (dumb_module != NULL)
	{
		initialize(2, 44100);
		return true;
	}
	else
	{
		LOG_MESSAGE(1, "Failed to load module music file \"%s\"", filename.c_str());
		return false;
	}
}

/* ModMusic::getDuration
 * Returns the duration of the currently loaded mod
 *******************************************************************/
sf::Time ModMusic::getDuration() const
{
	return sf::seconds(static_cast<float>(duh_get_length(dumb_module) / 65536));
}

/* ModMusic::close
 * Closes and unloads any currently open mod
 *******************************************************************/
void ModMusic::close()
{
	if (dumb_player != NULL)
	{
		duh_end_sigrenderer(dumb_player);
		dumb_player = NULL;
	}
	if (dumb_module != NULL)
	{
		unload_duh(dumb_module);
		dumb_module = NULL;
	}
}

/* ModMusic::onSeek
 * Called when seeking is requested on the sound stream
 *******************************************************************/
void ModMusic::onSeek(sf::Time timeOffset)
{
	long pos = static_cast<long>(timeOffset.asSeconds() * 65536);
	dumb_player = duh_start_sigrenderer(dumb_module, 0, 2, pos);
}

/* ModMusic::onGetData
 * Called when sound data is requested from the stream
 *******************************************************************/
bool ModMusic::onGetData(Chunk& data)
{
	duh_render(dumb_player, 16, 0, 1.0f, (65536.0f / 44100.0f), 44100 / 2, samples);
	data.samples = samples;
	data.sampleCount = 44100;
	return true;
}

/* ModMusic::initDumb
 * Initialises the DUMB library
 *******************************************************************/
void ModMusic::initDumb()
{
	dumb_register_stdfiles();
	init_done = true;
}
