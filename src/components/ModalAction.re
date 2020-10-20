open ReactNative;

type valueImperativeHandle = {closeModal: unit => unit};

[@bs.send] external closeModal: unit => unit = "closeModal";

let useAnimatedValue = value => {
  React.useRef(Animated.Value.create(value)).current;
};

let styles =
  Style.(
    StyleSheet.create({
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
let make =
  React.forwardRef((~visible, ~onRequestClose=unit => unit, ~children, ref_) => {
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
              ~toValue=Animated.Value.Spring.fromRawValue(visible ? 0.7 : 0.),
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

    let closeModal = () => {
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

    React.useImperativeHandle1(
      ref_,
      () => {{closeModal: closeModal}},
      [|closeModal|],
    );

    React.useEffect1(
      () => {
        setShowContent(_ => visible);
        animateOverlay(visible);
        None;
      },
      [|visible|],
    );

    <Modal
      animationType=`none
      transparent=true
      supportedOrientations=[|
        Modal.Orientation.portrait,
        Modal.Orientation.landscape,
      |]
      visible
      onRequestClose=closeModal>
      <View
        style=Style.(
          array([|styles##modal, style(~opacity=showContent ? 1. : 0., ())|])
        )>
        <TouchableWithoutFeedback onPress={_ => closeModal()}>
          <Animated.View
            style=Style.(
              array([|
                styles##modalOverlay,
                style(~opacity=overlayOpacity->Animated.StyleProp.float, ()),
              |])
            )
          />
        </TouchableWithoutFeedback>
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
    </Modal>;
  });
