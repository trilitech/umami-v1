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

let supportEmail = "incoming+nomadic-labs-umami-wallet-umami-20392089-issue-@incoming.gitlab.com";

let buildBody = err_msg => {
  let err = I18n.support_mailto_error_body;
  {j|$(err)\n\n$(err_msg)|j};
};

let buildMailtoUrl = body => {
  let subject = Js.Global.encodeURI(I18n.support_mailto_error_subject);
  let body = Js.Global.encodeURI(buildBody(body));
  {j|mailto:$(supportEmail)?subject=$(subject)&body=$(body)|j};
};

[@react.component]
let make = (~style=?, ~toast, ~message) => {
  let onPress = _ => Linking.openURL(buildMailtoUrl(message))->ignore;

  <IconButton
    ?style
    isPrimary=false
    icon=Icons.QuestionMarkOutlined.build
    onPress
    toast
  />;
};
