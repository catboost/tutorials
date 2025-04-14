#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>
#include <catboost/c_api.h>
#include <catboost/wrapped_calcer.h>

float Sigmoid(const float x) {
  return 1. / (1. + std::exp(-x));
}

std::string Answer(const bool makes_over_50k_a_year) {
  if (makes_over_50k_a_year) {
    return "makes over 50K a year";
  }
  return "doesn't make over 50K a year";
}

int main(const int argc, const char* argv[]) {
  argparse::ArgumentParser program("apply-model", "",
                                   argparse::default_arguments::help);
  program.add_argument("-m", "--model")
      .help("path to model")
      .metavar("model")
      .action([](const auto& path) {
        return std::filesystem::path(path);
      });
  program.parse_args(argc, argv);
  const auto model_path = program.get<std::filesystem::path>("-m");

  // Load model that we trained withing Jupyter Notebook
  ModelCalcerWrapper model(model_path);
  std::cout << "Adult dataset model metainformation" << std::endl;
  std::cout << "Tree count: " << model.GetTreeCount() << std::endl;

  // In our case we were solving a binary classification problem (weather person makes over 50K a year), so the
  // dimension of the prediction will be 1, it will return probability of the object to belong to the positive
  // class; in our case we had two classed encoded as "<=50K" and ">50K", during data preprocessing (see
  // `get_fixed_adult()` in Notebook) we encoded "<=50K" as 0 and ">50K" as 1, so that ">50K" became a positive
  // class. Probability of the negative class ("<=50K") can be easily deduced as (1-p) where p is a probability of
  // positive class.
  //
  // For most of cases prediction dimension will be 1 (for regression and for ranking), it can be N for cases of
  // multiclassification, where N is a number of classes.
  std::cout << "numeric feature count: " << model.GetFloatFeaturesCount()
            << std::endl;
  std::cout << "categoric feature count: " << model.GetCatFeaturesCount()
            << std::endl;

  std::cout << std::endl;

  // When we were training CatBoost we used a default classification threshold for AUC which is equal to 0.5,
  // this means that our formula is optimized for this threashold, though we may change threshold to optimize some
  // other metric on a different dataset, but we won't do it in this tutorial.
  static constexpr auto kClassificationThreshold = 0.5;

  // Ok now lets try to use our model for prediction. We'll look at the test part of Adult dataset. You will need
  // to download it [1] from UCI repository. Look for "adult.test", "adult.name" will also be useful because it
  // in contains human-readable description of the dataset.
  //
  // So the first line of test part of the dataset is:
  //
  // "25, Private, 226802, 11th, 7, Never-married, Machine-op-inspct, Own-child, Black, Male, 0, 0, 40, United-States, <=50K."
  //
  // Based on "adult.name" we can recover its vectors of numeric and categoric features (in our case all
  // "continuous" features are numeric and all other features are categoric):
  //
  // numericFeatures: {25, 226802, 7, 0, 0, 40}
  // categoricFeatures: {"Private", "11th", "Never-married", "Machine-op-inspct", "Own-child", "Black", "Male", "United-States"}
  //
  // And he doesn't make 50K per year. Also note that order of numeric and categoric features in source data and
  // in `numericFeatures` and `categoricFeatures` is kept the same. Otherwise we can't apply the model (well, we
  // can, but result of prediction will be garbage).
  //
  // Now lets run it! And let's call this person "person A", to make variable names unique.
  //
  // [1]: https://archive.ics.uci.edu/ml/machine-learning-databases/adult/
  const std::vector<float> person_a_numeric_features(
      {25., 226'802., 7., 0., 0., 40.});
  const std::vector<std::string> person_a_categoric_features(
      {"Private", "11th", "Never-married", "Machine-op-inspct", "Own-child",
       "Black", "Male", "United-States"});
  const auto person_a_prediction =
      model.Calc(person_a_numeric_features, person_a_categoric_features);
  const auto person_a_makes_over_50k_probability = Sigmoid(person_a_prediction);

  // Since we made prediction only for one person and prediction dimension is 1, proability of person A make
  // over 50K will have index 0 in `person_a_prediction`.
  //
  // CatBoost doesn't compute "probability", to turn CatBoost prediction into a probability we'll need to apply
  // sigmoid function.
  const auto person_a_makes_over_50k =
      person_a_makes_over_50k_probability > kClassificationThreshold;
  std::cout << "Person A make over 50K a year with probability "
            << person_a_makes_over_50k_probability << std::endl;
  std::cout << "Person A " << Answer(person_a_makes_over_50k) << std::endl;
  std::cout << std::endl;

  // Now lets find an example with missing features and income greater than 50K a year. At line 40 of "adult.test"
  // we can find following line:
  //
  // "40, Private, 85019, Doctorate, 16, Married-civ-spouse, Prof-specialty, Husband, Asian-Pac-Islander, Male, 0, 0, 45, ?, >50K."
  //
  // Lets call this person "Person B", dataset missing (missing features are marked with "?") "native-county"
  // feature for Person B. When we were doing preprocessing in `get_fixed_adult` we replaced missing categoric
  // features with string "nan", now, when we apply trained model we must also use "nan" for missing features.
  // Lets write out feature vectors for Person B:
  //
  // numericFeatures = {40, 85019, 16, 0, 0, 45};
  // categoricFeatures = {"Private", "Doctorate", "Married-civ-spouce", "Prof-specialty", "Husband", "Asian-Pac-Islander", "Male", "nan"};
  //
  // And according to the dataset Person B makes more than 50K a year. Ok, lets try to apply the model to this
  // example.
  const std::vector<float> person_b_numeric_features(
      {40., 85019., 16., 0., 0., 45.});
  const std::vector<std::string> person_b_categoric_features(
      {"Private", "Doctorate", "Married-civ-spouce", "Prof-specialty",
       "Husband", "Asian-Pac-Islander", "Male", "nan"});
  const auto person_b_prediction =
      model.Calc(person_b_numeric_features, person_b_categoric_features);
  const auto person_b_makes_over_50k_probability = Sigmoid(person_b_prediction);

  const auto person_b_makes_over_50k =
      person_b_makes_over_50k_probability > kClassificationThreshold;
  std::cout << "Person B make over 50K a year with probability "
            << person_b_makes_over_50k_probability << std::endl;
  std::cout << "Person B " << Answer(person_b_makes_over_50k) << std::endl;
  std::cout << std::endl;

  // Let's try to apply the model to Person A and Person B in one call.
  const std::vector<std::vector<float>> persons_ab_numberic_features =
      {person_a_numeric_features, person_b_numeric_features};
  const std::vector<std::vector<std::string>> persons_ab_categoric_features =
      {person_a_categoric_features, person_b_categoric_features};
  const auto persons_ab_predictions = model.Calc(
      persons_ab_numberic_features, persons_ab_categoric_features);
  const std::vector<float> persons_ab_make_over_50k_probabilities = {
      Sigmoid(persons_ab_predictions[0]), Sigmoid(persons_ab_predictions[1])};
  const std::vector<bool> persons_ab_make_over_50k = {
      persons_ab_make_over_50k_probabilities[0] > kClassificationThreshold,
      persons_ab_make_over_50k_probabilities[1] > kClassificationThreshold};

  // Predictions should be same as above
  std::cout << "Using batch interface" << std::endl;
  std::cout << "Person A make over 50K a year with probability "
            << persons_ab_make_over_50k_probabilities[0] << std::endl;
  std::cout << "Person A " << Answer(persons_ab_make_over_50k[0]) << std::endl;
  std::cout << "Person B make over 50K a year with probability "
            << persons_ab_make_over_50k_probabilities[1] << std::endl;
  std::cout << "Person B " << Answer(persons_ab_make_over_50k[1]) << std::endl;
}
