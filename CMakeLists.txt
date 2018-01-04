cmake_minimum_required(VERSION 2.8)

project( mpr3dbug )
#Seta os paths da VTK, ITK e SDL2
set(ITK_DIR "C:/itk_nova/ITK_build")
#set(VTK_DIR "C:/VTK-7.0.0-build")
set(VTK_DIR "C:/VTK-7.0.0-build-opengl1")
# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
# Find VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
#sdl
set(SDL2_INCLUDE_DIR "C:/Program Files (x86)/SDL2/include")
set(SDL2_LIBRARY "C:/Program Files (x86)/SDL2/lib/SDL2.lib")
include_directories(${SDL2_INCLUDE_DIR})
# Find SDL

file(GLOB my_source_files "*.h" "*.cpp" "*.cxx")
add_executable(teste_mpr ${my_source_files} )     

target_link_libraries(teste_mpr ${ITK_LIBRARIES} ${VTK_LIBRARIES}  ${SDL2_LIBRARY})

#boost.org
SET (BOOST_INCLUDE_DIR "C:/boost_1_64_0")
include_directories(${BOOST_INCLUDE_DIR})
target_link_libraries(teste_mpr "C:/boost_1_64_0/stage/win32/lib/libboost_signals-vc120-mt-1_64.lib")
target_link_libraries(teste_mpr "C:/boost_1_64_0/stage/win32/lib/libboost_exception-vc120-mt-gd-1_64.lib")
target_link_libraries(teste_mpr "C:/boost_1_64_0/stage/win32/lib/libboost_locale-vc120-mt-1_64.lib")

#sdl_ttf
#include_directories("C:/libs/SDL2_ttf-2.0.14/include")
target_link_libraries(teste_mpr "C:/libs/SDL2_ttf-2.0.14/lib/x86/SDL2_ttf.lib")

