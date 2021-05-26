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

let styles =
  Style.(
    StyleSheet.create({
      "content": style(~marginTop=20.->dp, ()),
      "empty": style(~textAlign=`center, ()),
      "view": style(~minHeight=400.->dp, ()),
      "modal":
        style(
          ~width=642.->dp,
          ~paddingTop=45.->dp,
          ~paddingBottom=32.->dp,
          ~paddingHorizontal=55.->dp,
          (),
        ),
      "clear": style(~width=auto, ~alignSelf=`flexEnd, ~padding=5.->dp, ()),
    })
  );

module ClearButton = {
  [@react.component]
  let make = () => {
    let clearLogs = LogsContext.useClear();
    <Buttons.Form
      style=styles##clear
      fontSize=12.
      text=I18n.t#logs_clearall
      onPress={_ => clearLogs()}
    />;
  };
};

[@react.component]
let make = (~closeAction) => {
  let errors = LogsContext.useLogs();
  let deleteError = LogsContext.useDelete();
  let addLog = LogsContext.useAdd();

  <ModalTemplate.Base
    style=styles##modal
    headerRight={
      <ModalTemplate.HeaderButtons.Close onPress={_ => closeAction()} />
    }>
    <View style=styles##view>
      <Typography.Headline style=ModalAction.styles##title>
        I18n.title#error_logs->React.string
      </Typography.Headline>
      <View style=styles##content>
        {ReactUtils.onlyWhen(<ClearButton />, errors != [])}
        {switch (errors) {
         | [] =>
           <Typography.Body1 style=styles##empty>
             I18n.t#logs_no_recent->React.string
           </Typography.Body1>
         | errors =>
           errors
           ->List.keep(({Logs.kind}) => kind == Logs.Error)
           ->List.toArray
           ->Array.mapWithIndex((i, log) =>
               <LogItem
                 key={i->string_of_int}
                 indice=i
                 log
                 addToast={addLog(true)}
                 handleDelete=deleteError
               />
             )
           ->React.array
         }}
      </View>
    </View>
  </ModalTemplate.Base>;
};
