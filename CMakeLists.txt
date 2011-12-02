#------------------------------------------------------------------------------
# CMakeLists.txt
# author Yury Yuryev <yu.yuryev.dev@gmail.com>, ...
# CMake input file.
# Usage:
# 1. Install cmake (see cmake.org)
# 2. Make build dir (f.e. ./build
# 3. Run cmake-gui
# 4. Select soource path (this is dir with root CMakeLists.txt)
# 5. Select build path
# 6. push config button (select VS 2010 project in dialog)
# 7. push generate button
# 8. start MVS and open sollution from build dir
# 9. build it (F7)
# 10. build INSTALL project
# 11. go to ./build/bin dir (this is installation dir) and run wxcam
#------------------------------------------------------------------------------
cmake_minimum_required ( VERSION 2.8 )
#------------------------------------------------------------------------------
# project name
project ( wxcam )
#------------------------------------------------------------------------------
# wxWidgets
find_package(wxWidgets COMPONENTS core base REQUIRED )
#------------------------------------------------------------------------------
# wxWidgets include
include( "${wxWidgets_USE_FILE}" )
#------------------------------------------------------------------------------
# set install dir
set ( CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} )
#------------------------------------------------------------------------------
# find multicam lib
find_library ( MULTICAM_LIB NAMES multicam PATHS ${PROJECT_SOURCE_DIR}/MultiCam )
message ( "STATUS MULTICAM_LIB = ${MULTICAM_LIB}" )
#------------------------------------------------------------------------------
# include dirs
include_directories (
  ./ )
#------------------------------------------------------------------------------
# add sources to compile
set ( CAM_SOURCES ${CAM_SOURCES}
  gui/about.cpp
  camera/camera.cpp
  gui/camview.cpp
  gui/frame.cpp
  stdwx.cpp
  worker.cpp
  wxMulticam.cpp )
#------------------------------------------------------------------------------
# add resoures to compile
#set ( CAM_SOURCES ${CAM_SOURCES}
#  xrd5.rc )
#------------------------------------------------------------------------------
# build gui
add_executable ( wxMulticam WIN32 ${CAM_SOURCES} )
#------------------------------------------------------------------------------
# link with libs
target_link_libraries( wxMulticam ${wxWidgets_LIBRARIES} ${MULTICAM_LIB} )
#------------------------------------------------------------------------------
# install program
install ( TARGETS wxMulticam DESTINATION bin )
install ( FILES MultiCam.dll DESTINATION bin )
#------------------------------------------------------------------------------