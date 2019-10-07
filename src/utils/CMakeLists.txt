add_library(mpe SHARED
  marker_detector.cc
  path_loader.cc
  draw_cube.cc
  )

target_link_libraries(mpe
  ${THIRD_PARTY_LIBS}
  -lstdc++fs
  )