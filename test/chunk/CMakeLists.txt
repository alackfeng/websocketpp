
##  chunk
file (GLOB SOURCE_FILES chunk_test.cpp)

init_target (chunk_test)
build_test (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
link_boost ()
final_target ()
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "test")

##  bigfile
file (GLOB SOURCE_FILES bigfile_test.cpp)

init_target (bigfile_test)
build_test (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
link_boost ()
final_target ()
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "test")


## chunkmanager
file (GLOB SOURCE_FILES ckmanager_test.cpp)

init_target (ckmanager_test)
build_test (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
link_boost ()
final_target ()
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "test")
