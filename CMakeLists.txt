PROJECT(serialflex)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
INCLUDE(CheckIncludeFileCXX)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_DEBUG_POSTFIX d)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(./src)
IF (MSVC)
    ADD_COMPILE_OPTIONS(/D _CRT_SECURE_NO_WARNINGS)
ENDIF (MSVC)

# serialflex
SET(INCLUDE "include/serialflex/traits.h" "include/serialflex/serialize.h")
SOURCE_GROUP(include FILES ${INCLUDE})

# json
SET(INCLUDEJSON "include/serialflex/json/encoder.h" "include/serialflex/json/decoder.h")
SOURCE_GROUP("include\\json" FILES ${INCLUDEJSON})
SET(SRCJSON "src/json/encoder.cpp" "src/json/decoder.cpp")
SOURCE_GROUP("src\\json" FILES ${SRCJSON})

# third
SET(JSON "src/json/reader.h" "src/json/reader.cpp" "src/json/writer.h" "src/json/writer.cpp")
SOURCE_GROUP("src\\json" FILES ${JSON})

# xml
SET(INCLUDEXML "include/serialflex/xml/encoder.h" "include/serialflex/xml/decoder.h")
SOURCE_GROUP("include\\xml" FILES ${INCLUDEXML})
SET(SRCXML "src/xml/encoder.cpp" "src/xml/decoder.cpp")
SOURCE_GROUP("src\\xml" FILES ${SRCXML})

# third
SET(XML "src/xml/reader.h" "src/xml/reader.cpp" "src/xml/writer.h" "src/xml/writer.cpp")
SOURCE_GROUP("src\\xml" FILES ${XML})

# node
SET(NODE "src/node.h")
SOURCE_GROUP("src" FILES ${NODE})

IF (MSVC)
    ADD_LIBRARY(${PROJECT_NAME} SHARED
    ${INCLUDE}
    ${JSON}
    ${INCLUDEJSON} ${SRCJSON}
    ${XML}
    ${INCLUDEXML} ${SRCXML}
    ${NODE}
    )
ELSE ()
    ADD_LIBRARY(${PROJECT_NAME} 
    ${INCLUDE}
    ${JSON}
    ${INCLUDEJSON} ${SRCJSON}
    ${XML}
    ${INCLUDEXML} ${SRCXML}
    ${NODE}
    )
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
ENDIF (MSVC)


SET(EXAMPLE ON CACHE BOOL "")
if (${EXAMPLE} STREQUAL ON)
    SET(EXAMPLE "example/main.cpp")
    SOURCE_GROUP(example FILES ${EXAMPLE})
    ADD_EXECUTABLE(example ${EXAMPLE})
    TARGET_LINK_LIBRARIES(example ${PROJECT_NAME})
endif()
