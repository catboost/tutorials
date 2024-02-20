add_executable(
    apply_model
    main.cpp
)

target_link_libraries(
    apply_model
    argparse
    catboost
)
