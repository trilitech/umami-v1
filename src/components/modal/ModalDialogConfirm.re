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

module Modal = {
  [@react.component]
  let make =
      (
        ~closeAction,
        ~action,
        ~loading=?,
        ~title,
        ~subtitle=?,
        ~cancelText,
        ~actionText,
      ) => {
    let theme = ThemeContext.useTheme();
    <ModalTemplate.Dialog>
      <Typography.Headline style=FormStyles.header>
        title->React.string
      </Typography.Headline>
      {subtitle->ReactUtils.mapOpt(sub => {
         <Typography.Headline> sub->React.string </Typography.Headline>
       })}
      <View style=FormStyles.formAction>
        <Buttons.Form
          style=Style.(style(~backgroundColor=theme.colors.stateActive, ()))
          text=cancelText
          onPress={_ => closeAction()}
          disabled=?loading
        />
        <Buttons.Form onPress={_ => action()} text=actionText ?loading />
      </View>
    </ModalTemplate.Dialog>;
  };
};

let useModal =
    (~action, ~loading=?, ~title, ~subtitle=?, ~cancelText, ~actionText, ()) => {
  let (openModal, closeModal, wrapModal) = ModalAction.useModal();

  let action = () =>
    action()
    ->Promise.get(
        fun
        | Ok(_) => closeModal()
        | Error(_) => (),
      );

  let modal = () =>
    wrapModal(
      <Modal
        action
        ?loading
        title
        ?subtitle
        cancelText
        actionText
        closeAction=closeModal
      />,
    );

  (openModal, closeModal, modal);
};
