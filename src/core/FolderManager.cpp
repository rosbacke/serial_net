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
 * FolderManager.cpp
 *
 *  Created on: 26 juli 2017
 *      Author: mikaelr
 */

#include "FolderManager.h"
#include "mains/serialnet/SNConfig.h"

#include "utility/Log.h"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/stat.h>

FolderManager::FolderManager(SNConfig* snConfig) : m_cfg(snConfig)
{
    m_rootPath = rootPath(snConfig ? snConfig->m_rootPath : std::string());
    setupRootDir(m_rootPath);
}

FolderManager::~FolderManager()
{
    // TODO Auto-generated destructor stub
}

void
FolderManager::setupRootDir(std::string path)
{
    struct stat buf;
    ::memset(&buf, 0, sizeof buf);

    int res = ::stat(path.c_str(), &buf);
    if (res == -1)
    {
        if (errno == ENOENT)
        {
            LOG_INFO << "Missing root path, creating it at:" << path;
            int res = ::mkdir(path.c_str(), 0777);
            if (res == -1)
                throw std::system_error(errno, std::system_category());
        }
        else
        {
            throw std::system_error(errno, std::system_category());
        }
    }
}

std::string
FolderManager::rootPath(std::string configPath)
{
    std::string path;
    if (configPath.substr(0, 2) == "~/")
    {
        const char* homedir = ::getenv("HOME");
        if (homedir == NULL)
        {
            homedir = ::getpwuid(::getuid())->pw_dir;
        }
        if (homedir == NULL)
        {
            throw std::runtime_error(
                "Failed to read home dir when setting up root dir.");
        }
        path = std::string(homedir) +
               std::string(configPath.begin() + 1, configPath.end());
    }
    else
    {
        path = configPath;
    }
    return path;
}

void
FolderManager::addPtyLink(LocalAddress addr, std::string ptyPath)
{
    std::stringstream path;
    path << m_rootPath << "/pty/" << addr;
    std::string ptyDirPath = m_rootPath + "/pty";
    struct stat buf;

    int res = ::stat(ptyDirPath.c_str(), &buf);
    if (res == -1)
    {
        if (errno == ENOENT)
        {
            LOG_INFO << "Missing pty path, creating it at:" << ptyDirPath;
            int res = ::mkdir(ptyDirPath.c_str(), 0777);
            if (res == -1)
                throw std::system_error(errno, std::system_category());
        }
    }

    res = ::stat(path.str().c_str(), &buf);
    if (res != -1)
    {
        res = ::unlink(path.str().c_str());
        if (res == -1)
            throw std::system_error(errno, std::system_category());
    }

    res = ::symlink(ptyPath.c_str(), path.str().c_str());
    if (res == -1)
        throw std::system_error(errno, std::system_category());
}

void
FolderManager::removePtyLink(LocalAddress addr)
{
    std::stringstream path;
    path << m_rootPath << "/pty/" << addr;

    int res = ::unlink(path.str().c_str());
    if (res == -1)
        throw std::system_error(errno, std::system_category());
}
