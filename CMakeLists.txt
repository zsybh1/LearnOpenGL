cmake_minimum_required(VERSION 3.14)

project(LearnOpenGL)

file(GLOB_RECURSE myFiles ${PROJECT_SOURCE_DIR}/include/*.*)
foreach(myFile ${myFiles}) 
    string(REGEX REPLACE ".+/(include/.+\\..*)" "\\1" fileName ${myFile})
	list(APPEND fileList ${fileName})
endforeach(myFile)

file(GLOB_RECURSE myFiles ./src/*.*)
foreach(myFile ${myFiles}) 
    string(REGEX REPLACE ".+/(src/.+\\..*)" "\\1" fileName ${myFile})
	list(APPEND fileList ${fileName})
endforeach(myFile)

find_package(Eigen3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


add_executable(main ${fileList})
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/extern/OBJ_Loader/Source)
target_link_libraries(main 
	PRIVATE 
		Eigen3::Eigen
		glad::glad
		glfw
		imgui::imgui
)

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