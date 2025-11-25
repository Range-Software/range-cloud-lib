# Create a static library
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_library(range-cloud-lib
    STATIC
        src/rcl_access_mode.cpp
        src/rcl_access_owner.cpp
        src/rcl_access_rights.cpp
        src/rcl_auth_token.cpp
        src/rcl_cloud_action.cpp
        src/rcl_cloud_action_info.cpp
        src/rcl_cloud_client.cpp
        src/rcl_cloud_process_info.cpp
        src/rcl_cloud_process_request.cpp
        src/rcl_cloud_process_response.cpp
        src/rcl_cloud_process_result.cpp
        src/rcl_cloud_session_info.cpp
        src/rcl_cloud_session_manager.cpp
        src/rcl_cloud_tool_action.cpp
        src/rcl_file_info.cpp
        src/rcl_file_manager.cpp
        src/rcl_file_manager_cache.cpp
        src/rcl_file_manager_settings.cpp
        src/rcl_file_quota.cpp
        src/rcl_file_tools.cpp
        src/rcl_group_info.cpp
        src/rcl_http_client.cpp
        src/rcl_http_client_settings.cpp
        src/rcl_http_message.cpp
        src/rcl_http_proxy_settings.cpp
        src/rcl_http_server.cpp
        src/rcl_http_server_settings.cpp
        src/rcl_http_settings.cpp
        src/rcl_network_message.cpp
        src/rcl_report_record.cpp
        src/rcl_software_manager.cpp
        src/rcl_software_manager_settings.cpp
        src/rcl_tls_key_store.cpp
        src/rcl_tls_trust_store.cpp
        src/rcl_user_info.cpp
        include/rcl_access_mode.h
        include/rcl_access_owner.h
        include/rcl_access_rights.h
        include/rcl_argument_option.h
        include/rcl_auth_token.h
        include/rcl_auth_token_validator.h
        include/rcl_cloud_action.h
        include/rcl_cloud_action_info.h
        include/rcl_cloud_client.h
        include/rcl_cloud_process_info.h
        include/rcl_cloud_process_request.h
        include/rcl_cloud_process_response.h
        include/rcl_cloud_process_result.h
        include/rcl_cloud_session_info.h
        include/rcl_cloud_session_manager.h
        include/rcl_cloud_tool_action.h
        include/rcl_file_info.h
        include/rcl_file_manager.h
        include/rcl_file_manager_cache.h
        include/rcl_file_manager_settings.h
        include/rcl_file_quota.h
        include/rcl_file_tools.h
        include/rcl_group_info.h
        include/rcl_http_client.h
        include/rcl_http_client_settings.h
        include/rcl_http_message.h
        include/rcl_http_proxy_settings.h
        include/rcl_http_server.h
        include/rcl_http_server_settings.h
        include/rcl_http_settings.h
        include/rcl_network_message.h
        include/rcl_report_record.h
        include/rcl_software_manager.h
        include/rcl_software_manager_settings.h
        include/rcl_tls_key_store.h
        include/rcl_tls_trust_store.h
        include/rcl_user_info.h
)

# Compiler definitions
target_compile_definitions(range-cloud-lib
    PRIVATE
        FILE_MAJOR_VERSION=0
        FILE_MINOR_VERSION=0
        FILE_RELEASE_VERSION=1
)

# Include directories
target_include_directories(range-cloud-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(range-cloud-lib
    PUBLIC
        range-base-lib
        Qt6::Concurrent
        Qt6::HttpServer
        Qt6::Network
        common_defines

)
target_link_libraries(range-cloud-lib PRIVATE Qt6::Core)
target_link_libraries(range-cloud-lib PRIVATE Qt6::Core)

qt_add_translations(range-cloud-lib
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/range-cloud-lib
)
