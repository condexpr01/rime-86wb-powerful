
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 4.0)
project(table CXX)

# clang用的compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(core SHARED)
target_sources(core
	PRIVATE
		cppcore/core.impl.cpp

	PUBLIC
		cppcore/core.header.h
)

# CMAKE_INSTALL_PREFIX/bin
install(TARGETS core LIBRARY DESTINATION lib)

function(add_bin_tool name src)
	add_executable(${name})
	target_sources(${name} PRIVATE ${src})
	target_link_libraries(${name} PRIVATE core)
	set_target_properties(${name} PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")
	install(TARGETS ${name} RUNTIME DESTINATION bin)
endfunction()

add_bin_tool(cppt binsrc/cppt.cpp)

add_bin_tool(encoder binsrc/encoder.cpp)
add_bin_tool(encoder-with-filter binsrc/encoder-with-filter.cpp)

add_bin_tool(get-freq binsrc/get-freq.cpp)
add_bin_tool(get-freq-with-filter binsrc/get-freq-with-filter.cpp)

add_bin_tool(word-set-intersect binsrc/word-set-intersect.cpp)
add_bin_tool(word-set-unite binsrc/word-set-unite.cpp)
add_bin_tool(word-set-difference binsrc/word-set-difference.cpp)

add_bin_tool(pinyin-normalize binsrc/pinyin-normalize.cpp)

add_bin_tool(sort-freq binsrc/sort-freq.cpp)
add_bin_tool(sort-codec-seq binsrc/sort-codec-seq.cpp)
add_bin_tool(sort-codec-seq-wbfilter binsrc/sort-codec-seq-wbfilter.cpp)

add_bin_tool(make-seq-from-freq binsrc/make-seq-from-freq.cpp)
add_bin_tool(make-seq-from-freq-wbfilter binsrc/make-seq-from-freq-wbfilter.cpp)


add_bin_tool(codec-set-intersect binsrc/codec-set-intersect.cpp)
add_bin_tool(codec-set-unite binsrc/codec-set-unite.cpp)
add_bin_tool(codec-set-difference binsrc/codec-set-difference.cpp)



