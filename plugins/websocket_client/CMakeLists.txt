
file (GLOB SOURCE_FILES *.cpp)
file (GLOB HEADER_FILES *.hpp)

init_target (websocket_client)

build_executable (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})

link_boost()
if(SDK_WEBSOCKET_TLS)
link_openssl()
endif()
final_target()

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "plugins")
