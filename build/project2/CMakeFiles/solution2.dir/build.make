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

# Include any dependencies generated for this target.
include project2/CMakeFiles/solution2.dir/depend.make

# Include the progress variables for this target.
include project2/CMakeFiles/solution2.dir/progress.make

# Include the compile flags for this target's objects.
include project2/CMakeFiles/solution2.dir/flags.make

project2/CMakeFiles/solution2.dir/solution/solution2.cc.o: project2/CMakeFiles/solution2.dir/flags.make
project2/CMakeFiles/solution2.dir/solution/solution2.cc.o: ../project2/solution/solution2.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/harson/p2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object project2/CMakeFiles/solution2.dir/solution/solution2.cc.o"
	cd /home/harson/p2/build/project2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/solution2.dir/solution/solution2.cc.o -c /home/harson/p2/project2/solution/solution2.cc

project2/CMakeFiles/solution2.dir/solution/solution2.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/solution2.dir/solution/solution2.cc.i"
	cd /home/harson/p2/build/project2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harson/p2/project2/solution/solution2.cc > CMakeFiles/solution2.dir/solution/solution2.cc.i

project2/CMakeFiles/solution2.dir/solution/solution2.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/solution2.dir/solution/solution2.cc.s"
	cd /home/harson/p2/build/project2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harson/p2/project2/solution/solution2.cc -o CMakeFiles/solution2.dir/solution/solution2.cc.s

project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.requires:

.PHONY : project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.requires

project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.provides: project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.requires
	$(MAKE) -f project2/CMakeFiles/solution2.dir/build.make project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.provides.build
.PHONY : project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.provides

project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.provides.build: project2/CMakeFiles/solution2.dir/solution/solution2.cc.o


# Object files for target solution2
solution2_OBJECTS = \
"CMakeFiles/solution2.dir/solution/solution2.cc.o"

# External object files for target solution2
solution2_EXTERNAL_OBJECTS =

bin/solution2: project2/CMakeFiles/solution2.dir/solution/solution2.cc.o
bin/solution2: project2/CMakeFiles/solution2.dir/build.make
bin/solution2: lib/libboost.so
bin/solution2: lib/libjsoncpp.a
bin/solution2: project2/CMakeFiles/solution2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/harson/p2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/solution2"
	cd /home/harson/p2/build/project2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/solution2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
project2/CMakeFiles/solution2.dir/build: bin/solution2

.PHONY : project2/CMakeFiles/solution2.dir/build

project2/CMakeFiles/solution2.dir/requires: project2/CMakeFiles/solution2.dir/solution/solution2.cc.o.requires

.PHONY : project2/CMakeFiles/solution2.dir/requires

project2/CMakeFiles/solution2.dir/clean:
	cd /home/harson/p2/build/project2 && $(CMAKE_COMMAND) -P CMakeFiles/solution2.dir/cmake_clean.cmake
.PHONY : project2/CMakeFiles/solution2.dir/clean

project2/CMakeFiles/solution2.dir/depend:
	cd /home/harson/p2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/harson/p2 /home/harson/p2/project2 /home/harson/p2/build /home/harson/p2/build/project2 /home/harson/p2/build/project2/CMakeFiles/solution2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : project2/CMakeFiles/solution2.dir/depend

