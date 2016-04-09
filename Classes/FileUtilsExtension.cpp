#pragma execution_character_set("utf-8")
//
//  FileDelete.cpp
//  ToBeHero
//
//  Created by darklinden on 11/7/14.
//
//

#include "FileUtilsExtension.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)

int FileUtilsExtension::path_is_directory(const std::string& path) {
#ifndef WIN32
	struct stat s_buf;

	if (stat(path.c_str(), &s_buf))
		return 0;

	return S_ISDIR(s_buf.st_mode);
#else
	DWORD dwAttrs = GetFileAttributes((LPCWSTR)path.c_str());
	if (dwAttrs == INVALID_FILE_ATTRIBUTES) return false;

	if (dwAttrs & FILE_ATTRIBUTE_DIRECTORY) {
		return 1;
	}

	return 0;
#endif
}

bool FileUtilsExtension::delete_folder_tree(const std::string& directory_name) {
#ifndef WIN32
    DIR*            dp;
    struct dirent*  ep;
    
    dp = opendir(directory_name.c_str());
    
    while ((ep = readdir(dp)) != NULL) {
        if (std::string(ep->d_name) == "." || std::string(ep->d_name) == "..") continue;
        
        std::string p_buf = directory_name + "/" + ep->d_name; //"%s/%s"
        if (path_is_directory(p_buf))
            delete_folder_tree(p_buf);
        else
            unlink(p_buf.c_str());
    }
    
    closedir(dp);
    return (0 == rmdir(directory_name.c_str()));
#else
	BOOL ret = RemoveDirectory((LPCWSTR)directory_name.c_str());
	if (!ret) {
		cocos2d::log("FileUtilsExtension::delete_folder_tree failed with error %ld", GetLastError());
	}

	return !!ret;
#endif
}

bool FileUtilsExtension::delete_file(const std::string& fileName) {
#ifndef WIN32
    std::string path = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);
    
    if (cocos2d::FileUtils::getInstance()->isFileExist(path))
    {
        if (FileUtilsExtension::path_is_directory(path)) {
            //is dir
            return FileUtilsExtension::delete_folder_tree(path);
        }
        else {
            //is file
            return (0 == remove(path.c_str()));
        }
    }
    else {
        return false;
    }
#else
	
	BOOL ret = DeleteFile((LPCWSTR)fileName.c_str());
	if (!ret) {
		cocos2d::log("FileUtilsExtension::delete_folder_tree failed with error %ld", GetLastError());
	}

	return !!ret;
#endif
};

#ifdef WIN32
int SearchDirectory(std::vector<std::string> &refvecFiles,
    const std::string        &refcstrRootDirectory,
    const std::string        &refcstrExtension,
    bool                     bSearchSubdirectories = true)
{
    std::string     strFilePath;             // Filepath
    std::string     strPattern;              // Pattern
    std::string     strExtension;            // Extension
    HANDLE          hFile;                   // Handle to file
    WIN32_FIND_DATAA FileInformation;         // File information


    strPattern = refcstrRootDirectory + "\\*.*";

    hFile = ::FindFirstFileA(strPattern.c_str(), &FileInformation);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (FileInformation.cFileName[0] != '.')
            {
                strFilePath.erase();
                strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

                if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (bSearchSubdirectories)
                    {
                        // Search subdirectory
                        int iRC = SearchDirectory(refvecFiles,
                            strFilePath,
                            refcstrExtension,
                            bSearchSubdirectories);
                        if (iRC)
                            return iRC;
                    }
                }
                else
                {
                    // Check extension
                    strExtension = FileInformation.cFileName;
                    strExtension = strExtension.substr(strExtension.rfind(".") + 1);

                    if (refcstrExtension.length())
                    {
                        // Save filename
                        if (strExtension == refcstrExtension)
                        {
                            refvecFiles.push_back(strFilePath);
                        }
                    }
                    else
                    {
                        refvecFiles.push_back(strFilePath);
                    }
                }
            }
        } while (::FindNextFileA(hFile, &FileInformation) == TRUE);

        // Close handle
        ::FindClose(hFile);

        DWORD dwError = ::GetLastError();
        if (dwError != ERROR_NO_MORE_FILES)
            return dwError;
    }

    return 0;
}

#endif

std::vector<std::string> FileUtilsExtension::content_of_folder(const std::string& path, const std::string& extension)
{
#ifndef WIN32
	std::vector<std::string> ret;
	ret.clear();

	if (!path_is_directory(path)) {
		return ret;
	}

    DIR*            dp;
    struct dirent*  ep;
    
    dp = opendir(path.c_str());
    
    while ((ep = readdir(dp)) != NULL) {
        if (std::string(ep->d_name) == "." || std::string(ep->d_name) == "..") continue;
        if (!extension.length() || extension == p.pathExtension.lowercaseString.UTF8String) {
            ret.push_back(std::string(ep->d_name));
        }
    }
    
    closedir(dp);
    return ret;
#else

    int                      iRC = 0;
    std::vector<std::string> vecFiles;

    // Search 'c:' for '.avi' files including subdirectories
    iRC = SearchDirectory(vecFiles, path, extension);
    if (iRC)
    {
        vecFiles.clear();
    }
    
    return vecFiles;

#endif
}

std::string FileUtilsExtension::file_name_for_path(const std::string& path)
{
    std::string ret = path;
    auto pos = path.rfind("\\");
    if (pos != path.npos)
    {
        ret = path.substr(pos, path.length() - pos);
    }
    return ret;
}

std::string FileUtilsExtension::string_by_delete_path_extension(const std::string& path)
{
    std::string ret = path;
    auto pos = path.rfind(".");
    if (pos != path.npos)
    {
        ret = path.substr(0, pos);
    }
    return ret;
}

std::string FileUtilsExtension::path_extension(const std::string& path)
{
    std::string ret = path;
    auto pos = path.rfind(".");
    if (pos != path.npos)
    {
        ret = path.substr(pos, path.length() - pos);
    }
    return ret;
}

void FileUtilsExtension::skipiCloudBackupForItemAtPath(const std::string& path)
{
}

std::string FileUtilsExtension::res_path()
{
    std::string ret = "";
    
#ifdef WIN32
    char ownPth[MAX_PATH];

    // Will contain exe path
    HMODULE hModule = GetModuleHandleA(NULL);
    if (hModule != NULL)
    {
        // When passing NULL to GetModuleHandle, it returns handle of exe itself
        GetModuleFileNameA(hModule, ownPth, (sizeof(ownPth)));
        CCLOG("AppDelegate::AppDelegate GetModuleFileNameA %s", ownPth);
        std::string path = ownPth;
        ret = path;

        auto pos = path.rfind("\\");
        if (pos != path.npos)
        {
            path = path.substr(0, pos);
            ret = path;
        }
    }
#endif

    return ret;
}
#endif