# Build icedelivery example
cmake_minimum_required(VERSION 3.5)
project(example_icedelivery)

include(GNUInstallDirs)

find_package(iceoryx_posh CONFIG REQUIRED)

get_target_property(ICEORYX_CXX_STANDARD iceoryx_posh::iceoryx_posh CXX_STANDARD)
if ( NOT ICEORYX_CXX_STANDARD )
    include(IceoryxPlatformDetection)
endif ( NOT ICEORYX_CXX_STANDARD )

# ========================= Deprecated ========================= //

add_executable(iox-ex-publisher-bare-metal ./ice_publisher_bare_metal.cpp)
target_link_libraries(iox-ex-publisher-bare-metal
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-publisher-bare-metal PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-subscriber-bare-metal ./ice_subscriber_bare_metal.cpp)
target_link_libraries(iox-ex-subscriber-bare-metal
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-subscriber-bare-metal PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-publisher-simple ./ice_publisher_simple.cpp)
target_link_libraries(iox-ex-publisher-simple
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-publisher-simple PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-subscriber-simple ./ice_subscriber_simple.cpp)
target_link_libraries(iox-ex-subscriber-simple
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-subscriber-simple PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

# ========================= Modern ========================= //

add_executable(iox-ex-publisher-typed-modern ./iox_publisher_typed_modern.cpp)
target_link_libraries(iox-ex-publisher-typed-modern
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-publisher-typed-modern PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-publisher-untyped-modern ./iox_publisher_untyped_modern.cpp)
target_link_libraries(iox-ex-publisher-untyped-modern
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-publisher-untyped-modern PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-subscriber-typed-modern ./iox_subscriber_typed_modern.cpp)
target_link_libraries(iox-ex-subscriber-typed-modern
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-subscriber-typed-modern PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

add_executable(iox-ex-subscriber-untyped-modern ./iox_subscriber_untyped_modern.cpp)
target_link_libraries(iox-ex-subscriber-untyped-modern
    iceoryx_posh::iceoryx_posh
)
set_target_properties(iox-ex-subscriber-untyped-modern PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD ${ICEORYX_CXX_STANDARD}
    POSITION_INDEPENDENT_CODE ON
)

# ========================================================== //

install(
    TARGETS iox-ex-publisher-bare-metal iox-ex-subscriber-bare-metal iox-ex-publisher-simple iox-ex-subscriber-simple
    RUNTIME DESTINATION bin
)
