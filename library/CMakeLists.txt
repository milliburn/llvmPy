set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.68.0) # TODO: XXX
find_package(Boost 1.65 REQUIRED)

add_subdirectory(Catch2)
add_subdirectory(FakeIt)
add_subdirectory(llvmPyLitParse)
