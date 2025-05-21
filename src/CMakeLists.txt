add_executable(app
    main.cpp
    nmea.cpp
    scenario.cpp
    mathFormulaJO.cpp
)

target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/inc/sdx_api
app PRIVATE ${PROJECT_SOURCE_DIR}/inc)

# Lier avec la bibliothèque sdx_api si nécessaire
target_link_libraries(app PRIVATE sdx_api)

set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})