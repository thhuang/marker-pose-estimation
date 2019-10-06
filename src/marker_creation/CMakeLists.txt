add_executable(marker_creation marker_creation.cc)

target_link_libraries(marker_creation
  mpe
  ${THIRD_PARTY_LIBS}
  )