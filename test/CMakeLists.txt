add_executable(module-tests
        oatpp-mongo/bson/BooleanTest.cpp
        oatpp-mongo/bson/BooleanTest.hpp
        oatpp-mongo/bson/FloatTest.cpp
        oatpp-mongo/bson/FloatTest.hpp
        oatpp-mongo/bson/Int8Test.cpp
        oatpp-mongo/bson/Int8Test.hpp
        oatpp-mongo/bson/Int16Test.cpp
        oatpp-mongo/bson/Int16Test.hpp
        oatpp-mongo/bson/Int32Test.cpp
        oatpp-mongo/bson/Int32Test.hpp
        oatpp-mongo/bson/Int64Test.cpp
        oatpp-mongo/bson/Int64Test.hpp
        oatpp-mongo/bson/ListTest.cpp
        oatpp-mongo/bson/ListTest.hpp
        oatpp-mongo/bson/MapTest.cpp
        oatpp-mongo/bson/MapTest.hpp
        oatpp-mongo/bson/ObjectTest.cpp
        oatpp-mongo/bson/ObjectTest.hpp
        oatpp-mongo/bson/StringTest.cpp
        oatpp-mongo/bson/StringTest.hpp
        oatpp-mongo/tests.cpp
)

set_target_properties(module-tests PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(module-tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(module-tests ${LIB_OATPP_EXTERNAL})
endif()

add_dependencies(module-tests ${OATPP_THIS_MODULE_NAME})

target_link_oatpp(module-tests)

target_link_libraries(module-tests
        PRIVATE ${OATPP_THIS_MODULE_NAME}
)

target_include_directories(module-tests
        PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS}
        )

target_link_libraries(module-tests
        PRIVATE ${LIBMONGOCXX_LIBRARIES}
        )

target_compile_definitions(module-tests
        PRIVATE ${LIBMONGOCXX_DEFINITIONS}
        )

add_test(module-tests module-tests)