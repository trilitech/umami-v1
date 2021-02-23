open ReactNative;

type targetLayout = {
  x: float,
  y: float,
  width: float,
  height: float,
};

type openingStyle =
  | Top
  | TopRight;

let styles =
  Style.(
    StyleSheet.create({
      "popover": style(~position=`absolute, ()),
      "popoverTop": style(~top=3.->dp, ~left=0.->dp, ~right=0.->dp, ()),
      "popoverTopRight": style(~top=3.->dp, ~right=0.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~isOpen=false,
      ~config: option(targetLayout)=?,
      ~openingStyle=Top,
      ~style as styleFromProp=?,
      ~children,
    ) => {
  let (visible, animatedOpenValue) =
    AnimationHooks.useAnimationOpen(~speed=80., ~bounciness=0., isOpen, _ =>
      ()
    );

  <View
    style={Style.array([|
      StyleSheet.absoluteFillObject,
      ReactUtils.displayOn(visible),
    |])}
    pointerEvents=`boxNone>
    <Animated.View
      style=Style.(
        style(
          ~position=`absolute,
          ~width=?{
            switch (openingStyle) {
            | Top => config->Option.map(({width}) => width->dp)
            | TopRight => None
            };
          },
          ~top=?{
            config->Option.map(({y, height}) => (y +. height)->dp);
          },
          ~left=?{
            config->Option.map(({x, width}) =>
              (
                switch (openingStyle) {
                | Top => x
                | TopRight => x +. width
                }
              )
              ->dp
            );
          },
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
      <View
        style=Style.(
          arrayOption([|
            Some(styles##popover),
            switch (openingStyle) {
            | Top => Some(styles##popoverTop)
            | TopRight => Some(styles##popoverTopRight)
            },
            styleFromProp,
          |])
        )>
        children
      </View>
    </Animated.View>
  </View>;
};

let usePopoverState = () => {
  let pressableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);
  let (popoverConfig, setPopoverConfig) = React.useState(_ => None);

  DocumentContext.useClickOutside(
    pressableRef,
    isOpen,
    React.useCallback1(_pressEvent => setIsOpen(_ => false), [|setIsOpen|]),
  );

  let togglePopover = () => {
    pressableRef.current
    ->Js.Nullable.toOption
    ->Option.map(pressableElement => {
        pressableElement->PressableCustom.measureInWindow(
          (~x, ~y, ~width, ~height) => {
          setPopoverConfig(_ => Some({x, y, width, height}))
        })
      })
    ->ignore;
    setIsOpen(isOpen => !isOpen);
  };

  (pressableRef, isOpen, popoverConfig, togglePopover);
};
