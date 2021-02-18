open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M9.407 4.300 C 8.823 4.496,7.940 4.913,7.830 5.045 C 7.737 5.157,7.742 5.327,7.843 5.527 C 8.269 6.362,7.428 7.694,6.330 7.922 C 6.044 7.981,5.738 7.951,5.527 7.843 C 5.327 7.742,5.157 7.737,5.045 7.830 C 4.806 8.030,4.084 9.824,4.140 10.079 C 4.172 10.225,4.209 10.254,4.490 10.360 C 5.120 10.596,5.528 11.466,5.404 12.310 C 5.310 12.949,4.945 13.474,4.476 13.645 C 4.229 13.735,4.120 13.837,4.121 13.979 C 4.122 14.285,4.848 16.005,5.045 16.170 C 5.156 16.263,5.322 16.258,5.516 16.157 C 6.347 15.724,7.694 16.570,7.922 17.670 C 7.981 17.956,7.951 18.262,7.843 18.473 C 7.742 18.673,7.737 18.843,7.830 18.955 C 8.028 19.192,9.826 19.916,10.078 19.860 C 10.225 19.828,10.254 19.792,10.360 19.510 C 10.596 18.880,11.466 18.472,12.310 18.596 C 12.949 18.690,13.474 19.055,13.645 19.524 C 13.735 19.771,13.837 19.880,13.979 19.879 C 14.294 19.877,16.052 19.131,16.183 18.944 C 16.263 18.830,16.254 18.669,16.157 18.484 C 15.724 17.653,16.570 16.306,17.670 16.078 C 17.956 16.019,18.278 16.050,18.484 16.157 C 18.678 16.258,18.844 16.263,18.954 16.170 C 19.194 15.966,19.916 14.175,19.860 13.921 C 19.828 13.776,19.791 13.745,19.515 13.642 C 19.056 13.470,18.689 12.938,18.596 12.310 C 18.470 11.457,18.884 10.589,19.529 10.354 C 19.772 10.265,19.880 10.162,19.879 10.021 C 19.878 9.715,19.152 7.995,18.955 7.830 C 18.843 7.737,18.673 7.742,18.473 7.843 C 17.638 8.269,16.306 7.428,16.078 6.330 C 16.019 6.044,16.050 5.722,16.157 5.516 C 16.254 5.331,16.263 5.170,16.183 5.056 C 16.024 4.829,14.181 4.083,13.921 4.140 C 13.775 4.172,13.746 4.209,13.640 4.490 C 13.404 5.120,12.534 5.528,11.690 5.404 C 11.051 5.310,10.526 4.945,10.355 4.476 C 10.265 4.229,10.163 4.120,10.021 4.121 C 9.976 4.121,9.700 4.202,9.407 4.300 M9.860 4.920 C 9.909 5.074,10.200 5.396,10.456 5.579 C 10.599 5.681,10.865 5.820,11.048 5.888 C 11.337 5.996,11.444 6.015,11.880 6.030 C 12.453 6.050,12.761 5.996,13.167 5.802 C 13.618 5.588,13.863 5.357,14.186 4.846 C 14.205 4.816,14.759 5.018,15.132 5.192 C 15.398 5.315,15.517 5.391,15.503 5.427 C 15.424 5.632,15.394 6.026,15.436 6.308 C 15.524 6.897,15.782 7.376,16.260 7.834 C 16.721 8.276,17.209 8.513,17.801 8.581 C 17.991 8.603,18.418 8.557,18.575 8.496 C 18.607 8.484,18.687 8.607,18.792 8.829 C 18.939 9.138,19.160 9.723,19.160 9.802 C 19.160 9.818,19.101 9.854,19.030 9.884 C 18.959 9.914,18.783 10.055,18.639 10.199 C 18.182 10.657,17.960 11.247,17.960 12.005 C 17.960 12.675,18.131 13.191,18.508 13.657 C 18.677 13.865,18.988 14.123,19.125 14.168 C 19.188 14.189,19.046 14.620,18.808 15.132 C 18.685 15.398,18.609 15.517,18.573 15.503 C 18.368 15.424,17.974 15.394,17.692 15.436 C 17.103 15.524,16.624 15.782,16.166 16.260 C 15.724 16.721,15.487 17.209,15.419 17.801 C 15.397 17.991,15.443 18.418,15.504 18.575 C 15.516 18.607,15.393 18.687,15.171 18.792 C 14.862 18.939,14.277 19.160,14.198 19.160 C 14.182 19.160,14.146 19.101,14.116 19.030 C 14.022 18.804,13.572 18.395,13.240 18.234 C 12.774 18.009,12.454 17.947,11.872 17.969 C 11.186 17.996,10.768 18.149,10.294 18.547 C 10.115 18.697,9.873 19.001,9.830 19.130 C 9.811 19.187,9.358 19.036,8.868 18.808 C 8.616 18.691,8.484 18.608,8.496 18.576 C 8.557 18.418,8.603 17.991,8.581 17.801 C 8.513 17.209,8.276 16.721,7.834 16.260 C 7.376 15.782,6.897 15.524,6.308 15.436 C 6.027 15.394,5.632 15.424,5.428 15.503 C 5.366 15.526,5.098 14.988,4.941 14.523 C 4.820 14.166,4.820 14.172,4.920 14.140 C 5.228 14.042,5.691 13.476,5.891 12.952 C 6.014 12.631,6.018 12.600,6.018 12.000 C 6.018 11.400,6.014 11.369,5.891 11.048 C 5.691 10.524,5.228 9.958,4.920 9.860 C 4.820 9.828,4.820 9.834,4.941 9.477 C 5.093 9.026,5.366 8.474,5.425 8.496 C 5.582 8.557,6.009 8.603,6.199 8.581 C 6.828 8.508,7.286 8.277,7.781 7.781 C 8.277 7.286,8.508 6.828,8.581 6.199 C 8.603 6.009,8.557 5.582,8.496 5.425 C 8.475 5.368,9.012 5.099,9.460 4.944 C 9.807 4.823,9.829 4.821,9.860 4.920 M11.280 8.580 C 9.987 8.869,8.972 9.837,8.614 11.120 C 8.511 11.489,8.511 12.511,8.614 12.880 C 8.896 13.893,9.591 14.721,10.527 15.158 C 11.074 15.413,11.294 15.458,12.000 15.458 C 12.720 15.458,12.940 15.412,13.500 15.141 C 14.277 14.766,14.817 14.215,15.180 13.428 C 15.413 12.923,15.458 12.691,15.458 12.000 C 15.458 11.293,15.414 11.074,15.156 10.522 C 14.725 9.599,13.890 8.896,12.900 8.620 C 12.563 8.526,11.624 8.503,11.280 8.580 M12.752 9.243 C 13.538 9.444,14.232 10.029,14.586 10.787 C 14.771 11.184,14.851 11.550,14.851 12.000 C 14.851 13.585,13.602 14.842,12.020 14.850 C 11.530 14.852,11.172 14.771,10.740 14.561 C 9.746 14.075,9.152 13.118,9.152 12.000 C 9.152 10.518,10.213 9.339,11.720 9.147 C 11.949 9.118,12.440 9.163,12.752 9.243 " fillRule=`evenodd> </Path> </Svg>;
