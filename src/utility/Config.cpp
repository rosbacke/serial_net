/**
 * This file is part of SerialNet.
 *
 *  SerialNet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SerialNet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SerialNet.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Config.cpp
 *
 *  Created on: 27 aug. 2016
 *      Author: mikaelr
 */

#include "Config.h"

#include <fstream>
#include <libconfig.h++>

Config::Config()
{
}

Config::~Config()
{
}

Config&
Config::instance()
{
    static Config cfg;
    return cfg;
}

void
Config::readConfig(const std::string& cfgFilename)
{
    std::fstream cfgFile(cfgFilename);
    if (!cfgFile.is_open())
    {
        throw std::runtime_error("Can not open file.");
    }
}
