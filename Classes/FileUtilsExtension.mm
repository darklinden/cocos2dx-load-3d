//
//  FileDelete.cpp
//  ToBeHero
//
//  Created by darklinden on 11/7/14.
//
//

#include "FileUtilsExtension.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

int FileUtilsExtension::path_is_directory(const std::string& path) {
    BOOL isDirectory = false;
    BOOL exist = [[NSFileManager defaultManager]
                  fileExistsAtPath:[NSString stringWithUTF8String:path.c_str()]
                  isDirectory:&isDirectory];
    return isDirectory && exist;
}

bool FileUtilsExtension::delete_folder_tree(const std::string& directory_name) {
    return !![[NSFileManager defaultManager]
              removeItemAtPath:[NSString stringWithUTF8String:directory_name.c_str()]
              error:nil];
}

bool FileUtilsExtension::delete_file(const std::string& fileName) {
    return !![[NSFileManager defaultManager]
              removeItemAtPath:[NSString stringWithUTF8String:fileName.c_str()]
              error:nil];
};

std::vector<std::string> FileUtilsExtension::content_of_folder(const std::string& path, const std::string& extension)
{
    NSArray* path_list = [[NSFileManager defaultManager] subpathsAtPath:[NSString stringWithUTF8String:path.c_str()]];
    
    std::vector<std::string> ret;
    for (NSString* p in path_list) {
        if (!extension.length() || extension == p.pathExtension.lowercaseString.UTF8String) {
            ret.push_back(p.UTF8String);
        }
    }
    return ret;
}

std::string FileUtilsExtension::file_name_for_path(const std::string& path)
{
    return [[[NSString stringWithUTF8String:path.c_str()] lastPathComponent] UTF8String];
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
//    return [[[NSString stringWithUTF8String:path.c_str()] stringByDeletingPathExtension] UTF8String];
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
//    return [[[NSString stringWithUTF8String:path.c_str()] pathExtension] UTF8String];
}

void FileUtilsExtension::skipiCloudBackupForItemAtPath(const std::string& path)
{
    NSString* filePath = [NSString stringWithUTF8String:path.c_str()];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        return;
    }
    
    NSURL* URL = [NSURL fileURLWithPath:filePath];
    if (URL) {
        [URL setResourceValue:[NSNumber numberWithBool:YES]
                       forKey:NSURLIsExcludedFromBackupKey
                        error:nil];
    }
}

std::string FileUtilsExtension::res_path()
{
    std::string ret = "";
    
    return ret;
}

#endif