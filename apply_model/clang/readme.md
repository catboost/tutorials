# Apply CatBoost model from C
This tutorial consists of two parts:
- first part where we preprocess dataset and train the classifier model.
  This part can be found in [train_model.ipynb](train_model.ipynb).
- second part where we load model into C application and then apply it.
  This part presented as a C file. At first you need to build a library, as it is suggested on [Evaluation library](https://catboost.ai/en/docs/concepts/c-plus-plus-api_dynamic-c-pluplus-wrapper). To run, you can execute:
  * in case Linux/macOS

  `clang <your sources and options> -L<path_to_dir_with_libcatboostmodel> -lcatboostmodel`
  * in case Windows

  `cl.exe <your sources and options> /link <path_to_dir_with_libcatboostmodel>\catboostmodel.lib`
  
  If you just want to look at code snippets you can go directly to [src/main.c](src/main.c).
