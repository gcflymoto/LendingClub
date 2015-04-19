INIT_BIICODE_BLOCK()

project (CppLoanMiner)
add_executable(CppLoanMiner)

include(biicode/boost/setup)

ADD_BII_TARGETS()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

bii_setup_boost(COMPONENTS system thread filesystem log REQUIRED)
find_package(Boost COMPONENTS system thread filesystem date_time log log_setup REQUIRED)

target_compile_options(${BII_BLOCK_TARGET} INTERFACE -DBOOST_LOG_DYN_LINK)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE  ${Boost_LIBRARIES})