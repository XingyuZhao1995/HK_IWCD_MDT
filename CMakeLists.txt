# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required(VERSION 3.1)


set(PROJECT_NAME MDT)

list(APPEND CMAKE_MODULE_PATH
        $ENV{HK_PILOT_DIR}/cmake
        )
include(GetVersionGit)
MESSAGE("Preparing project ${PROJECT_NAME} version ${PROJECT_VERSION}" )
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})
include(HKPackageBuilder)
hkbuilder_prepare_project()
set( PUBLIC_EXT_LIBS )
hk_check_dependencies()

# Additional lines to overwrite default WCSim in container with own installed one
get_directory_property(_incs INCLUDE_DIRECTORIES)
list(FILTER _incs EXCLUDE REGEX "/include/WCSim/?$")
set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "${_incs}")

add_definitions(-DHYBRIDWCSIM)
# manually include WCSim libraries
list( APPEND PUBLIC_EXT_LIBS
        WCSim::WCSimRoot
        WCSim::WCSimCore
)

include_directories(BEFORE
        ${PROJECT_SOURCE_DIR}/cpp/include
        ${PROJECT_SOURCE_DIR}/app/utilities/include

)
add_subdirectory(cpp)
add_subdirectory(app)
