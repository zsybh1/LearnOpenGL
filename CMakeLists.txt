cmake_minimum_required(VERSION 3.14)

project(LearnOpenGL)

file(GLOB_RECURSE myFiles ./src/*.*)
foreach(myFile ${myFiles}) 
    string(REGEX REPLACE ".+/(src/.+\\..*)" "\\1" fileName ${myFile})
	list(APPEND fileList ${fileName})
endforeach(myFile)
list(REMOVE_ITEM fileList "src/main.cpp")

find_package(Eigen3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})


function(configure_executable_target target_name addition_file)
add_executable(${target_name} ${addition_file} ${fileList})
target_include_directories(${target_name} 
	PRIVATE 
		${PROJECT_SOURCE_DIR}/include 
		${PROJECT_SOURCE_DIR}/extern/OBJ_Loader/Source
)
target_link_libraries(${target_name} 
	PRIVATE 
		Eigen3::Eigen
		glad::glad
		glfw
		imgui::imgui
)
endfunction()

configure_executable_target(main "src/main.cpp")
configure_executable_target(example "example/example.cpp")
configure_executable_target(blending_transparent "example/Blending/transparent.cpp")
configure_executable_target(framebuffer_post_processing "example/FrameBuffer/post_processing.cpp")

set(destAssets ${EXECUTABLE_OUTPUT_PATH}/assets)
file(GLOB assets LIST_DIRECTORIES true ${destAssets})
if(NOT assets)
	set(sourceAssets ${PROJECT_SOURCE_DIR}/assets)
	string(REPLACE "/" "\\" sourceAssets ${sourceAssets})
	string(REPLACE "/" "\\" destAssets ${destAssets})

	execute_process(
		COMMAND cmd /C mklink /J ${destAssets} ${sourceAssets}
	)
endif()