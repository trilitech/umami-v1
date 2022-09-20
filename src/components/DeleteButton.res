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

let useModal = (~title, ~loading, ~onPressConfirmDelete) =>
  ModalDialogConfirm.useModal(
    ~action=onPressConfirmDelete,
    ~loading,
    ~title,
    ~cancelText=I18n.Btn.cancel,
    ~actionText=I18n.Btn.delete,
  )

module MenuItem = {
  @react.component
  let make = (~color=?, ~buttonText, ~modalTitle, ~onPressConfirmDelete, ~request) => {
    let loading = request->ApiRequest.isLoading

    let (openAction, _, modal) = useModal(~title=modalTitle, ~loading, ~onPressConfirmDelete, ())

    let icon = (~color as colorin=?) => {
      let color = list{color, colorin}->Option.firstSome
      Icons.Delete.build(~color?)
    }

    let onPress = _ => openAction()

    <> <Menu.Item text=buttonText icon onPress colorStyle=#error /> {modal()} </>
  }
}

module Generic = {
  @react.component
  let make = (
    ~color=?,
    ~tooltip,
    ~modalTitle,
    ~onPressConfirmDelete,
    ~request,
    ~icon: Icons.builder,
    ~iconSize=?,
    ~iconSizeRatio=?,
  ) => {
    let loading = request->ApiRequest.isLoading

    let (openAction, _, modal) = useModal(~title=modalTitle, ~loading, ~onPressConfirmDelete, ())

    let icon = (~color as colorin=?) => {
      let color = list{color, colorin}->Option.firstSome
      icon(~color?)
    }

    let onPress = _ => openAction()

    <> <IconButton tooltip icon onPress size=?iconSize ?iconSizeRatio /> {modal()} </>
  }
}

module IconButton = {
  @react.component
  let make = (~color=?, ~tooltip, ~modalTitle, ~onPressConfirmDelete, ~request) =>
    <Generic ?color tooltip modalTitle onPressConfirmDelete request icon=Icons.Delete.build />
}
