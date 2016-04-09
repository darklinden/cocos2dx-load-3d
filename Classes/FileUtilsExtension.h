#pragma execution_character_set("utf-8")
//
//  FileDelete.h
//  ToBeHero
//
//  Created by darklinden on 11/7/14.
//
//

#ifndef __ToBeHero__FileDelete__
#define __ToBeHero__FileDelete__

#include "cocos2d.h"
#include <stdio.h>
#include <sys/stat.h>

#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
#include <unistd.h>
#include <dirent.h>
#else
/** This file is part of the Mingw32 package. * unistd.h maps (roughly) to io.h */ 
#ifndef _UNISTD_H 
#define _UNISTD_H 
#include <io.h> 
#include <process.h> 
#endif /* _UNISTD_H */ 
#endif


#include <vector>

class FileUtilsExtension {
    
public:
    
    static bool delete_file(const std::string& fileName);
    
    static int path_is_directory(const std::string& path);
    
    static bool delete_folder_tree(const std::string& directory_name);
    
    static std::vector<std::string> content_of_folder(const std::string& path, const std::string& extension = "");
    
    static std::string file_name_for_path(const std::string& path);
    
    static std::string string_by_delete_path_extension(const std::string& path);
    
    static std::string path_extension(const std::string& path);
    
    static void skipiCloudBackupForItemAtPath(const std::string& path);

    static std::string res_path();
};

#endif /* defined(__ToBeHero__FileDelete__) */
