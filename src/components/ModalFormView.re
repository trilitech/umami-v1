/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

type confirm = {
  title: string,
  subtitle: option(string),
  cancelText: string,
  actionText: string,
  action: unit => unit,
};

type closing =
  | Close(unit => unit)
  | Deny(string)
  | Confirm(confirm);

let confirm =
    (
      ~title=I18n.title#confirm_cancel,
      ~subtitle=?,
      ~cancelText=I18n.btn#go_back,
      ~actionText,
      action,
    ) =>
  Confirm({title, subtitle, cancelText, actionText, action});

module BackButton = {
  [@react.component]
  let make = (~back) => {
    back->ReactUtils.mapOpt(back => {
      <ModalTemplate.HeaderButtons.Back onPress={_ => back()} />
    });
  };
};

module ConfirmCloseModal = {
  [@react.component]
  let make = (~confirm, ~closeAction, ~visible) => {
    let {title, subtitle, cancelText, actionText, action} = confirm;
    <ModalDialogConfirm
      visible
      closeAction
      action={() => {
        closeAction();
        action();
      }}
      title
      ?subtitle
      cancelText
      actionText
    />;
  };
};

module CloseButton = {
  [@react.component]
  let make = (~closing) => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let (confirm, disabled, tooltip) =
      switch (closing) {
      | Deny(msg) => (React.null, true, Some(("close_button", msg)))
      | Close(_) => (React.null, false, None)
      | Confirm(confirm) => (
          <ConfirmCloseModal confirm visible=visibleModal closeAction />,
          false,
          None,
        )
      };

    let onPress = _ => {
      switch (closing) {
      | Deny(_) => ()
      | Close(f) => f()
      | Confirm(_) => openAction()
      };
    };

    <>
      <ModalTemplate.HeaderButtons.Close ?tooltip disabled onPress />
      confirm
    </>;
  };
};

module Large = {
  [@react.component]
  let make = (~closing=?, ~title=?, ~back=?, ~loading=?, ~children, ~style=?) => {
    let closeButton = closing->Option.map(closing => <CloseButton closing />);

    let backButton = back->Option.map(back => <BackButton back />);

    let headerCenter =
      title->Option.map(title =>
        <Typography.Headline style=FormStyles.header>
          title->React.string
        </Typography.Headline>
      );

    <ModalTemplate.Large
      ?headerCenter
      headerRight=?closeButton
      headerLeft=?backButton
      ?loading
      ?style>
      children
    </ModalTemplate.Large>;
  };
};

[@react.component]
let make =
    (
      ~closing=?,
      ~title=?,
      ~headerActionButton=?,
      ~back=?,
      ~loading=?,
      ~children,
      ~style=?,
    ) => {
  let closeButton = closing->Option.map(closing => <CloseButton closing />);

  let backButton = back->Option.map(back => <BackButton back />);

  let styleWithButton =
    headerActionButton->Option.map(_ =>
      Style.style(
        ~justifyContent=`spaceBetween,
        ~display=`flex,
        ~flexDirection=`row,
        (),
      )
    );

  let headerCenter = {
    title->Option.map(title =>
      <View style=?styleWithButton>
        <Typography.Headline style=FormStyles.header>
          title->React.string
        </Typography.Headline>
        headerActionButton->ReactUtils.opt
      </View>
    );
  };

  <ModalTemplate.Form
    ?headerCenter
    headerRight=?closeButton
    headerLeft=?backButton
    ?loading
    ?style>
    children
  </ModalTemplate.Form>;
};
