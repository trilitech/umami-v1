open ReactNative;

type openingStyle =
  | Top
  | TopRight;

let listVerticalPadding = 8.;

let styles =
  Style.(
    StyleSheet.create({
      "listContainer":
        style(~borderRadius=3., ())
        ->unsafeAddStyle({
            "boxShadow": "0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 3px 14px 2px rgba(0, 0, 0, 0.12), 0 8px 10px 1px rgba(0, 0, 0, 0.14)",
          }),
      "listContentContainer":
        style(~paddingVertical=listVerticalPadding->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~scrollRef=?,
      ~isOpen=false,
      ~openingStyle=Top,
      ~style as styleFromProp=?,
      ~onScroll=?,
      ~scrollEventThrottle=?,
      ~children,
    ) => {
  let (visible, animatedOpenValue) =
    AnimationHooks.useAnimationOpen(~speed=80., ~bounciness=0., isOpen, _ =>
      ()
    );

  let theme = ThemeContext.useTheme();

  <View style={ReactUtils.displayOn(visible)}>
    <Animated.View
      style=Style.(
        style(
          ~opacity=animatedOpenValue->Animated.StyleProp.float,
          ~transform=[|
            translateY(
              ~translateY=
                Animated.Interpolation.(
                  animatedOpenValue->interpolate(
                    config(
                      ~inputRange=[|0., 1.|],
                      ~outputRange=
                        (
                          switch (openingStyle) {
                          | Top => [|(-16.), 0.|]
                          | TopRight => [|0., 0.|]
                          }
                        )
                        ->fromFloatArray,
                      ~extrapolate=`clamp,
                      (),
                    ),
                  )
                )
                ->Animated.StyleProp.float,
            ),
            scaleX(
              ~scaleX=
                Animated.Interpolation.(
                  animatedOpenValue->interpolate(
                    config(
                      ~inputRange=[|0., 1.|],
                      ~outputRange=
                        (
                          switch (openingStyle) {
                          | Top => [|1., 1.|]
                          | TopRight => [|0.9, 1.|]
                          }
                        )
                        ->fromFloatArray,
                      ~extrapolate=`clamp,
                      (),
                    ),
                  )
                )
                ->Animated.StyleProp.float,
            ),
            scaleY(
              ~scaleY=
                Animated.Interpolation.(
                  animatedOpenValue->interpolate(
                    config(
                      ~inputRange=[|0., 1.|],
                      ~outputRange=[|0.9, 1.|]->fromFloatArray,
                      ~extrapolate=`clamp,
                      (),
                    ),
                  )
                )
                ->Animated.StyleProp.float,
            ),
          |],
          (),
        )
        ->unsafeAddStyle({
            "transformOrigin":
              switch (openingStyle) {
              | Top => "top center"
              | TopRight => "top right"
              },
          })
      )>
      <ScrollView
        ref=?scrollRef
        style=Style.(
          arrayOption([|
            Some(styles##listContainer),
            Some(style(~backgroundColor=theme.colors.background, ())),
            styleFromProp,
          |])
        )
        contentContainerStyle=Style.(
          arrayOption([|
            Some(styles##listContentContainer),
            theme.dark
              ? Some(style(~backgroundColor=theme.colors.stateActive, ()))
              : None,
          |])
        )
        ?onScroll
        ?scrollEventThrottle>
        children
      </ScrollView>
    </Animated.View>
  </View>;
};
