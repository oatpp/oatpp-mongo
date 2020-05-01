
add_library(${OATPP_THIS_MODULE_NAME}
        oatpp-mongo/bson/mapping/Serializer.cpp
        oatpp-mongo/bson/mapping/Serializer.hpp
        oatpp-mongo/bson/mapping/Deserializer.cpp
        oatpp-mongo/bson/mapping/Deserializer.hpp
        oatpp-mongo/bson/mapping/ObjectMapper.cpp
        oatpp-mongo/bson/mapping/ObjectMapper.hpp
        oatpp-mongo/bson/type/ObjectId.cpp
        oatpp-mongo/bson/type/ObjectId.hpp
        oatpp-mongo/bson/Utils.cpp
        oatpp-mongo/bson/Utils.hpp
        oatpp-mongo/bson/Types.cpp
        oatpp-mongo/bson/Types.hpp
        oatpp-mongo/driver/command/Command.hpp
        oatpp-mongo/driver/command/Delete.cpp
        oatpp-mongo/driver/command/Delete.hpp
        oatpp-mongo/driver/command/Find.cpp
        oatpp-mongo/driver/command/Find.hpp
        oatpp-mongo/driver/command/Insert.cpp
        oatpp-mongo/driver/command/Insert.hpp
        oatpp-mongo/driver/command/Miscellaneous.hpp
        oatpp-mongo/driver/command/Update.cpp
        oatpp-mongo/driver/command/Update.hpp
        oatpp-mongo/driver/wire/Connection.cpp
        oatpp-mongo/driver/wire/Connection.hpp
        oatpp-mongo/driver/wire/Message.cpp
        oatpp-mongo/driver/wire/Message.hpp
        oatpp-mongo/driver/wire/OpMsg.cpp
        oatpp-mongo/driver/wire/OpMsg.hpp
)

set_target_properties(${OATPP_THIS_MODULE_NAME} PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(${OATPP_THIS_MODULE_NAME} ${LIB_OATPP_EXTERNAL})
endif()

target_link_oatpp(${OATPP_THIS_MODULE_NAME})

target_include_directories(${OATPP_THIS_MODULE_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${LIBMONGOCXX_INCLUDE_DIRS}
)

target_link_libraries(${OATPP_THIS_MODULE_NAME}
        PRIVATE ${LIBMONGOCXX_LIBRARIES}
)

#if (TARGET mongo::mongocxx_shared)
#    target_link_libraries(${OATPP_THIS_MODULE_NAME}
#            PUBLIC mongo::mongocxx_shared
#    )
#elseif(TARGET mongo::mongocxx_static)
#    target_link_libraries(${OATPP_THIS_MODULE_NAME}
#            PUBLIC mongo::mongocxx_static
#    )
#endif()

#######################################################################################################
## install targets

if(OATPP_INSTALL)
    include("../cmake/module-install.cmake")
endif()
