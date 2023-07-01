set(CATBOOST_TAG v1.2)
set(CATBOOST_BASE_PATH ${CMAKE_BINARY_DIR}/third_party/catboost)

file(
    DOWNLOAD
    https://github.com/catboost/catboost/releases/download/${CATBOOST_TAG}/libcatboostmodel.so
    ${CATBOOST_BASE_PATH}/libcatboostmodel.so
    SHOW_PROGRESS
)

file(
    DOWNLOAD
    https://raw.githubusercontent.com/catboost/catboost/${CATBOOST_TAG}/catboost/libs/model_interface/c_api.cpp
    ${CATBOOST_BASE_PATH}/c_api.cpp
    SHOW_PROGRESS
)

file(
    DOWNLOAD
    https://raw.githubusercontent.com/catboost/catboost/${CATBOOST_TAG}/catboost/libs/model_interface/c_api.h
    ${CATBOOST_BASE_PATH}/c_api.h
    SHOW_PROGRESS
)

file(
    DOWNLOAD
    https://raw.githubusercontent.com/catboost/catboost/${CATBOOST_TAG}/catboost/libs/model_interface/wrapped_calcer.h
    ${CATBOOST_BASE_PATH}/wrapped_calcer.h
    SHOW_PROGRESS
)

add_library(catboost SHARED IMPORTED GLOBAL)
set_property(TARGET catboost PROPERTY IMPORTED_LOCATION ${CATBOOST_BASE_PATH}/libcatboostmodel.so)
