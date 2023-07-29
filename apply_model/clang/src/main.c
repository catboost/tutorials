#include <stdio.h>
#include <math.h>

// Bring catboost module into the scope
#include <path_to_dir_with_header_file/c_api.h>

double sigmoid(double x) {
    return 1. / (1. + exp(-x));
}

char* answer(bool makes_over_50k_a_year) {
    if (makes_over_50k_a_year) {
        return "makes over 50k a year";
    } else {
        return "doesn't make over 50k a year";
    }
}

int main(int argc, const char * argv[]) {
    // Load "adult.cbm" model that we trained withing Jupyter Notebook
    ModelCalcerHandle* modelHandle;
    modelHandle = ModelCalcerCreate();
    if (!LoadFullModelFromFile(modelHandle, "adult.cbm")) {
        printf("LoadFullModelFromFile error message: %s\n", GetErrorString());
    }

    // You can also try to load your own model just replace "adult.cbm" with path to your model that classifies data
    // from UCI Adult Dataset.

    printf("Adult dataset model metainformation\n");

    printf("tree count: %zu\n", GetTreeCount(modelHandle));

    // In our case we were solving a binary classification problem (weather person makes over 50K a year), so the
    // dimension of the prediction will be 1, it will return probability of the object to belong to the positive
    // class; in our case we had two classed encoded as "<=50K" and ">50K", during data preprocessing (see
    // `get_fixed_adult()` in Notebook) we encoded "<=50K" as 0 and ">50K" as 1, so that ">50K" became a positive
    // class. Probability of the negative class ("<=50K") can be easily deduced as (1-p) where p is a probability of
    // positive class.
    //
    // For most of cases prediction dimension will be 1 (for regression and for ranking), it can be N for cases of
    // multiclassification, where N is a number of classes.
    printf("prediction dimension: %zu\n", GetDimensionsCount(modelHandle));

    printf("numeric feature count: %zu\n", GetFloatFeaturesCount(modelHandle));

    printf("categoric feature count: %zu\n", GetCatFeaturesCount(modelHandle));

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

    printf("\n");

    float pers_a_num_feat[6] = {25., 226802., 7., 0., 0., 40.};
    char* pers_a_cat_feat[8] = {"Private","11th","Never-married","Machine-op-inspct","Own-child","Black","Male","United-States"};
    
    double result_a[1];

    const float* a_num_feat_ptr = pers_a_num_feat;
    const char** a_cat_feat_ptr = pers_a_cat_feat;

    if (!CalcModelPrediction(
                modelHandle,
                1,
                &a_num_feat_ptr, 6,
                &a_cat_feat_ptr, 8,
                &result_a, 1)
       ) {
        printf("CalcModelPrediction error message: %s\n", GetErrorString());
    }

    // Since we made prediction only for one person and prediction dimension is 1, proability of person A make
    // over 50K will have index 0 in `person_a_prediction`.
    //
    // CatBoost doesn't compute "probability", to turn CatBoost prediction into a probability we'll need to apply
    // sigmoid function.
    double pers_a_makes_over_50k_prob = sigmoid(result_a[0]);
    printf("Person A make over 50K a year with probability %f\n", pers_a_makes_over_50k_prob);

    // When we were training CatBoost we used a default classification threshold for AUC which is equal to 0.5,
    // this means that our formula is optimized for this threashold, though we may change threshold to optimize some
    // other metric on a different dataset, but we won't do it in this tutorial.
    double classification_threshold = 0.5;

    bool pers_a_makes_over_50k = pers_a_makes_over_50k_prob > classification_threshold;
    printf("Person A %s\n", answer(pers_a_makes_over_50k));

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

    printf("\n");

    float pers_b_num_feat[w] = {40., 85019., 16., 0., 0., 45.};
    char* pers_b_cat_feat[8] = {"Private","Doctorate","Married-civ-spouce","Prof-specialty","Husband","Asian-Pac-Islander","Male","nan"};

    double result_b[1];

    const float* b_num_feat_ptr = pers_b_num_feat;
    const char** b_cat_feat_ptr = pers_b_cat_feat;

    if (!CalcModelPrediction(
                modelHandle,
                1,
                &b_num_feat_ptr, 6,
                &b_cat_feat_ptr, 8,
                &result_b, 1)
       ) {
        printf("CalcModelPrediction error message: %s\n", GetErrorString());
    }
    double pers_b_makes_over_50k_prob = sigmoid(result_b[0]);
    bool pers_b_makes_over_50k = pers_b_makes_over_50k_prob > classification_threshold;
    printf("Person B make over 50K a year with probability %f\n", pers_b_makes_over_50k_prob);
    printf("Person B %s\n", answer(pers_b_makes_over_50k));

    // Let's try to apply the model to Person A and Person B in one call.
    printf("\n");

    float* pers_ab_num_feat[2] = {pers_a_num_feat, pers_b_num_feat};
    char** pers_ab_cat_feat[2] = {pers_a_cat_feat, pers_b_cat_feat};

    double result_ab[2];

    const float** ab_num_feat_ptr = (const float**)pers_ab_num_feat;
    const char*** ab_cat_feat_ptr = (const char**)pers_ab_cat_feat;

    if (!CalcModelPrediction(
                modelHandle,
                2,
                ab_num_feat_ptr, 6,
                ab_cat_feat_ptr, 8,
                &result_ab, 2)
       ) {
        printf("CalcModelPrediction error message: %s\n", GetErrorString());
    }
    double pers_ab_makes_over_50k_prob[2] = {sigmoid(result_ab[0]), sigmoid(result_ab[1])};
    bool pers_ab_makes_over_50k[2] = {pers_ab_makes_over_50k_prob[0] > classification_threshold, pers_ab_makes_over_50k_prob[1] > classification_threshold};

    printf("Using batch interface\n");

    // Predictions should be same as above
    printf("Person A make over 50K a year with probability %f\n", pers_ab_makes_over_50k_prob[0]);
    printf("Person A %s\n", answer(pers_ab_makes_over_50k[0]));
    printf("Person B make over 50K a year with probability %f\n", pers_ab_makes_over_50k_prob[1]);
    printf("Person B %s\n", answer(pers_ab_makes_over_50k[1]));

    ModelCalcerDelete(modelHandle);
    return 0;
}