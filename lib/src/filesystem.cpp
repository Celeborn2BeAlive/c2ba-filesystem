#include <c2ba/filesystem/filesystem.hpp>

#include <fstream>

#ifdef __GNUC__

// Use Posix API
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#else

#ifdef _WIN32

#include <windows.h>
#include <windef.h>
#include <shlwapi.h>

#endif

#endif

namespace c2ba
{
namespace filesystem
{

// Copy a given file
void copy(const path& srcFilePath, const path& dstFilePath) {
    std::ifstream src(srcFilePath.c_str(), std::ios::binary);
    std::ofstream dst(dstFilePath.c_str(), std::ios::binary);

    if (!src.is_open()) {
        // LOG
        return;
    }

    dst << src.rdbuf();
}


#ifdef __GNUC__

class Directory {
public:
    Directory(const path& path) :
        m_Path(path), m_pDir(opendir(m_Path.c_str())) {
    }

    ~Directory() {
        if (m_pDir) {
            closedir(m_pDir);
        }
    }

    operator bool() const {
        return m_pDir;
    }

    const path& path() const {
        return m_Path;
    }

    std::vector<path> files() const {
        std::vector<path> container;

        rewinddir(m_pDir);
        struct dirent* entry = nullptr;
        while (nullptr != (entry = readdir(m_pDir))) {
            path file(entry->d_name);
            if (file != ".." && file != ".") {
                container.emplace_back(file);
            }
        }
        rewinddir(m_pDir);

        return container;
    }

private:
    path m_Path;
    DIR* m_pDir;
};

inline bool exists(const path& path) {
    struct stat s;
    return 0 == stat(path.c_str(), &s);
}

inline bool is_directory(const path& path) {
    struct stat s;
    return 0 == stat(path.c_str(), &s) && S_ISDIR(s.st_mode);
}

inline bool is_regular_file(const path& path) {
    struct stat s;
    return 0 == stat(path.c_str(), &s) && S_ISREG(s.st_mode);
}

inline void create_directory(const path& path) {
    mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
}

void for_each(const path& directoryPath, const std::function<bool(const path&)>& functor, bool relativePath) {

}

std::vector<path> directory_content(const path& directoryPath, bool relativePath) {

}

#else

#ifdef _WIN32

bool exists(const path& path) {
    return PathFileExists(path.c_str()) ? true : false;
}

bool is_directory(const path& path) {
    return PathIsDirectory(path.c_str()) ? true : false;
}

bool is_regular_file(const path& path) {
    return exists(path) && !is_directory(path);
}

void create_directory(const path& path) {
    CreateDirectory(path.c_str(), 0);
}

void for_each(const path& directoryPath, const std::function<void(const path&)>& functor, bool extractRelativePath) {
    HANDLE handle{ CreateFile(directoryPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0) };

    if (handle == INVALID_HANDLE_VALUE) {
        // LOG
        return;
    }

    std::string p = directoryPath.str() + "\\*";

    WIN32_FIND_DATA ffd;
    auto hFind = FindFirstFile(p.c_str(), &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
        // LOG
        return;
    }

    do {
        path file(ffd.cFileName);
        if (file != ".." && file != ".") {
            functor(extractRelativePath ? file : directoryPath / file);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    CloseHandle(handle);
}

std::vector<path> directory_content(const path& directoryPath, bool extractRelativePath) {
    std::vector<path> files;
    for_each(directoryPath, [&](const path& path) {
        files.emplace_back(path);
    }, extractRelativePath);
    return files;
}

#endif

#endif

}
}