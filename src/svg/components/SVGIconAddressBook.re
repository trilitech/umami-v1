open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.317 3.040 C 4.942 3.104,4.606 3.363,4.406 3.740 L 4.300 3.940 4.290 12.127 L 4.279 20.315 4.365 20.544 C 4.469 20.823,4.708 21.091,4.978 21.234 L 5.180 21.340 12.000 21.340 L 18.820 21.340 19.020 21.233 C 19.260 21.106,19.477 20.885,19.606 20.640 L 19.700 20.460 19.700 12.200 L 19.700 3.940 19.603 3.743 C 19.474 3.480,19.241 3.255,18.966 3.126 L 18.740 3.020 12.120 3.014 C 8.479 3.011,5.418 3.023,5.317 3.040 M5.560 12.180 L 5.560 20.720 5.860 20.720 L 6.160 20.720 6.160 12.180 L 6.160 3.639 12.430 3.650 L 18.700 3.660 18.835 3.756 C 18.909 3.809,19.003 3.926,19.044 4.016 C 19.116 4.174,19.118 4.465,19.119 12.204 L 19.120 20.227 19.028 20.402 C 18.965 20.521,18.881 20.605,18.762 20.668 L 18.587 20.760 12.016 20.760 C 7.480 20.760,5.410 20.747,5.333 20.718 C 5.165 20.654,4.965 20.443,4.920 20.282 C 4.895 20.191,4.881 17.286,4.882 12.180 C 4.884 3.754,4.877 4.042,5.059 3.841 C 5.147 3.744,5.357 3.642,5.470 3.641 L 5.560 3.640 5.560 12.180 M4.920 5.260 L 4.920 5.560 5.220 5.560 L 5.520 5.560 5.520 5.260 L 5.520 4.960 5.220 4.960 L 4.920 4.960 4.920 5.260 M12.180 8.174 C 11.885 8.271,11.665 8.408,11.439 8.635 C 10.433 9.643,10.998 11.368,12.414 11.611 C 13.193 11.744,14.000 11.276,14.297 10.520 C 14.634 9.661,14.247 8.677,13.412 8.275 C 13.169 8.157,13.098 8.142,12.740 8.132 C 12.486 8.125,12.282 8.140,12.180 8.174 M4.920 8.480 L 4.920 8.800 5.220 8.800 L 5.520 8.800 5.520 8.480 L 5.520 8.160 5.220 8.160 L 4.920 8.160 4.920 8.480 M13.227 8.724 C 13.679 8.940,13.938 9.361,13.937 9.880 C 13.936 10.242,13.846 10.468,13.595 10.741 C 12.997 11.390,11.993 11.261,11.563 10.480 C 11.430 10.239,11.420 10.195,11.420 9.880 C 11.421 9.478,11.501 9.265,11.760 8.985 C 12.144 8.568,12.694 8.470,13.227 8.724 M4.920 12.020 L 4.920 12.320 5.220 12.320 L 5.520 12.320 5.520 12.020 L 5.520 11.720 5.220 11.720 L 4.920 11.720 4.920 12.020 M11.960 12.704 C 10.666 12.941,10.057 13.178,9.673 13.593 C 9.324 13.969,9.224 14.240,9.175 14.934 C 9.151 15.282,9.155 15.320,9.217 15.320 C 9.410 15.320,15.440 15.079,15.795 15.057 L 16.211 15.032 16.190 14.696 C 16.173 14.419,16.143 14.306,16.025 14.057 C 15.722 13.419,15.196 13.095,14.000 12.812 C 13.647 12.729,13.458 12.711,12.820 12.699 C 12.402 12.692,12.015 12.694,11.960 12.704 M14.040 13.310 C 15.012 13.563,15.449 13.841,15.630 14.321 C 15.676 14.442,15.701 14.552,15.686 14.566 C 15.666 14.587,12.442 14.724,9.960 14.810 L 9.660 14.820 9.672 14.680 C 9.696 14.403,9.809 14.147,9.995 13.948 C 10.211 13.717,10.475 13.576,11.014 13.404 C 11.645 13.202,11.963 13.162,12.820 13.176 C 13.512 13.188,13.621 13.200,14.040 13.310 M4.920 15.580 L 4.920 15.880 5.220 15.880 L 5.520 15.880 5.520 15.580 L 5.520 15.280 5.220 15.280 L 4.920 15.280 4.920 15.580 M4.920 19.140 L 4.920 19.440 5.220 19.440 L 5.520 19.440 5.520 19.140 L 5.520 18.840 5.220 18.840 L 4.920 18.840 4.920 19.140 " fillRule=`evenodd> </Path> </Svg>;
