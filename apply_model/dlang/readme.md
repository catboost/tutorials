# Apply CatBoost model from D
This tutorial consists of two parts:
- first part where we preprocess dataset and train the classifier model.
  This part can be found in [train_model.ipynb](train_model.ipynb).
- second part where we load model into D application and then apply it.
  This part presented as a D file. At first you need to build a library, as it is suggested on [Evaluation library](https://catboost.ai/en/docs/concepts/c-plus-plus-api_dynamic-c-pluplus-wrapper).
  
  After that you need to generate preprocessed header file (.i) for ImportC functionality. To prepare preprocessed header you need to create a file [lib_import.c](src/lib_import.c) consists of one line 
  
  `#include <path_to_header_file/c_api.h>`

  and use available C compiler (or built-in preprocessor in D compiler) as suggested on the page of [ImportC Documentation](https://dlang.org/spec/importc.html):

  `clang -E lib_import.c -o lib_import.i`
  
  To run, you can execute:
  * in case Linux/macOS

  `ldc2 <your sources and options> <your_preprocessed_header_file> -L<path_to_dir_with_libcatboostmodel>/libcatboostmodel.{so/dylib}`
  * in case Windows

  `ldc2.exe <your sources and options> <your_preprocessed_header_file> /link <path_to_dir_with_libcatboostmodel>\catboostmodel.{lib/dll}`
  
  If you just want to look at code snippets you can go directly to [src/main.d](src/main.d).
