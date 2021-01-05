open ReactNative;

let useAnimatedValue = value => {
  React.useRef(Animated.Value.create(value)).current;
};

let useAnimationOpen = (~speed=20., ~bounciness=3., visible, onClose) => {
  let (showContent, setShowContent) = React.useState(_ => visible);

  let animatedOpenValue = useAnimatedValue(0.);

  let animate = (~endCallback=?, visible) => {
    Animated.spring(
      animatedOpenValue,
      Animated.Value.Spring.config(
        ~toValue=Animated.Value.Spring.fromRawValue(visible ? 1. : 0.),
        ~speed,
        ~bounciness,
        ~useNativeDriver=true,
        (),
      ),
    )
    ->Animated.start(~endCallback?, ());
  };

  React.useEffect1(
    () => {
      if (visible == true) {
        setShowContent(_ => true);
        animate(true);
      } else {
        animate(
          false,
          ~endCallback=_ => {
            setShowContent(_ => false);
            onClose();
            ();
          },
        );
      };
      None;
    },
    [|visible|],
  );

  (showContent, animatedOpenValue);
};
