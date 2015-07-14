/* This file was generated by filter.py */

#define NUMBER_OF_INPUT_SAMPLES 80
#define NUMBER_OF_COEFFICIENTS 24
#define NUMBER_OF_OUTPUT_SAMPLES \
    (NUMBER_OF_INPUT_SAMPLES - NUMBER_OF_COEFFICIENTS + 1)

struct filter_test_t {
    const char *description;
    FAR const float *input;
    FAR const float *coefficients;
    FAR const float *ref_output;
};

FAR static const float mower_input[NUMBER_OF_INPUT_SAMPLES] = {
    /* sample 0-23 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 24-47 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 48-71 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,

    /* sample 72-79 */
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
};

FAR static const float mower_coefficients[NUMBER_OF_COEFFICIENTS] = {
    1.0, 0.0, -1.0, 0.0, 1.0, -1.0, 1.0, -1.0,
    0.0, 1.0, -1.0, 1.0, 0.0, -1.0, 0.0, 1.0,
    -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 0.0, -1.0,
};

FAR static const float mower_ref_output[NUMBER_OF_OUTPUT_SAMPLES] = {
    /* sample 0-23 */
    16.0, -8.0, -5.0, 9.0, -3.0, -5.0, 6.0, 0.0,
    -5.0, 4.0, -1.0, -1.0, 2.0, -1.0, -1.0, 4.0,
    -5.0, 0.0, 6.0, -5.0, -3.0, 9.0, -5.0, -8.0,

    /* sample 24-47 */
    16.0, -8.0, -5.0, 9.0, -3.0, -5.0, 6.0, 0.0,
    -5.0, 4.0, -1.0, -1.0, 2.0, -1.0, -1.0, 4.0,
    -5.0, 0.0, 6.0, -5.0, -3.0, 9.0, -5.0, -8.0,

    /* sample 48-56 */
    16.0, -8.0, -5.0, 9.0, -3.0, -5.0, 6.0, 0.0,
    -5.0
};


FAR static const float low_pass_6000_hz_input[NUMBER_OF_INPUT_SAMPLES] = {
    0.000000, -0.072610, 0.149275, -0.172995, 0.215707, -0.165142, 0.164981, -0.049051,
    0.031411, 0.111872, -0.102162, 0.227959, -0.152894, 0.235805, -0.086471, 0.135407,
    0.062791, -0.009827, 0.212048, -0.110232, 0.278459, -0.102403, 0.227708, 0.013661,
    0.094108, 0.174554, -0.039497, 0.290606, -0.090266, 0.298413, -0.023884, 0.197973,
    0.125333, 0.052692, 0.274543, -0.047764, 0.340901, -0.039988, 0.290094, 0.076018,
    0.156434, 0.236849, 0.022765, 0.352835, -0.028071, 0.360572, 0.038239, 0.260059,
    0.187381, 0.114701, 0.336512, 0.014164, 0.402787, 0.021855, 0.351893, 0.137772,
    0.218143, 0.298511, 0.084380, 0.414401, 0.033445, 0.422038, 0.099654, 0.321421,
    0.248690, 0.175955, 0.397711, 0.075307, 0.463873, 0.082883, 0.412862, 0.198681,
    0.278991, 0.359297, 0.145103, 0.475061, 0.094040, 0.482567, 0.160117, 0.381817,

};

FAR static const float low_pass_6000_hz_coefficients[NUMBER_OF_COEFFICIENTS] = {
    0.002041, -0.001123, -0.001880, 0.007802, -0.012847, 0.008347, 0.012626, -0.045254,
    0.067508, -0.043599, -0.077939, 0.584317, 0.584317, -0.077939, -0.043599, 0.067508,
    -0.045254, 0.012626, 0.008347, -0.012847, 0.007802, -0.001880, -0.001123, 0.002041,

};

FAR static const float low_pass_6000_hz_ref_output[NUMBER_OF_OUTPUT_SAMPLES] = {
    0.044563, 0.048485, 0.054648, 0.054429, 0.063758, 0.061822, 0.071150, 0.070927,
    0.077084, 0.080999, 0.082671, 0.090724, 0.089226, 0.098990, 0.097489, 0.105536,
    0.107200, 0.111103, 0.117245, 0.117005, 0.126312, 0.124353, 0.133657, 0.133408,
    0.139539, 0.143427, 0.145072, 0.153095, 0.151567, 0.161301, 0.159767, 0.167781,
    0.169412, 0.173280, 0.179387, 0.179110, 0.188379, 0.186382, 0.195646, 0.195357,
    0.201446, 0.205291, 0.206893, 0.214872, 0.213298, 0.222986, 0.221406, 0.229371,
    0.230953, 0.234771, 0.240827, 0.240498, 0.249715, 0.247663, 0.256872, 0.256528,
    0.262560,
};


struct filter_test_t filter_test[] = {
    {
        .description = "test: mower",
        .input = mower_input,
        .coefficients = mower_coefficients,
        .ref_output = mower_ref_output
    },

    {
        .description = "test: low_pass_6000_hz",
        .input = low_pass_6000_hz_input,
        .coefficients = low_pass_6000_hz_coefficients,
        .ref_output = low_pass_6000_hz_ref_output
    },

    {
        .description = NULL,
        .input = NULL,
        .coefficients = NULL,
        .ref_output = NULL
    },
};
