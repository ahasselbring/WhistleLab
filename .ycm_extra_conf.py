import os
import ycm_core

def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))

def GetDatabase():
    compilation_database_folder = os.path.join(DirectoryOfThisScript(), 'Build')
    if os.path.exists(compilation_database_folder):
        return ycm_core.CompilationDatabase(compilation_database_folder)
    return None


def IsHeaderFile(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.hpp']


def GetCompilationInfoForFile(filename):
    database = GetDatabase()
    if not database:
        return None
    if IsHeaderFile(filename):
        basename = os.path.splitext(filename)[0]
        cpp_file = basename + '.cpp'
        # for pure headers (no cpp file), default to Main.cpp
        if not os.path.exists(cpp_file):
            cpp_file = os.path.join(DirectoryOfThisScript(), 'Source', 'Main.cpp')
            if not os.path.exists(cpp_file):
                return None
        compilation_info = database.GetCompilationInfoForFile(cpp_file)
        if compilation_info.compiler_flags_:
            return compilation_info
        return None
    return database.GetCompilationInfoForFile(filename)


def FlagsForFile(filename, **kwargs):
    compilation_info = GetCompilationInfoForFile(filename)
    if not compilation_info:
        return None
    # Add flags not needed for clang-the-binary,
    # but needed for libclang-the-library (YCM uses this last one).
    flags = (list(compilation_info.compiler_flags_)
             if compilation_info.compiler_flags_
             else [])
    return {
        'flags': flags,
        'do_cache': True
    }
