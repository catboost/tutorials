# Apply CatBoost model from C++
This tutorial consists of two parts:
- first part where we preprocess dataset and train the classifier model.
  This part can be found in [train_model.ipynb](model/train_model.ipynb).
- second part where we load model into C++ application and then apply it.
  This part presented as a small CMake project.

  To configure CMake, execute:
  ```bash
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=<path_to_clang> -DCMAKE_CXX_COMPILER:FILEPATH=<path_to_clang++> -Bbuild -G "Unix Makefiles"
  ```

  Build target `apply_model`:
  ```bash
  cmake --build build --config Release --target apply_model
  ```

  Run binary:
  ```bash
  build/bin/apply_model -m model/adult.cbm
  ```
