
# Print build configuration
macro (print_used_build_config)
    message ("\n=========== Used Build Configuration =============\n")
    message (STATUS "ENABLE_CPP11        = " ${ENABLE_CPP11})
    message (STATUS "BUILD_EXAMPLES      = " ${BUILD_EXAMPLES})
    message (STATUS "BUILD_TESTS         = " ${BUILD_TESTS})
    message ("")
    message (STATUS "WEBSOCKETPP_ROOT    = " ${WEBSOCKETPP_ROOT})
    message (STATUS "WEBSOCKETPP_BIN     = " ${WEBSOCKETPP_BIN})
    message (STATUS "WEBSOCKETPP_LIB     = " ${WEBSOCKETPP_LIB})
    message (STATUS "Install prefix      = " ${CMAKE_INSTALL_PREFIX})
    message ("")
    message (STATUS "WEBSOCKETPP_BOOST_LIBS        = ${WEBSOCKETPP_BOOST_LIBS}")
    message (STATUS "WEBSOCKETPP_PLATFORM_LIBS     = ${WEBSOCKETPP_PLATFORM_LIBS}")
    message (STATUS "WEBSOCKETPP_PLATFORM_TLS_LIBS = ${WEBSOCKETPP_PLATFORM_TLS_LIBS}")
    message ("") 
    message (STATUS "OPENSSL_FOUND        = ${OPENSSL_FOUND}")
    message (STATUS "OPENSSL_INCLUDE_DIR     = ${OPENSSL_INCLUDE_DIR}")
    message (STATUS "OPENSSL_LIBRARIES = ${OPENSSL_LIBRARIES}")
    message (STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")
    message ("") 
    message (STATUS "ZLIB_FOUND        = ${ZLIB_FOUND}")
    message (STATUS "ZLIB_INCLUDE_DIR     = ${ZLIB_INCLUDE_DIR}")
    message (STATUS "ZLIB_LIBRARIES = ${ZLIB_LIBRARIES}")
    message ("") 
endmacro ()

# Adds the given folder_name into the source files of the current project. 
# Use this macro when your module contains .cpp and .h files in several subdirectories.
# Your sources variable needs to be WSPP_SOURCE_FILES and headers variable WSPP_HEADER_FILES.
macro(add_source_folder folder_name)
    file(GLOB H_FILES_IN_FOLDER_${folder_name} 
        ${WEBSOCKETPP_ROOT}/plugins/${folder_name}/*.hpp  
        ${WEBSOCKETPP_ROOT}/plugins/${folder_name}/*.h 
        ${WEBSOCKETPP_ROOT}/cores/${folder_name}/*.hpp  
        ${WEBSOCKETPP_ROOT}/cores/${folder_name}/*.h
        ${WEBSOCKETPP_ROOT}/chunk/${folder_name}/*.hpp  
        ${WEBSOCKETPP_ROOT}/chunk/${folder_name}/*.h
        ${WEBSOCKETPP_ROOT}/utils/${folder_name}/*.hpp  
        ${WEBSOCKETPP_ROOT}/utils/${folder_name}/*.h
    )
    file(GLOB CPP_FILES_IN_FOLDER_${folder_name} 
        ${WEBSOCKETPP_ROOT}/plugins/${folder_name}/*.cpp  
        ${WEBSOCKETPP_ROOT}/plugins/${folder_name}/*.c
        ${WEBSOCKETPP_ROOT}/cores/${folder_name}/*.cpp  
        ${WEBSOCKETPP_ROOT}/cores/${folder_name}/*.c
        ${WEBSOCKETPP_ROOT}/chunk/${folder_name}/*.cpp  
        ${WEBSOCKETPP_ROOT}/chunk/${folder_name}/*.c
        ${WEBSOCKETPP_ROOT}/utils/${folder_name}/*.cpp  
        ${WEBSOCKETPP_ROOT}/utils/${folder_name}/*.c
    )
    source_group("Header Files\\${folder_name}" FILES ${H_FILES_IN_FOLDER_${folder_name}})
    source_group("Source Files\\${folder_name}" FILES ${CPP_FILES_IN_FOLDER_${folder_name}})
    set(WSPP_HEADER_FILES ${WSPP_HEADER_FILES} ${H_FILES_IN_FOLDER_${folder_name}})
    set(WSPP_SOURCE_FILES ${WSPP_SOURCE_FILES} ${CPP_FILES_IN_FOLDER_${folder_name}})
#    message (STATUS "       message source: " ${WSPP_SOURCE_FILES})
endmacro()

# Initialize target.
macro (init_target NAME)
    set (TARGET_NAME ${NAME})
    message ("** " ${TARGET_NAME})

    # Include our own module path. This makes #include "x.h" 
    # work in project subfolders to include the main directory headers.
    message (STATUS "init_target "  ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories (${CMAKE_CURRENT_SOURCE_DIR})
endmacro ()

# Build executable for executables
macro (build_executable TARGET_NAME)
    set (TARGET_LIB_TYPE "EXECUTABLE")
    message (STATUS "-- Build Type:")
    message (STATUS "       " ${TARGET_LIB_TYPE})
    
#    file(GLOB PLUGINS_LIB_SOURCE ${WEBSOCKETPP_ROOT}/plugins/base/*.cpp)
#    message (STATUS "       include hpp: " ${WSPP_HEADER_FILES})
#    message (STATUS "       source  cpp: " ${WSPP_SOURCE_FILES})

    ### add build link source
    add_source_folder(base)
    add_source_folder(utils)
    add_source_folder(chunk)
    add_executable (${TARGET_NAME} ${ARGN} ${WSPP_SOURCE_FILES} ${WSPP_SOURCE_FILES})

    include_directories (${WEBSOCKETPP_ROOT} ${WEBSOCKETPP_INCLUDE} ${WSPP_HEADER_FILES} )

    target_link_libraries(${TARGET_NAME} ${WEBSOCKETPP_PLATFORM_LIBS})

    set_target_properties (${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${WEBSOCKETPP_BIN})
    set_target_properties (${TARGET_NAME} PROPERTIES DEBUG_POSTFIX d)
endmacro ()

# Build library for librarys
macro (build_library TARGET_NAME)
    set (TARGET_LIB_TYPE "STATIC_LIBRARY")
    message (STATUS "-- Build Type:")
    message (STATUS "       " ${TARGET_LIB_TYPE})
    add_library (${TARGET_NAME} STATIC ${ARGN})

    include_directories (${WEBSOCKETPP_ROOT} ${WEBSOCKETPP_INCLUDE})

    target_link_libraries(${TARGET_NAME} ${WEBSOCKETPP_PLATFORM_LIBS})

    set_target_properties (${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${WEBSOCKETPP_LIB})
    set_target_properties (${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${WEBSOCKETPP_LIB})
    set_target_properties (${TARGET_NAME} PROPERTIES DEBUG_POSTFIX d)
endmacro ()


# Build executable and register as test
macro (build_test TARGET_NAME)
    build_executable (${TARGET_NAME} ${ARGN})

    if (${CMAKE_VERSION} VERSION_LESS 3)
        message(WARNING "CMake too old to register ${TARGET_NAME} as a test")
    else ()
        add_test(NAME ${TARGET_NAME} COMMAND $<TARGET_FILE:${TARGET_NAME}>)
    endif ()
endmacro ()

# Finalize target for all types
macro (final_target)
    if ("${TARGET_LIB_TYPE}" STREQUAL "EXECUTABLE")
        install (TARGETS ${TARGET_NAME} 
                 RUNTIME DESTINATION "bin" 
                 CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
    endif ()

    # install headers, directly from current source dir and look for subfolders with headers
    file (GLOB_RECURSE TARGET_INSTALL_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)
    foreach (hppfile ${TARGET_INSTALL_HEADERS})
      get_filename_component (currdir ${hppfile} PATH)
      install (FILES ${hppfile} DESTINATION "include/${TARGET_NAME}/${currdir}")
    endforeach()
endmacro ()

macro (link_sdklib)
    target_link_libraries (${TARGET_NAME} "chunk_manager")
endmacro ()

macro (link_boost)
    target_link_libraries (${TARGET_NAME} ${Boost_LIBRARIES})
endmacro ()

macro (link_openssl)
    # if(MSVC)
        include_directories (${OPENSSL_INCLUDE_DIR})
    # endif()
    target_link_libraries (${TARGET_NAME} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
endmacro ()

macro (link_zlib)
    if(MSVC)
        include_directories (${ZLIB_INCLUDE_DIR})
    endif()
	target_link_libraries (${TARGET_NAME} ${ZLIB_LIBRARIES})
endmacro ()

macro (include_subdirs PARENT)
    file (GLOB SDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PARENT}/*")
    foreach (SUBDIR ${SDIRS})
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/CMakeLists.txt")
            add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}")
        endif ()
    endforeach ()
endmacro()
