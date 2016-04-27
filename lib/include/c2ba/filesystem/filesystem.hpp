#pragma once

#include <functional>
#include <string>
#include <ostream>
#include <vector>

namespace c2ba
{
namespace filesystem
{

// A class to represent a path in the filesystem
// @todo: match http://en.cppreference.com/w/cpp/experimental/fs/path API
class path {
public:
#ifdef _WIN32
    static const char PATH_SEPARATOR = '\\';
#else
    static const char PATH_SEPARATOR = '/';
#endif
    path() = default;

    explicit path(const char* filepath) : m_FilePath(filepath) {
        format();
    }

    explicit path(const std::string& filepath) : m_FilePath(filepath) {
        format();
    }

    const std::string& str() const { return m_FilePath; }

    const char* c_str() const { return m_FilePath.c_str(); }

    bool empty() const {
        return m_FilePath.empty();
    }

    /*! returns the containing directory path of a file */
    path parent_path() const {
        size_t pos = m_FilePath.find_last_of(PATH_SEPARATOR);
        if (pos == std::string::npos) { return path(); }
        return path{ m_FilePath.substr(0, pos) };
    }

    /*! returns the file name of a filepath  */
    std::string filename() const {
        size_t pos = m_FilePath.find_last_of(PATH_SEPARATOR);
        if (pos == std::string::npos) { return m_FilePath; }
        return m_FilePath.substr(pos + 1);
    }

    /*! returns the file extension */
    std::string extension() const {
        size_t pos = m_FilePath.find_last_of('.');
        if (pos == std::string::npos || pos == 0) { return ""; }
        return m_FilePath.substr(pos);
    }

    // Returns true if the file path ends with ext
    bool ends_with(const std::string& ext) const {
        return m_FilePath.size() >= ext.size() && m_FilePath.substr(m_FilePath.size() - ext.size(), ext.size()) == ext;
    }

    /*! adds file extension */
    path add_extension(const std::string& ext) const {
        return path(m_FilePath + "." + ext);
    }

    // Concatenate other to this file path
    path& operator /=(const path& other)
    {
        if (m_FilePath.empty()) {
            *this = other;
        }
        else if (!other.empty()) {
            if (other.m_FilePath.front() != PATH_SEPARATOR) {
                m_FilePath += PATH_SEPARATOR;
            }
            m_FilePath += other.m_FilePath;
        }
        return *this;
    }

    path& operator +=(const path& other)
    {
        m_FilePath += other.m_FilePath;
        return *this;
    }

    bool operator ==(const path& other) const {
        return other.m_FilePath == m_FilePath;
    }

    bool operator ==(const std::string& other) const {
        return other == m_FilePath;
    }

    bool operator ==(const char* other) const {
        return path{ other } == m_FilePath;
    }

    template<typename T>
    friend bool operator !=(const path& lhs, T&& rhs) {
        return !(lhs == std::forward<T>(rhs));
    }

    template<typename T>
    friend bool operator !=(T&& lhs, const path& rhs) {
        return rhs != lhs;
    }

    /*! output operator */
    friend std::ostream& operator<<(std::ostream& cout, const path& filepath) {
        return (cout << filepath.m_FilePath);
    }

private:
    void format() {
        for (auto& c : m_FilePath) {
            if (c == '\\' || c == '/') {
                c = PATH_SEPARATOR;
            }
        }
        while (!m_FilePath.empty() && m_FilePath.back() == PATH_SEPARATOR) {
            m_FilePath.pop_back();
        }
    }

    std::string m_FilePath;
};

inline path operator /(path lhs, const path& rhs) {
    return (lhs /= rhs);
}

inline path operator +(path lhs, const path& rhs) {
    return (lhs += rhs);
}

// Copy a given file
void copy(const path& srcFilePath, const path& dstFilePath);

// Return true if a file pointed by path exists
bool exists(const path& p);

// Return true if the file pointed by path is a directory
bool is_directory(const path& p);

// Return true if the file pointed by path is regular
bool is_regular_file(const path& p);

// Create a directory. If it already exists, nothing is done.
void create_directory(const path& p);

// Iterate over the files contained in the directory pointed by directoryPath, calling functor for each one
// The path given to functor is relative to directoryPath if extractRelativePath = true. Otherwise, the path given to functor is directoryPath + path of file
// If functor returns false, the function returns
void for_each(const path& directoryPath, const std::function<void(const path&)>& functor, bool extractRelativePath = true);

std::vector<path> directory_content(const path& directoryPath, bool extractRelativePath = true);

}

namespace fs = filesystem;

}