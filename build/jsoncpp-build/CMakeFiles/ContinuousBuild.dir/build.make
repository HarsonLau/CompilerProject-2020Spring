# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/harson/p2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/harson/p2/build

# Utility rule file for ContinuousBuild.

# Include the progress variables for this target.
include jsoncpp-build/CMakeFiles/ContinuousBuild.dir/progress.make

jsoncpp-build/CMakeFiles/ContinuousBuild:
	cd /home/harson/p2/build/jsoncpp-build && /usr/bin/ctest -D ContinuousBuild

ContinuousBuild: jsoncpp-build/CMakeFiles/ContinuousBuild
ContinuousBuild: jsoncpp-build/CMakeFiles/ContinuousBuild.dir/build.make

.PHONY : ContinuousBuild

# Rule to build all files generated by this target.
jsoncpp-build/CMakeFiles/ContinuousBuild.dir/build: ContinuousBuild

.PHONY : jsoncpp-build/CMakeFiles/ContinuousBuild.dir/build

jsoncpp-build/CMakeFiles/ContinuousBuild.dir/clean:
	cd /home/harson/p2/build/jsoncpp-build && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousBuild.dir/cmake_clean.cmake
.PHONY : jsoncpp-build/CMakeFiles/ContinuousBuild.dir/clean

jsoncpp-build/CMakeFiles/ContinuousBuild.dir/depend:
	cd /home/harson/p2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/harson/p2 /home/harson/p2/3rdparty/jsoncpp /home/harson/p2/build /home/harson/p2/build/jsoncpp-build /home/harson/p2/build/jsoncpp-build/CMakeFiles/ContinuousBuild.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : jsoncpp-build/CMakeFiles/ContinuousBuild.dir/depend

