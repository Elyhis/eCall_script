add_executable(transcription
    main.cpp
    nmea.cpp
    test.cpp
)

target_include_directories(transcription PRIVATE
    ${CMAKE_SOURCE_DIR}/script
)

# Lier avec la bibliothèque sdx_api si nécessaire
target_link_libraries(transcription PRIVATE sdx_api)
