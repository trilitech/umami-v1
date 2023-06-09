/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "title": style(~textAlign=#center, ()),
    "modal": style(~flex=1., ()),
    "modalOverlay": StyleSheet.flatten([
      StyleSheet.absoluteFillObject,
      style(~backgroundColor="black", ())->unsafeAddStyle({"willChange": "transform"}),
    ]),
    "modalView": style(
      ~marginTop=auto,
      ~marginBottom=auto, // fix android bug : marginVertical doesn't work with auto
      ~backgroundColor="transparent", // fix android bug : no animation without it
      ~opacity=1.,
      ~transform=[scale(~scale=1.)],
      (),
    )->unsafeAddStyle({"willChange": "transform"}),
  })
}

module Component = {
  @react.component
  let make = (~visible, ~onRequestClose: unit => unit=unit => unit, ~children) => {
    let (visible, animatedOpenValue) = AnimationHooks.useAnimationOpen(visible, onRequestClose)

    let overlayOpacity = animatedOpenValue
    let viewOpacity = animatedOpenValue
    let viewScale = {
      open Animated.Interpolation
      animatedOpenValue->interpolate(
        config(
          ~inputRange=[0., 1.],
          ~outputRange=[0.8, 1.]->fromFloatArray,
          ~extrapolate=#clamp,
          (),
        ),
      )
    }

    let theme = ThemeContext.useTheme()

    <Modal
      animationType=#none
      transparent=true
      supportedOrientations=[Modal.Orientation.portrait, Modal.Orientation.landscape]
      visible
      onRequestClose>
      <DocumentContext>
        <View style={styles["modal"]}>
          <Animated.View
            accessibilityRole=#none
            accessible=true
            style={
              open Style
              array([
                styles["modalOverlay"],
                StyleSheet.absoluteFillObject,
                style(~backgroundColor=theme.colors.scrim, ()),
                style(~opacity=overlayOpacity->Animated.StyleProp.float, ()),
              ])
            }
          />
          <Animated.View
            style={
              open Style
              array([
                styles["modalView"],
                style(
                  ~opacity=viewOpacity->Animated.StyleProp.float,
                  ~transform=[scale(~scale=viewScale->Animated.StyleProp.float)],
                  (),
                ),
              ])
            }
            pointerEvents=#boxNone>
            children
          </Animated.View>
        </View>
      </DocumentContext>
    </Modal>
  }
}

let useModalActionState = () => {
  let (visible, setVisible) = React.useState(_ => false)
  let openAction = () => setVisible(_ => true)
  let closeAction = () => setVisible(_ => false)

  (visible, openAction, closeAction)
}

let useModal = (~onClose=() => (), ()) => {
  let (visible, setVisible) = React.useState(_ => false)
  let openModal = () => setVisible(_ => true)
  let closeModal = () => setVisible(_ => false)

  let wrap = children => <Component visible onRequestClose=onClose> children </Component>

  (openModal, closeModal, wrap)
}

include Component
