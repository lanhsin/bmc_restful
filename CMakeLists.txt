cmake_minimum_required(VERSION 3.5)

project(go-server)

set(GO /usr/bin/go)
set(GOPATH $ENV{HOME}/go)
set(GOBIN ${GOPATH}/bin)

message(STATUS "GO=${GO}")
message(STATUS "GOPATH=${GOPATH}")
message(STATUS "GOBIN=${GOBIN}")

add_subdirectory(libcgocommon)

add_custom_target( 
    go-server ALL
    COMMAND cd ${GOPATH}/src/go-server &&
    env CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER}
    ${GO} install -v &&  ${GO} build -v -x -o $@
    DEPENDS main.go
)

add_custom_command(
    OUTPUT main.go
    COMMAND mkdir -p ${GOPATH}/lib
    COMMAND cp  libcgocommon/libcgocommon.a ${GOPATH}/lib
    COMMAND mkdir -p  ${GOPATH}/include
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/libcgocommon/include ${GOPATH}/
    COMMAND mkdir -p ${GOPATH}/src
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/go-server ${GOPATH}/src/
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/cgocommon ${GOPATH}/src/
    DEPENDS libcgocommon/cgocommon
)

set_property(
  TARGET go-server
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES  ${GOPATH}
)
