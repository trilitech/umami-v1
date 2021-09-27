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

let formatOnBlur = (token: option(TokenRepr.t), handleChange, value) =>
  switch (token) {
  | None =>
    value->Tez.formatString->Option.getWithDefault(value)->handleChange
  | Some({decimals, _}) =>
    value
    ->Token.Unit.formatString(decimals)
    ->Result.getWithDefault(value)
    ->handleChange
  };

let tezDecoration = (~style) =>
  <Typography.Body1 colorStyle=`mediumEmphasis style>
    I18n.t#tez->React.string
  </Typography.Body1>;

[@react.component]
let make =
    (
      ~label,
      ~value: string,
      ~handleChange,
      ~error,
      ~style: option(ReactNative.Style.t)=?,
      ~decoration=?,
      ~token: option(Token.t)=?,
    ) => {
  // reformat value if token change
  React.useEffect1(
    () => {
      if (value != "") {
        formatOnBlur(token, handleChange, value);
      };
      None;
    },
    [|token|],
  );

  let placeholder =
    token == None
      ? I18n.input_placeholder#tez_amount : I18n.input_placeholder#token_amount;

  <FormGroupTextInput
    label
    ?style
    placeholder
    value
    error
    ?decoration
    handleChange
    onBlur={_ => formatOnBlur(token, handleChange, value)}
    keyboardType=`numeric
  />;
};
