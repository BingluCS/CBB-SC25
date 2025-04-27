set(HDF5_LIBRARIES "$ENV{HDF5}/lib/libhdf5.so")
set(HDF5_INCLUDE_DIRS "$ENV{HDF5}/include")

message("HDF5LIB PATH:" ${HDF5_LIBRARIES})
message("HDF5INCLUDE PATH:" ${HDF5_INCLUDE_DIRS})
#find_package(HDF5 REQUIRED)
add_library(
  hdf5sz
  src/H5Z_SZ.cpp
  )
target_compile_options(hdf5sz PRIVATE -fpermissive)
target_link_libraries(
  hdf5sz
  PUBLIC SZ ${HDF5_LIBRARIES}
  )
target_include_directories(
  hdf5sz
  PUBLIC
  ${HDF5_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:hdf5_sz>
  )

if(BUILD_TESTS)
  add_subdirectory(test)
endif()

install(TARGETS hdf5sz EXPORT HDF5SZ
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
export(TARGETS hdf5sz FILE HDF5SZ.cmake)
