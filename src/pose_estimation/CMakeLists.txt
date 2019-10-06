add_executable(pose_estimation pose_estimation.cc)

target_link_libraries(pose_estimation
  mpe
  ${THIRD_PARTY_LIBS}
  )