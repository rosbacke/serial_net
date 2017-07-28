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
 * FolderManager.h
 *
 *  Created on: 26 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_CORE_FOLDERMANAGER_H_
#define SRC_CORE_FOLDERMANAGER_H_

#include "mains/serialnet/SNConfig.h"

#include <string>

class FolderManager
{
  public:
    FolderManager(SNConfig* snConfig);
    ~FolderManager();

    std::string rootPath(std::string configPath);
    void setupRootDir(std::string path);

    void addPtyLink(LocalAddress addr, std::string ptyPath);
    void removePtyLink(LocalAddress addr);

  private:
    std::string m_rootPath;
    SNConfig* m_cfg = nullptr;
};

#endif /* SRC_CORE_FOLDERMANAGER_H_ */
