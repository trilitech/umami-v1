open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~textAlign=`center, ()),
      "modal": style(~flex=1., ()),
      "modalOverlay":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(~backgroundColor="black", ())
          ->unsafeAddStyle({"willChange": "transform"}),
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
        )
        ->unsafeAddStyle({"willChange": "transform"}),
    })
  );

[@react.component]
let make = (~visible, ~onRequestClose: unit => unit=unit => unit, ~children) => {
  let (visible, animatedOpenValue) =
    AnimationHooks.useAnimationOpen(visible, onRequestClose);

  let overlayOpacity = animatedOpenValue;
  let viewOpacity = animatedOpenValue;
  let viewScale =
    Animated.Interpolation.(
      animatedOpenValue->interpolate(
        config(
          ~inputRange=[|0., 1.|],
          ~outputRange=[|0.8, 1.|]->fromFloatArray,
          ~extrapolate=`clamp,
          (),
        ),
      )
    );

  let theme = ThemeContext.useTheme();

  <Modal
    animationType=`none
    transparent=true
    supportedOrientations=[|
      Modal.Orientation.portrait,
      Modal.Orientation.landscape,
    |]
    visible
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
