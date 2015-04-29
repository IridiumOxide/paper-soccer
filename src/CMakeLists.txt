file(GLOB_RECURSE PILKA_SOURCES ./*.cpp)
file(GLOB_RECURSE PILKA_HEADERS ./*.h)

add_executable(pilka ${PILKA_SOURCES} ${PILKA_HEADERS})
target_link_libraries(pilka Qt5::Core Qt5::Widgets)

install(TARGETS pilka RUNTIME DESTINATION bin)
