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

module ModalBase = {
  @react.component
  let make = (
    ~closeAction,
    ~action,
    ~loading=?,
    ~title,
    ~subtitle=?,
    ~contentText=?,
    ~cancelText,
    ~cancelStyle=?,
  ) => {
    <ModalTemplate.Dialog>
      <Typography.Headline style=FormStyles.header> {title->React.string} </Typography.Headline>
      {subtitle->ReactUtils.mapOpt(sub =>
        <Typography.Headline> {sub->React.string} </Typography.Headline>
      )}
      {contentText->ReactUtils.mapOpt(contentText =>
        <Typography.Body1 style=FormStyles.textContent>
          {contentText->React.string}
        </Typography.Body1>
      )}
      <View style=FormStyles.formAction>
        {action}
        <Buttons.Form
          style=?cancelStyle text=cancelText onPress={_ => closeAction()} disabled=?loading
        />
      </View>
    </ModalTemplate.Dialog>
  }
}

module Modal = {
  @react.component
  let make = (
    ~closeAction,
    ~action,
    ~loading=?,
    ~title,
    ~subtitle=?,
    ~contentText=?,
    ~cancelText,
  ) => {
    let theme = ThemeContext.useTheme()
    let cancelStyle = Style.style(~backgroundColor=theme.colors.stateActive, ())
    <ModalBase closeAction action ?loading title ?subtitle ?contentText cancelText cancelStyle />
  }
}

let useModal = (
  ~action,
  ~loading=?,
  ~title,
  ~subtitle=?,
  ~contentText=?,
  ~cancelText,
  ~actionText,
  (),
) => {
  let (openModal, closeModal, wrapModal) = ModalAction.useModal()

  let onPress = _ =>
    action()->Promise.get(x =>
      switch x {
      | Ok(_) => closeModal()
      | Error(_) => ()
      }
    )

  let action = <Buttons.Form onPress text=actionText ?loading />

  let modal = () =>
    wrapModal(
      <Modal action ?loading title ?subtitle ?contentText cancelText closeAction=closeModal />,
    )

  (openModal, closeModal, modal)
}
