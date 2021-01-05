open ReactNative;

let useAnimatedValue = value => {
  React.useRef(Animated.Value.create(value)).current;
};

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~textAlign=`center, ()),
      "modal": style(~flex=1., ()),
      "modalOverlay":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(~backgroundColor="black", ()),
        |]),
      "modalView":
        style(
          ~marginTop=auto,
          ~marginBottom=auto, // fix android bug : marginVertical doesn't work with auto
          ~maxHeight=90.->pct,
          ~backgroundColor="transparent", // fix android bug : no animation without it
          ~opacity=1.,
          ~transform=[|scale(~scale=1.)|],
          (),
        ),
    })
  );

[@react.component]
let make = (~visible, ~onRequestClose: unit => unit=unit => unit, ~children) => {
  let (showContent, setShowContent) = React.useState(_ => visible);

  let overlayOpacity = useAnimatedValue(0.);
  let viewScale = useAnimatedValue(0.8);
  let viewOpacity = useAnimatedValue(0.);

  let animateOverlay = (~endCallback=?, visible) => {
    Animated.parallel(
      [|
        Animated.spring(
          overlayOpacity,
          Animated.Value.Spring.config(
            ~toValue=Animated.Value.Spring.fromRawValue(visible ? 1. : 0.),
            ~speed=20.,
            ~bounciness=3.,
            ~useNativeDriver=true,
            (),
          ),
        ),
        Animated.spring(
          viewScale,
          Animated.Value.Spring.config(
            ~toValue=Animated.Value.Spring.fromRawValue(visible ? 1. : 0.8),
            ~speed=20.,
            ~bounciness=3.,
            ~useNativeDriver=true,
            (),
          ),
        ),
        Animated.spring(
          viewOpacity,
          Animated.Value.Spring.config(
            ~toValue=Animated.Value.Spring.fromRawValue(visible ? 1. : 0.),
            ~speed=20.,
            ~bounciness=3.,
            ~useNativeDriver=true,
            (),
          ),
        ),
      |],
      {stopTogether: true},
    )
    ->Animated.start(~endCallback?, ());
  };

  let onRequestClose = () => {
    animateOverlay(
      false,
      ~endCallback=_ => {
        setShowContent(_ => false);
        onRequestClose();
        ();
      },
    )
    ->ignore;
  };

  React.useEffect1(
    () => {
      if (visible == true) {
        setShowContent(_ => true);
        animateOverlay(true);
      } else {
        animateOverlay(
          false,
          ~endCallback=_ => {
            setShowContent(_ => false);
            onRequestClose();
            ();
          },
        );
      };
      None;
    },
    [|visible|],
  );

  let theme = ThemeContext.useTheme();

  <Modal
    animationType=`none
    transparent=true
    supportedOrientations=[|
      Modal.Orientation.portrait,
      Modal.Orientation.landscape,
    |]
    visible=showContent
    onRequestClose>
    <DocumentContext>
      <View style=styles##modal>
        <Animated.View
          style=Style.(
            array([|
              styles##modalOverlay,
              StyleSheet.absoluteFillObject,
              style(~backgroundColor=theme.colors.scrim, ()),
              style(~opacity=overlayOpacity->Animated.StyleProp.float, ()),
            |])
          )
        />
        <Animated.View
          style=Style.(
            array([|
              styles##modalView,
              style(
                ~opacity=viewOpacity->Animated.StyleProp.float,
                ~transform=[|
                  scale(~scale=viewScale->Animated.StyleProp.float),
                |],
                (),
              ),
            |])
          )
          pointerEvents=`boxNone>
          children
        </Animated.View>
      </View>
    </DocumentContext>
  </Modal>;
};

let useModalActionState = () => {
  let (visible, setVisible) = React.useState(_ => false);
  let openAction = () => setVisible(_ => true);
  let closeAction = () => setVisible(_ => false);

  (visible, openAction, closeAction);
};
